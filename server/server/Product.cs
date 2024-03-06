class Allergen {
    public int Id { get; set; }
    public string Name { get; set; }
}
class Product {
    public int Id { get; set; }
    public string Barcode { get; set; }
    public string Name { get; set; }
    public int Count { get; set; }
    public DateTime Expiry { get; set; }
    public List<Allergen> Allergens { get; set; } = new();
    public Product(string barcode, string name, DateTime expiry)
    {
        Barcode = barcode;
        Name = name;
        Count = 0;
        Expiry = expiry;
    }
}

class Recipe {
    public int Id { get; set; }
    public string Name { get; set; }
    public string Description { get; set; }
    public List<Product> Ingredients { get; set; } = new();
    public TimeSpan Duration { get; set; }
}