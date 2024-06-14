using Server.Data;

interface INotifier {
    public void NotifyOn(string text, DateTime dateTime, NotifPriority priority = NotifPriority.NONE, NotifPeriod repetition = NotifPeriod.NONE);
}

class NotificationHandler : INotifier, IDisposable {
    IDataController db;
    //SocketServer socket;
    //HttpServer http;
    CancellationTokenSource cancel;
    public NotificationHandler(IDataController db/*, SocketServer socketServer*//*, HttpServer httpServer*/) {
        this.db = db;
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

    internal void LoadFromDb()
    {
        foreach (var n in db.GetNotifications())
        {
            NotifyOn(n);
        }
    }

    private void TriggerNotification(Notification notif) {
        //socket.SendMessage(notif.Text, notif.Priority);
        //MqttHandler.SendNotif()
        // We need to remove the notification after it is expired
        db.RemoveNotification(notif);
        switch (notif.Repetition) {
            case NotifPeriod.DAILY:
                NotifyOn(notif.Text, DateTime.Now.AddDays(1), notif.Priority, notif.Repetition);
            break;
            case NotifPeriod.WEEKLY:
                NotifyOn(notif.Text, DateTime.Now.AddDays(7), notif.Priority, notif.Repetition);
            break;
            case NotifPeriod.MONTHLY:
                NotifyOn(notif.Text, DateTime.Now.AddMonths(1), notif.Priority, notif.Repetition);
            break;
        }
    }
}