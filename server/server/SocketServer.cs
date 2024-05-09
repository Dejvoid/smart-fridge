
using System.Collections.Concurrent;
using System.Net;
using System.Net.Sockets;
using System.Reflection.Metadata.Ecma335;
using System.Text;

/// <summary>
/// Socket service for communication with the device
/// </summary>
class SocketServer : IDisposable {
    IDataController productControl;
    private Socket socket;
    private int buffSize = 1024;
    private CancellationTokenSource cancel = new CancellationTokenSource();
    private ConcurrentDictionary<Socket, string> clients = new ConcurrentDictionary<Socket, string>();

    public SocketServer(IPEndPoint ip, IDataController productController)
    {
        productControl = productController;
        socket = new Socket(ip.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
        socket.Bind(ip);
    }

    /// <summary>
    /// Start the service
    /// </summary>
    /// <exception cref="Exception">Unexpected error</exception>
    public async void Start() {
        socket.Listen();
        while (true) {
            var client = await socket.AcceptAsync(cancel.Token);
            //var client = socket.Accept();
            Task t = new Task(() => {ConnectionHandle(client);});
            t.Start();
        }
        throw new Exception();
    }

    /// <summary>
    /// Send the message to the device
    /// </summary>
    /// <param name="data"></param>
    public async void SendMessage(object? data) {
        if (data is null)
            data = "";
        await Parallel.ForEachAsync(clients, async (client, name) => {
            var msg = Encoding.ASCII.GetBytes((string)data);
            // try {
            await client.Key.SendAsync(msg, cancel.Token);
            //} catch user disconnected
        });
    }

    public void Dispose()
    {
        cancel.Cancel();
        socket.Dispose();
    }

    /// <summary>
    /// Device connection handler
    /// </summary>
    /// <param name="client"></param>
    private async void ConnectionHandle(Socket client) {
        clients.AddOrUpdate(client, "", (key, oldValue) => "");
        Console.WriteLine($"{client.RemoteEndPoint} connected");
        while (!cancel.IsCancellationRequested && client.Poll(10, SelectMode.SelectWrite)) {
            var buffer = new byte[buffSize];
            int msgLen = await client.ReceiveAsync(buffer, cancel.Token);
            if (msgLen > 0) {
                var msg = Encoding.ASCII.GetString(buffer, 0, msgLen);
                //Console.WriteLine($"received: {msg} from {client.RemoteEndPoint}");
                var tmp = Encoding.ASCII.GetBytes($"received: {msg} from {client.RemoteEndPoint}");
                ProcessMessage(msg, client);
                //await client.SendAsync(tmp, cancel.Token);
            }
        }
        Console.WriteLine($"{client.RemoteEndPoint} disconnected");
        clients.Remove(client, out _);
    }

    /// <summary>
    /// Process the device message. Works on text based protocol
    /// </summary>
    /// <param name="msg"></param>
    /// <param name="client"></param>
    private void ProcessMessage(string msg, Socket client) 
    {
        var msgParts = msg.Split(' ');
        if (msgParts.Length > 1) {
            switch (msgParts[0]) {
            case "add":
                SendMessage(productControl.AddProduct(msgParts[1]).ToString());
            break;
            case "rm": 
                SendMessage(productControl.RemoveProduct(msgParts[1]).ToString());
            break;
            case "temp":
                float temp;
                if (float.TryParse(msgParts[1], out temp)) {
                    if (temp > 10.0) {
                        SendMessage("3: Temperature too high!");
                    }
                }
            break;
            case "notify":

            break;
            default:
                Console.WriteLine("Unknown command");
            break;
            }
        }
        else {
            Console.WriteLine("Unknown message format");
        }
    }
}