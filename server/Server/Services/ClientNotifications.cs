
using Server.Data;

/// <summary>
/// Web application client-side notification display
/// </summary>
interface IClientNotifications {
    /// <summary>
    /// Method used by other classes to send notification to this interface
    /// </summary>
    /// <param name="n">Notification to be displayed</param>
    public void AcceptNotification(Notification n);
}

class ClientNotifications : IDisposable, IClientNotifications { 
    public Server.Components.NotificationComponent? Component { get; set; }
    ICollection<IClientNotifications>? clients;
    public ClientNotifications() {
    }

    /// <summary>
    /// Register in the collection of the other class that calls AcceptNotification
    /// </summary>
    /// <param name="otherClients">collection to be registered in</param>
    public void Register(ICollection<IClientNotifications> otherClients) {
        otherClients.Add(this);
        clients = otherClients;
    }
    public void Dispose() {
        if (clients is not null)
            clients.Remove(this);
    }

    public void AcceptNotification(Notification n)
    {
        if (Component is not null) {
            Component.UpdateNotification(n);
        }
    }

}