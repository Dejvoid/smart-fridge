using Server.Data;


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
    bool Cookable(Recipe r);
    void Cook(Recipe r);
    List<Recipe> GetCookable();
}

/// <summary>
/// Implementation of IDataController for SQL DB with EntityFramework
/// </summary>
class DataController : IDataController
{
    private ApplicationDbContext dbContext;
    public DataController(ApplicationDbContext db)
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
        return (int)ret;
    }

    public int RemoveProduct(string identifier)
    {
        var product = dbContext.Products.Where(p => p.Barcode == identifier).FirstOrDefault();
        int ret;
        if (product != null) {
            if (product.Count >= 1)
                --product.Count;
            ret = (int)product.Count;
        }
        // If the product is not known to the database
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
        if (dbContext.Notifications.Contains(notif)) {
            dbContext.Notifications.Remove(notif);
            dbContext.SaveChanges();
        }
    }

    public List<Notification> GetNotifications() {
        return dbContext.Notifications.ToList();
    }

    public bool Cookable(Recipe r) {
        bool ret = true;
        foreach (var rp in r.Ingredients) {
            ret &= rp.Product.Count >= rp.Count;
        }
        return ret;
    }

    public void Cook(Recipe r) {
        if (Cookable(r)) {
            foreach (var rp in r.Ingredients) {
                var p = rp.Product;
                p.Count -= rp.Count;
            }
            dbContext.SaveChanges();
        }
    }
    public List<Recipe> GetCookable() {
        return dbContext.Recipes.Where(
            r => r.Ingredients
                .All(rp => rp.Count <= rp.Product.Count)).ToList();
    }
}