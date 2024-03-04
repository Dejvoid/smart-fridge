

using System.Collections.Immutable;
using System.Net;
using System.Net.Sockets;
using System.Text;

class SocketServer : IDisposable {
    private Socket socket;
    private int buffSize = 1024;
    private CancellationTokenSource cancel = new CancellationTokenSource();

    public SocketServer(IPEndPoint ip)
    {
        socket = new Socket(ip.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
        socket.Bind(ip);
    }

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

    public void Dispose()
    {
        cancel.Cancel();
        socket.Dispose();
    }

    private async void ConnectionHandle(Socket client) {
        Console.WriteLine($"{client.RemoteEndPoint} connected");
        while (!cancel.IsCancellationRequested) {
            var buffer = new byte[buffSize];
            int msgLen = await client.ReceiveAsync(buffer, cancel.Token);
            if (msgLen > 0) {
                var msg = Encoding.ASCII.GetString(buffer, 0, msgLen);
                //Console.WriteLine($"received: {msg} from {client.RemoteEndPoint}");
                var tmp = Encoding.ASCII.GetBytes($"received: {msg} from {client.RemoteEndPoint}");
                await client.SendAsync(tmp, cancel.Token);
            }
        }
    }
}