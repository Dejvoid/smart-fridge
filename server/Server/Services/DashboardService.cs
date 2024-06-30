
using Microsoft.CodeAnalysis.CSharp.Syntax;

/// <summary>
/// Dashboard service interface
/// </summary>
interface IDashBoard {
    /// <summary>
    /// Maximal allowed temperature
    /// </summary>
    public float MaxTemperature { get; set; }
    /// <summary>
    /// Minimal allowed temperature
    /// </summary>
    public float MinTemperature { get; set; }
    /// <summary>
    /// Maximal allowed humidity
    /// </summary>
    public float MaxHumidity { get; set; }
    /// <summary>
    /// Minimal allowed humidity.
    /// </summary>
    public float MinHumidity { get; set; }
    /// <summary>
    /// Updates the temperature
    /// </summary>
    /// <param name="t">Temperature value</param>
    public void UpdateTemperature(float t);
    /// <summary>
    /// Updates the humidity
    /// </summary>
    /// <param name="h">Humidity value</param>
    public void UpdateHumidity(float h);
}

/// <summary>
/// Holds and provides general information about temperature and humidity.
/// </summary>
class DashboardService : IDashBoard {
    public float MinTemperature { get; set; } = 0.0F;
    public float MaxTemperature { get; set; } = 30.0F;
    public float MinHumidity { get; set; } = 20.0F;
    public float MaxHumidity { get; set; } = 80.0F;
    private float humidity = 10.0F;
    private float temperature = 5.0F;
    public float Temperature { get => temperature; }
    public float Humidity { get => humidity; }
    /// <summary>
    /// Notifier used to send notifications
    /// Has to be nullable due to cyclic dependency of the services
    /// </summary>
    public INotifier? Notifier { get; set; }
    /// <summary>
    /// Updates the temperature value and notifies if the value is outside the given range
    /// </summary>
    /// <param name="temp">New temperature value</param>
    public void UpdateTemperature(float temp) {
        temperature = temp;
        if (Notifier is not null && (MaxTemperature < temperature || temperature < MinTemperature)) {
            Notifier.NotifyImmediate("Check temperature", Server.Data.NotifPriority.HIGH);
        }
    }
    /// <summary>
    /// Updates the humidity value and notifies if the value is outside the given range
    /// </summary>
    /// <param name="hum">New humidity value</param>
    public void UpdateHumidity(float hum) {
        humidity = hum;
        if (Notifier is not null && (MaxHumidity < humidity || humidity < MinHumidity)) {
            Notifier.NotifyImmediate("Check humidity", Server.Data.NotifPriority.MEDIUM);
        }
    }
}