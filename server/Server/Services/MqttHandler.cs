using System.Configuration;
using MQTTnet;
using MQTTnet.Server;
using MQTTnet.Client;
using Server.Data;
using System.Net;
using System.Text;

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

    public MqttHandler(IDashBoard dash, IDataController data) {
        dashboard = dash;
        dataControl = data;
    }

    public async void Start() {
        var mqttFactory = new MqttFactory();
        var mqttServerOptions = new MqttServerOptionsBuilder().WithDefaultEndpoint().WithDefaultEndpointPort(1883).Build();
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
            .WithTopicFilter(MqttTopics.PRODUCT_ALL)
            .WithTopicFilter(MqttTopics.HUM)
            .WithTopicFilter(MqttTopics.THERM)
            .Build());
        await client.PublishStringAsync("notifications", "hello");
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