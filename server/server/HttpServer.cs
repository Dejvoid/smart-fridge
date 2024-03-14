using System.Net;
using Microsoft.EntityFrameworkCore.Metadata.Internal;

class HttpServer {
    private HttpListener http;
    public HttpServer(IDataController products) {
        http = new HttpListener();
        http.Prefixes.Add("http://localhost:8080/");
    }

    public async void Start() {
        http.Start();
        while (true) {
            var ctx = await http.GetContextAsync();
            Task t = new Task(() => {HandleRequest(ctx);});
            t.Start();
        }
    }

    private void HandleRequest(HttpListenerContext ctx) {
        var request = ctx.Request;
        Console.WriteLine(request.RawUrl);
        if (request.RawUrl is null) {
            ViewDefault(ctx);
        }
        else {
        switch (request.RawUrl.ToLower()) {
            case "/products":
                ViewProducts(ctx);
            break;
            default:
                ViewDefault(ctx);
            break;
        }
        }
    }

    private void ViewDefault(HttpListenerContext ctx) {
        SendHtml("Main page", ctx);
    }

    private void ViewProducts(HttpListenerContext ctx) {
        SendHtml("Products page", ctx);
    }

    private void SendHtml(string content, HttpListenerContext ctx) {
        var response = ctx.Response;
        string responseString = $"<HTML><BODY> {content} </BODY></HTML>";
        byte[] buffer = System.Text.Encoding.UTF8.GetBytes(responseString);
        // Get a response stream and write the response to it.
        response.ContentLength64 = buffer.Length;
        Stream output = response.OutputStream;
        output.Write(buffer,0,buffer.Length);
    }
}