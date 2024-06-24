
using Microsoft.CodeAnalysis.CSharp.Syntax;

interface IDashBoard {
    public void UpdateTemperature(float t);
    public void UpdateHumidity(float h);
}

class DashboardService : IDashBoard {
    private float temperature = 22.0F;
    private float humidity = 40.0F;
    public float Temperature { get => temperature; }
    public float Humidity { get => humidity; }
    public void UpdateTemperature(float temp) {
        temperature = temp;
    }
    public void UpdateHumidity(float hum) {
        humidity = hum;
    }
}