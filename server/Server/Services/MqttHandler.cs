using System.Configuration;
using MQTTnet;
using MQTTnet.Server;
using MQTTnet.Client;
using Server.Data;
using System.Net;
using System.Text;
using System.Security.Cryptography.X509Certificates;
using System.Net.Security;

/// <summary>
/// Constants definition for the MQTT topics used in the system
/// </summary>
readonly struct MqttTopics {
    public const string NOTIF = "notifications";
    public const string NOTIF_ALL = NOTIF + "/#";
    public const string THERM = "temperature";
    public const string HUM = "humidity";
    public const string PRODUCT = "product";
    public const string PRODUCT_ALL = PRODUCT + "/#";
    public const string PRODUCT_ADD = PRODUCT + "/add";
    public const string PRODUCT_RM = PRODUCT + "/rm";
}

/// <summary>
/// MQTT handler. Server as genuine MQTT gateway. Publishing to product topic is authorized via the certificate
/// </summary>
class MqttHandler : IDisposable
{
    MqttServer srv;
    IMqttClient client;
    IDashBoard dashboard;
    IDataController dataControl;
    INotifier notifications = null;
    X509Certificate2 caCert;
    X509Certificate2 cert;
    HashSet<string> verifiedClients = new();

    public MqttHandler(IDashBoard dash, IDataController data) {
        dashboard = dash;
        dataControl = data;
    }

    /// <summary>
    /// Start the service
    /// </summary>
    /// <param name="caCertFile">Certification authority file in .crt format</param>
    /// <param name="serverCertFile">Server certificate file in .pfx format</param>
    public async void Start(string? caCertFile = null, string? serverCertFile = null) {
        var mqttFactory = new MqttFactory();
        
        var mqttServerOptions = new MqttServerOptionsBuilder()
            .WithDefaultEndpoint()
            .WithDefaultEndpointPort(1883);
            if (caCertFile != null && serverCertFile != null) {
                caCert = new X509Certificate2(caCertFile);
                cert = new X509Certificate2(serverCertFile);
                mqttServerOptions
                    .WithEncryptedEndpoint()
                    .WithEncryptedEndpointPort(8883)
                    .WithEncryptionCertificate(cert)
                    .WithClientCertificate(DeviceValidation);
            }
            //.WithEncryptionSslProtocol(System.Security.Authentication.SslProtocols.Tls13) - buggy with ESP32

        string localClient = "server";
        // our client is verified. if it wasn't we could still do anything because we have access to the server
        verifiedClients.Add(localClient);
        var mqttClientOptions = new MqttClientOptionsBuilder()
            .WithClientId(localClient)
            .WithConnectionUri("mqtt://localhost:1883");
        srv = mqttFactory.CreateMqttServer(mqttServerOptions.Build());

        srv.ValidatingConnectionAsync += ValidateConnectionAsync;
        srv.InterceptingPublishAsync += VerifyPublish;
        srv.ClientDisconnectedAsync += RemoveVerified;

        client = mqttFactory.CreateMqttClient();

        await srv.StartAsync();
        Console.WriteLine("MQTT service started");
        await client.ConnectAsync(mqttClientOptions.Build());

        client.ApplicationMessageReceivedAsync += (e => {
            return MessageReceivedCallback(e);
        });
        await client.SubscribeAsync(new MqttClientSubscribeOptionsBuilder()
            .WithTopicFilter(MqttTopics.NOTIF_ALL)
            .WithTopicFilter(MqttTopics.PRODUCT_ALL)
            .WithTopicFilter(MqttTopics.HUM)
            .WithTopicFilter(MqttTopics.THERM)
            .Build());
        //await client.PublishStringAsync("notifications", "hello");
    }

    /// <summary>
    /// Callback on client disconnect. Removes client id from verified IDs.
    /// </summary>
    /// <param name="args"></param>
    /// <returns></returns>
    private async Task RemoveVerified(ClientDisconnectedEventArgs args)
    {
        var id = args.ClientId;
        if (verifiedClients.Contains(id)) {
            verifiedClients.Remove(id);
        }
    }

    /// <summary>
    /// Authorization for the products topic
    /// </summary>
    /// <param name="args"></param>
    /// <returns></returns>
    private Task VerifyPublish(InterceptingPublishEventArgs args)
    {
        // only verified users/devices can publish to product/#
        if (args.ApplicationMessage.Topic.StartsWith(MqttTopics.PRODUCT)) {
            args.ProcessPublish = verifiedClients.Contains(args.ClientId);
        }
        if (!verifiedClients.Contains(args.ClientId)) {
            System.Console.WriteLine("Unauthenticated user");
        }
        return Task.CompletedTask;
    }

    /// <summary>
    /// Validates user's connection
    /// </summary>
    /// <param name="args"></param>
    /// <returns></returns>
    private Task ValidateConnectionAsync(ValidatingConnectionEventArgs args)
    {
        bool certVerified = false;
        if (args.ClientCertificate is not null) {
            var chain = new X509Chain();
            chain.ChainPolicy.RevocationMode = X509RevocationMode.NoCheck;
            chain.ChainPolicy.RevocationFlag = X509RevocationFlag.ExcludeRoot;
            chain.ChainPolicy.VerificationFlags = X509VerificationFlags.AllowUnknownCertificateAuthority;
            chain.ChainPolicy.VerificationTime = DateTime.Now;
            chain.ChainPolicy.UrlRetrievalTimeout = new TimeSpan(0, 0, 0);
            chain.ChainPolicy.ExtraStore.Add(caCert);
            var isValid = chain.Build(args.ClientCertificate);
            certVerified = chain.ChainElements
                .Cast<X509ChainElement>()
                .Any(x => x.Certificate.Thumbprint == caCert.Thumbprint);
            
        }
        
        if (certVerified) {
            verifiedClients.Add(args.ClientId);
        }
        return Task.CompletedTask;
        //throw new NotImplementedException();
    }

    /// <summary>
    /// More of a dummy method since without this callback the service doesn't consider certificates
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="certificate"></param>
    /// <param name="chain"></param>
    /// <param name="sslPolicyErrors"></param>
    /// <returns></returns>
    private bool DeviceValidation(object sender, X509Certificate? certificate, X509Chain? chain, SslPolicyErrors sslPolicyErrors)
    {
        // In order to be able to add users to verifiedClients, we need this "dummy"
        // Only with this we can accept all clients but still see their certificates
        return true;
        /*
        bool isValid = false;
        if (certificate is not null && chain is not null) {
            chain.ChainPolicy.RevocationMode = X509RevocationMode.NoCheck;
            chain.ChainPolicy.RevocationFlag = X509RevocationFlag.ExcludeRoot;
            chain.ChainPolicy.VerificationFlags = X509VerificationFlags.AllowUnknownCertificateAuthority;
            chain.ChainPolicy.VerificationTime = DateTime.Now;
            chain.ChainPolicy.UrlRetrievalTimeout = new TimeSpan(0, 0, 0);
            chain.ChainPolicy.ExtraStore.Add(caCert);
            isValid = chain.Build(new X509Certificate2(certificate));
            var valid = chain.ChainElements
                .Cast<X509ChainElement>()
                .Any(x => x.Certificate.Thumbprint == caCert.Thumbprint);
            // now if valid, user has our certificate and we can do something
            if (valid) {
            }
        }
        else 
            isValid = true;
        return isValid;
        */
    }

/// <summary>
/// Called on mesage received.
/// </summary>
/// <param name="e"></param>
/// <returns></returns>
    private async Task MessageReceivedCallback(MqttApplicationMessageReceivedEventArgs e)
    {
        string topic = e.ApplicationMessage.Topic;
        string payload = Encoding.UTF8.GetString(e.ApplicationMessage.PayloadSegment);
        System.Console.WriteLine($"{e.ClientId} published to {topic}");
        switch (topic)
        {
            case MqttTopics.HUM:
                await UpdateHumidity(payload);
            break;
            case MqttTopics.THERM:
                await UpdateTemperature(payload);
            break;
            case MqttTopics.PRODUCT_ADD:
                await AddProduct(payload);
            break;
            case MqttTopics.PRODUCT_RM:
                await RemoveProduct(payload);
            break;
            default:
                if (topic.StartsWith(MqttTopics.NOTIF)) {
                    await TriggerNotification(payload, topic);
                }
            break;
        }
    }
    /// <summary>
    /// Wrapper for product removal
    /// </summary>
    /// <param name="payload">bar-code of the product</param>
    /// <returns></returns>
    private async Task RemoveProduct(string payload)
    {
        dataControl.RemoveProduct(payload);
    }


    /// <summary>
    /// Wrapper for product adding
    /// </summary>
    /// <param name="payload">bar-code of the product</param>
    /// <returns></returns>
    private async Task AddProduct(string payload)
    {
        dataControl.AddProduct(payload);
    }

    /// <summary>
    /// [TODO]
    /// Triggers the notification for the HTTP clients of the server
    /// </summary>
    /// <param name="payload">text of the notification</param>
    /// <param name="topic">full notification topic. Used to derrive the priority of the notification</param>
    /// <returns></returns>
    private async Task TriggerNotification(string payload, string topic)
    {
    }

    /// <summary>
    /// Temperature update
    /// </summary>
    /// <param name="payload">expected float in string value of the temperature</param>
    /// <returns></returns>
    private async Task UpdateTemperature(string payload)
    {
        float temp;
        if (float.TryParse(payload, out temp))
            dashboard.UpdateTemperature(temp);
    }

    /// <summary>
    /// Humidity update
    /// </summary>
    /// <param name="payload">expected float in string value of the humidity</param>
    /// <returns></returns>
    private async Task UpdateHumidity(string payload)
    {
        float hum;
        if (float.TryParse(payload, out hum)) {
            dashboard.UpdateHumidity(hum);
        }
    }

    /// <summary>
    /// Interface for sending the notifications from the server
    /// </summary>
    /// <param name="notif">Notification to be published</param>
    public async void SendNotif(Notification notif) {
        if (!srv.IsStarted) 
            return;

        var msg = new MqttApplicationMessageBuilder()
                .WithTopic($"notifications/{(int)notif.Priority}")
                .WithPayload(notif.Text)
                .Build();
        await client.PublishAsync(msg);
    }

    async void IDisposable.Dispose()
    {
        await srv.StopAsync();
        srv.Dispose();
    }
}