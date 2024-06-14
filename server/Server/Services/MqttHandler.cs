using System.Configuration;
using MQTTnet;
using MQTTnet.Server;
using MQTTnet.Client;
using Server.Data;

class MqttHandler : IDisposable
{
    MqttServer srv;
    IMqttClient client;

    public async void Start() {
        var mqttFactory = new MqttFactory();
        var mqttServerOptions = new MqttServerOptionsBuilder().WithDefaultEndpoint().Build();
        var mqttClientOptions = new MqttClientOptionsBuilder().WithClientId("server").WithTcpServer("localhost").Build();

        srv = mqttFactory.CreateMqttServer(mqttServerOptions);
        client = mqttFactory.CreateMqttClient();
        
        await srv.StartAsync();
        Console.WriteLine("MQTT service started");
        await client.ConnectAsync(mqttClientOptions);

        client.ApplicationMessageReceivedAsync += (e => {
            return MessageReceivedCallback(e);
        });
        await client.SubscribeAsync(new MqttClientSubscribeOptionsBuilder().WithTopicFilter("notifications").Build());
        await client.PublishStringAsync("faggot", "hello");
    }

    private async Task MessageReceivedCallback(MqttApplicationMessageReceivedEventArgs e)
    {
        if (e.ClientId != "server") {
            System.Console.WriteLine($"{e.ClientId} published to {e.ApplicationMessage.Topic}");
        }
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

    async void Dispose() {
        await srv.StopAsync();
        srv.Dispose();
        
    }

    void IDisposable.Dispose()
    {
        throw new NotImplementedException();
    }
}