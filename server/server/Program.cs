
using System.Net;


IPAddress ip = IPAddress.Any;
int port = 2666;
var ipEndpoint = new IPEndPoint(ip, port);

using var db = new FridgeContext();

IDataController productController = new DataController(db);

SocketServer socketServer = new SocketServer(ipEndpoint, productController);
HttpServer httpServer = new HttpServer(productController);
NotificationHandler notificationService = new(productController, socketServer, httpServer);

var delay = new TimeSpan(0,0, 10);

notificationService.LoadFromDb();
//notificationService.NotifyOn("yoyo", DateTime.Now + delay);

socketServer.Start();
httpServer.Start();

// our main thread would exit 
// so we have to keep the program alive
while(true)
    Thread.Sleep(100000);