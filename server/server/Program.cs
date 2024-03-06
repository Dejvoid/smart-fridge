
using System.Net;


IPAddress ip = IPAddress.Any;
int port = 2666;
var ipEndpoint = new IPEndPoint(ip, port);

//Thread httpThread = new Thread();
//
//httpThread.Start();
IDataController productController = new DataController();

SocketServer server = new SocketServer(ipEndpoint, productController);

server.Start();

while(true)
    Thread.Sleep(100000);