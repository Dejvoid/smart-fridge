
using System.Net;


IPAddress ip = IPAddress.Any;
int port = 2666;
var ipEndpoint = new IPEndPoint(ip, port);

using var db = new FridgeContext();

IDataController productController = new DataController(db);

SocketServer socketServer = new SocketServer(ipEndpoint, productController);
HttpServer httpServer = new HttpServer(productController);

socketServer.Start();
httpServer.Start();

while(true)
    Thread.Sleep(100000);