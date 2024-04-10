using System.Net;
using Microsoft.EntityFrameworkCore.Metadata.Internal;

class HttpServer {
    private HttpListener http;
    private IDataController db;
    private string prefix = "http://localhost:8080/";
    public HttpServer(IDataController products) {
        http = new HttpListener();
        http.Prefixes.Add(prefix);
        db = products;
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
        if (ctx.Request.HttpMethod.ToLower() == "get") {
            if (request.RawUrl is null) {
                ViewDefault(ctx);
            }
            else {
                string url = request.RawUrl.ToLower();
                if (url.StartsWith("/products")) {
                    HandleProducts(url.Substring("/products".Length), ctx);
                }
                else if (url.StartsWith("/recipes")) { 
                    HandleRecipes(url.Substring("/recipes".Length), ctx);
                }
                else
                    ViewDefault(ctx);
            }
        }
        else if (ctx.Request.HttpMethod.ToLower() == "post") {
            Console.WriteLine("poosting");
        }
    }

    private void HandleProducts(string url, HttpListenerContext ctx) {
        if (url == "") {
            ViewProducts(ctx);
            return;
        }
        else if (url.StartsWith("/edit") && url.Length > "/edit".Length) {
            int id;
            if (int.TryParse(url.Substring("/edit/".Length), out id)) {
                var product = db.GetProduct(id);
                if (product is not null) {
                    ViewEditProduct(product, ctx);
                    return;
                }
            }

        }
        ViewProducts(ctx);
    }

    private void HandleRecipes(string url, HttpListenerContext ctx) {
        ViewRecipes(ctx);
    }

    private void ViewDefault(HttpListenerContext ctx) {
        SendHtml("Main page", ctx);
    }

    private void ViewProducts(HttpListenerContext ctx) {
        var products = db.ListProducts();
        string htmlBody = "<h1>Products page: </h1>";
        htmlBody += "<ul>";
        foreach (var product in products)
        {
            htmlBody += $"<li>{product.Barcode} {product.Name} {product.Count} <a href=\"{prefix}products/edit/{product.Id}\">Edit</a> </li>";
        }
        htmlBody += "</ul>";
        SendHtml(htmlBody, ctx);
    }

    private void ViewRecipes(HttpListenerContext ctx) {
        var recipes = db.ListRecipes();
        string htmlBody = "<h1>recipes page: </h1>";
        htmlBody += "<ul>";
        foreach (var recipe in recipes)
        {
            htmlBody += $"<li>{recipe.Name}</li>";
        }
        htmlBody += "</ul>";
        SendHtml(htmlBody, ctx);
    }

    private void ViewEditProduct(Product p, HttpListenerContext ctx) {
        SendHtml("Products page", ctx);
        SendHtml(htmlBody, ctx);
    }

    private void ViewEditRecipe(Recipe r, HttpListenerContext ctx) {

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