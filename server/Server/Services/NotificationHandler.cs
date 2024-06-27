using Server.Data;

/// <summary>
/// Interface representation of the Notification handler
/// </summary>
interface INotifier {
    /// <summary>
    /// Sets the notification with given text to given time with priority and repetition
    /// </summary>
    /// <param name="text">Text of the notification</param>
    /// <param name="dateTime">Time of the notification</param>
    /// <param name="priority">Priority</param>
    /// <param name="repetition">Repetition period</param>
    public void NotifyOn(string text, DateTime dateTime, NotifPriority priority = NotifPriority.NONE, NotifPeriod repetition = NotifPeriod.NONE);
    public void NotifyOn(Notification notif);
    public void NotifyImmediate(string text, NotifPriority priority = NotifPriority.NONE);
}

class NotificationHandler : INotifier, IDisposable {
    IDataController db;
    //SocketServer socket;
    //HttpServer http;
    MqttHandler mqtt;
    CancellationTokenSource cancel;
    public NotificationHandler(IDataController db, MqttHandler mqtt/*, SocketServer socketServer*//*, HttpServer httpServer*/) {
        this.db = db;
        this.mqtt = mqtt;
        //socket = socketServer;
        //http = httpServer;
        cancel = new CancellationTokenSource();
    }

    public void Dispose()
    {
        cancel.Cancel();
    }

    public void NotifyOn(Notification n) {
        TimeSpan delay = n.DateTime - DateTime.Now;
        if (delay.Milliseconds > 0) {
            Task.Delay(delay,cancel.Token).ContinueWith(_ => {
                TriggerNotification(n);
            },cancel.Token);
        }
        else {
            n.Text += $"[MISSED - {n.DateTime}]";
            TriggerNotification(n);
        }
    }
    public void NotifyOn(string text, DateTime dateTime, NotifPriority priority = NotifPriority.NONE, NotifPeriod repetition = NotifPeriod.NONE) {
        TimeSpan delay = dateTime - DateTime.Now;
        var notif = new Notification(){Text = text, DateTime = dateTime, Priority = priority, Repetition = repetition};
        // We assume that someone did the persisting for us
        //db.AddNotification(notif);
        if (delay.Milliseconds > 0) {
            Task.Delay(delay,cancel.Token).ContinueWith(_ => {
                TriggerNotification(notif);
            },cancel.Token);
        }
        else {
            notif.Text += $"[MISSED - {notif.DateTime}]";
            TriggerNotification(notif);
        }
    }

    public void NotifyImmediate(string text, NotifPriority priority = NotifPriority.NONE) {
        var notif = new Notification(){Text = text, DateTime = DateTime.Now, Priority = priority, Repetition = NotifPeriod.NONE};
        TriggerNotification(notif);
    }

    internal void LoadFromDb()
    {
        foreach (var n in db.GetNotifications())
        {
            NotifyOn(n);
        }
    }

    private void TriggerNotification(Notification notif) {
        //socket.SendMessage(notif.Text, notif.Priority);
        mqtt.SendNotif(notif);
        // We need to remove the notification after it is expired
        db.RemoveNotification(notif);
        switch (notif.Repetition) {
            case NotifPeriod.DAILY:
                notif.DateTime = notif.DateTime.AddDays(1);
                db.AddNotification(notif);
                NotifyOn(notif);
            break;
            case NotifPeriod.WEEKLY:
                notif.DateTime = notif.DateTime.AddDays(7);
                db.AddNotification(notif);
                NotifyOn(notif);
            break;
            case NotifPeriod.MONTHLY:
                notif.DateTime = notif.DateTime.AddMonths(1);
                db.AddNotification(notif);
                NotifyOn(notif);
            break;
        }
    }
}