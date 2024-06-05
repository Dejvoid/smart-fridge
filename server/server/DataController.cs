/// <summary>
/// Interface for DB communication.
/// </summary>
interface IDataController {
    /// <summary>
    /// Adds the product with the given barcode identifier.
    /// If the product already is in the DB, increments the number of items.
    /// </summary>
    /// <param name="identifier"> Barcode of the item </param>
    /// <returns> Number of products of this identifier </returns>
    int AddProduct(string identifier);
    /// <summary>
    /// Removes the product with the given barcode identifier.
    /// Doesn't remove the information about the product, just decrements the count.
    /// </summary>
    /// <param name="identifier">Barcode of the product</param>
    /// <returns> Number of products of this identifier </returns>
    int RemoveProduct(string identifier);
    /// <summary>
    /// Lists all products
    /// </summary>
    /// <returns> List of all products </returns>
    List<Product> ListProducts();
    /// <summary>
    /// Lists all recipes
    /// </summary>
    /// <returns> List of all recipes </returns>
    List<Recipe> ListRecipes();
    /// <summary>
    /// Gets product with given ID (primary key).
    /// </summary>
    /// <param name="id"> primary key of the product </param>
    /// <returns> Product if in DB, else null </returns>
    Product? GetProduct(int id);

    void AddNotification(Notification notif);

    void RemoveNotification(Notification notif);

    List<Notification> GetNotifications();
}

/// <summary>
/// Implementation of IDataController for SQL DB with EntityFramework
/// </summary>
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

    public List<Product> ListProducts() {
        return dbContext.Products.ToList();
    }

    public List<Recipe> ListRecipes() {
        return dbContext.Recipes.ToList();
    }

    public Product? GetProduct(int id)
    {
        return dbContext.Products.FirstOrDefault(p => p.Id == id);
    }

    public void AddNotification(Notification notif) {
        dbContext.Notifications.Add(notif);
        dbContext.SaveChanges();
    }

    public void RemoveNotification(Notification notif) {
        dbContext.Notifications.Remove(notif);
        dbContext.SaveChanges();
    }

    public List<Notification> GetNotifications() {
        return dbContext.Notifications.ToList();
    }
}