
using System.Net;


IPAddress ip = IPAddress.Any;
int port = 2666;
var ipEndpoint = new IPEndPoint(ip, port);

using var db = new FridgeContext();

IDataController productController = new DataController(db);

SocketServer server = new SocketServer(ipEndpoint, productController);

server.Start();

while(true)
    Thread.Sleep(100000);