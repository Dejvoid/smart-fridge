
namespace Server.Data;

public class Allergen {
    public int Id { get; set; }
    public string Name { get; set; }
}
public class Product {
    public int Id { get; set; }
    
    /// <summary>
    /// Barcode of the product
    /// </summary>
    public string Barcode { get; set; }
    public string Name { get; set; }

    /// <summary>
    /// Number of the products present
    /// </summary>
    public int Count { get; set; }

    /// <summary>
    /// Expiry date of the product
    /// </summary>
    public DateTime Expiry { get; set; }

    /// <summary>
    /// Allergens contained in the product
    /// </summary>
    public List<Allergen> Allergens { get; set; } = new();
    public Product(string barcode, string name, DateTime expiry)
    {
        Barcode = barcode;
        Name = name;
        Count = 0;
        Expiry = expiry;
    }
}
public class Recipe {
    public int Id { get; set; }
    public string Name { get; set; }
    public string Description { get; set; }

    /// <summary>
    /// Ingredients for the recipe
    /// </summary>
    public List<Product> Ingredients { get; set; } = new();

    /// <summary>
    /// How long will the recipe take in minutes
    /// </summary>
    public int Duration { get; set; }
}

public enum NotifPeriod {
    NONE, DAILY, WEEKLY, MONTHLY
}

public enum NotifPriority {
    NONE = 0, LOW = 1, MEDIUM = 2, HIGH = 3
}

public class Notification {
    public int Id { get; set; }
    public string Text { get; set; }
    public NotifPriority Priority { get; set; }
    public NotifPeriod Repetition { get; set; }
    public DateTime DateTime { get; set; }
}