
using Microsoft.CodeAnalysis.CSharp.Syntax;

interface IDashBoard {
    public void UpdateTemperature(float t);
    public void UpdateHumidity(float h);
}

class DashboardService : IDashBoard {
    private float minTemperature = 0.0F;
    private float maxTemperature = 30.0F;
    private float temperature = 5.0F;
    private float minHumidty = 20.0F;
    private float maxHumidity = 80.0F;
    private float humidity = 10.0F;
    public float Temperature { get => temperature; }
    public float Humidity { get => humidity; }
    public INotifier? Notifier { get; set; }
    public void UpdateTemperature(float temp) {
        temperature = temp;
        if (Notifier is not null && (maxTemperature < temperature || temperature < minTemperature)) {
            Notifier.NotifyImmediate("Check temperature", Server.Data.NotifPriority.HIGH);
        }
    }
    public void UpdateHumidity(float hum) {
        humidity = hum;
        if (Notifier is not null && (maxHumidity < humidity || humidity < minHumidty)) {
            Notifier.NotifyImmediate("Check humidity", Server.Data.NotifPriority.MEDIUM);
        }
    }
}