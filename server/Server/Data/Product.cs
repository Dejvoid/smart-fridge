
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
    /// The amount which is the minimum we must have
    /// </summary>
    public int MinimumCount { get; set; }

    /// <summary>
    /// Expiry date of the product
    /// </summary>
    public DateTime Expiry { get; set; }

    /// <summary>
    /// Default value to be used for expiry in days 
    /// (when adding the product to the database, the expiry will be DateTime.Now + [number of days])
    /// </summary>
    public int DefaultExpiryDays { get; set; }

    /// <summary>
    /// Allergens contained in the product
    /// </summary>
    public ICollection<Allergen> Allergens { get; set; }
    public Product(string barcode, string name, DateTime expiry)
    {
        Barcode = barcode;
        Name = name;
        Count = 0;
        Expiry = expiry;
    }
}

public class RecipeProduct {
    public int ProductId { get; set; }
    public Product Product { get; set; }
    public decimal Count { get; set; }

    public int RecipeId { get; set; }
    public Recipe Recipe { get; set; }
}

public class Recipe {
    public int Id { get; set; }
    public string Name { get; set; } = "";
    public string Description { get; set; } = "";

    /// <summary>
    /// Ingredients for the recipe
    /// </summary>
    public ICollection<RecipeProduct> Ingredients { get; set; } = new HashSet<RecipeProduct>();

    /// <summary>
    /// How long will the recipe take in minutes
    /// </summary>
    public int Duration { get; set; }
}

/// <summary>
/// Notification period
/// </summary>
public enum NotifPeriod {
    NONE, DAILY, WEEKLY, MONTHLY
}


/// <summary>
/// Notification priority
/// </summary>
public enum NotifPriority {
    NONE = 0, LOW = 1, MEDIUM = 2, HIGH = 3
}

public class Notification {
    public int Id { get; set; }
    public string Text { get; set; }
    public NotifPriority Priority { get; set; }
    public NotifPeriod Repetition { get; set; }
    /// <summary>
    /// Date and time of notification
    /// </summary>
    public DateTime DateTime { get; set; }
}