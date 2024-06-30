
using Server.Data;

interface IClientNotifications {
    public void AcceptNotification(Notification n);
}

class ClientNotifications : IDisposable, IClientNotifications { 
    public Server.Components.NotificationComponent? Component { get; set; }
    ICollection<IClientNotifications>? clients;
    public ClientNotifications() {
    }
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