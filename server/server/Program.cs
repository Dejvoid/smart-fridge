﻿
using System.Diagnostics.CodeAnalysis;
using System.Net;
using System.Net.Sockets;
using System.Text;

int buffSize = 1024;
IPAddress ip = IPAddress.Any;
int port = 2666;
var ipEndpoint = new IPEndPoint(ip, port);

//Thread httpThread = new Thread();
//
//httpThread.Start();

SocketServer server = new SocketServer(ipEndpoint);

server.Start();

while(true)
    Thread.Sleep(100000);

/*
using Socket server = new Socket(ipEndpoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

server.Bind(ipEndpoint);

server.Listen();

var handler = await server.AcceptAsync();

while(true) {
    var buffer = new byte[buffSize];
    var received = await handler.ReceiveAsync(buffer, SocketFlags.None);
    var response = Encoding.ASCII.GetString(buffer, 0, received);

    {
        Console.WriteLine($"Received: {response}");
        var message = "Test message";
        var echoBytes = Encoding.ASCII.GetBytes(message);
        await handler.SendAsync(echoBytes, 0);
    }
}*/