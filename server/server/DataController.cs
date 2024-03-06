interface IDataController {
    public int AddProduct(string identifier);
    public int RemoveProduct(string identifier);
}

class DataController : IDataController
{
    private FridgeContext dbContext;
    public DataController(FridgeContext db)
    {
        dbContext = db;
    }
    public int AddProduct(string identifier)
    {
        var product = dbContext.Products.Where<Product>(p => p.Barcode == identifier).FirstOrDefault();
        if (product == null) {
            product = dbContext.Add(new Product(identifier, "", DateTime.Now.AddDays(3))).Entity;
        }
        var ret =  ++product.Count;
        dbContext.SaveChanges();
        return ret;
    }

    public int RemoveProduct(string identifier)
    {
        var product = dbContext.Products.Where(p => p.Barcode == identifier).FirstOrDefault();
        int ret;
        if (product != null) {
            if (product.Count >= 1)
                --product.Count;
            ret = product.Count;
        }
        else {
            dbContext.Products.Add(new Product(identifier, "", DateTime.Now));
            ret = 0;
        }
        dbContext.SaveChanges();
        return ret;
    }
}