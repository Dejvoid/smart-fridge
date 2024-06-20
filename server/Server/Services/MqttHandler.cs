using System.Configuration;
using MQTTnet;
using MQTTnet.Server;
using MQTTnet.Client;
using Server.Data;
using System.Net;
using System.Text;
using System.Security.Cryptography.X509Certificates;
using System.Net.Security;

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

class MqttHandler : IDisposable
{
    MqttServer srv;
    IMqttClient client;
    IDashBoard dashboard;
    IDataController dataControl;
    INotifier notifications = null;
    X509Certificate2 caCert;
    //HashSet<> verified;

    public MqttHandler(IDashBoard dash, IDataController data) {
        dashboard = dash;
        dataControl = data;
    }

    public async void Start(string caCertFile, string serverCertFile) {
        var mqttFactory = new MqttFactory();
        caCert = new X509Certificate2(caCertFile);
        var cert = new X509Certificate2(serverCertFile);
        //var client_crt = new X509Certificate2("/home/dejvoid/temp/device.crt");
        var mqttServerOptions = new MqttServerOptionsBuilder()
            .WithDefaultEndpoint()
            .WithDefaultEndpointPort(1883)
            .WithEncryptedEndpoint()
            .WithEncryptedEndpointPort(8883)
            .WithEncryptionCertificate(cert)
            .WithClientCertificate(DeviceValidation)
            //.WithEncryptionSslProtocol(System.Security.Authentication.SslProtocols.Tls13) - buggy with ESP32
            .Build();
        var mqttClientOptions = new MqttClientOptionsBuilder().WithClientId("server").WithConnectionUri("mqtt://localhost:1883").Build();
        srv = mqttFactory.CreateMqttServer(mqttServerOptions);


        client = mqttFactory.CreateMqttClient();

        await srv.StartAsync();
        Console.WriteLine("MQTT service started");
        await client.ConnectAsync(mqttClientOptions);

        client.ApplicationMessageReceivedAsync += (e => {
            return MessageReceivedCallback(e);
        });
        await client.SubscribeAsync(new MqttClientSubscribeOptionsBuilder()
            .WithTopicFilter(MqttTopics.NOTIF_ALL)
            //.WithTopicFilter(MqttTopics.PRODUCT_ALL)
            .WithTopicFilter(MqttTopics.HUM)
            .WithTopicFilter(MqttTopics.THERM)
            .Build());
        //await client.PublishStringAsync("notifications", "hello");
    }

    private bool DeviceValidation(object sender, X509Certificate? certificate, X509Chain? chain, SslPolicyErrors sslPolicyErrors)
    {
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
        }
        else 
            isValid = true;
        return isValid;
    }

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

    private async Task RemoveProduct(string payload)
    {
        dataControl.RemoveProduct(payload);
    }

    private async Task AddProduct(string payload)
    {
        dataControl.AddProduct(payload);
    }

    private async Task TriggerNotification(string payload, string topic)
    {
    }

    private async Task UpdateTemperature(string payload)
    {
        float temp;
        if (float.TryParse(payload, out temp))
            dashboard.UpdateTemperature(temp);
    }

    private async Task UpdateHumidity(string payload)
    {
        float hum;
        if (float.TryParse(payload, out hum))
            dashboard.UpdateHumidity(hum);
    }

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