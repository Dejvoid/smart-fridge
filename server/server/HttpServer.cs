using System.Net;
using System.Text;
using Microsoft.EntityFrameworkCore.Metadata.Internal;

/// <summary>
/// HTTP server for communication with the user
/// </summary>
class HttpServer {
    private HttpListener http;
    private IDataController db;
    private string prefix = "http://localhost:8080/";
    public HttpServer(IDataController products) {
        http = new HttpListener();
        http.Prefixes.Add(prefix);
        db = products;
    }

    /// <summary>
    /// Start the HTTP server
    /// </summary>
    public async void Start() {
        http.Start();
        while (true) {
            var ctx = await http.GetContextAsync();
            Task t = new Task(() => {HandleRequest(ctx);});
            t.Start();
        }
    }

    /// <summary>
    /// Handle the HTTP request from the given context
    /// </summary>
    /// <param name="ctx">HTTP context</param>
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
            byte[] postData = new byte[1024];
            int size = ctx.Request.InputStream.Read(postData);
            string data = ctx.Request.ContentEncoding.GetString(postData);
        }
    }

    /// <summary>
    /// Handle products section
    /// </summary>
    /// <param name="url">Requested URL</param>
    /// <param name="ctx">HTTP context</param>
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

    /// <summary>
    /// Handle recipe section
    /// </summary>
    /// <param name="url">Requested URL</param>
    /// <param name="ctx">HTTP context</param>
    private void HandleRecipes(string url, HttpListenerContext ctx) {
        ViewRecipes(ctx);
    }

    /// <summary>
    /// Default page
    /// </summary>
    /// <param name="ctx"></param>
    private void ViewDefault(HttpListenerContext ctx) {
        SendHtml("Main page", ctx);
    }

    /// <summary>
    /// Products list page
    /// </summary>
    /// <param name="ctx">HTTP context</param>
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

    /// <summary>
    /// Recipes list page
    /// </summary>
    /// <param name="ctx">HTTP context</param>
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

    /// <summary>
    /// Product editing page
    /// </summary>
    /// <param name="p">Product to be edited</param>
    /// <param name="ctx">HTTP context</param>
    private void ViewEditProduct(Product p, HttpListenerContext ctx) {
        string htmlBody = 
        $@"<form action=""{prefix}products/edit/{p.Id}"" method=""POST"">
            <div>
              <label for=""name"">Product Name ({p.Barcode}): </label>
              <input name=""name"" id=""name"" value=""{p.Name}"" />
            </div>
            <div>
              <label for=""expiry"">Expiry</label>
              <input name=""expiry"" id=""expiry"" value=""{p.Expiry}"" />
            </div>
            <div>
              <label for=""count"">Count</label>
              <input name=""count"" id=""count"" value=""{p.Count}"" />
            </div>
            <div>
              <input type=""submit"">Save changes</button>
            </div>
        </form>";
        SendHtml(htmlBody, ctx);
    }

    /// <summary>
    /// Recipe editing page
    /// </summary>
    /// <param name="r">Recipe to be edited</param>
    /// <param name="ctx">HTTP context</param>
    private void ViewEditRecipe(Recipe r, HttpListenerContext ctx) {

    }

    /// <summary>
    /// Send the HTML file to the client
    /// </summary>
    /// <param name="content">HTML content to be wrapped in the file</param>
    /// <param name="ctx">HTTP context</param>
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