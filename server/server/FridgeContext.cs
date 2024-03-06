using Microsoft.EntityFrameworkCore;

class FridgeContext : DbContext {
    public DbSet<Allergen> Allergens { get; set; }
    public DbSet<Product> Products { get; set; }
    public DbSet<Recipe> Recipes { get; set; }

    public string DbPath { get; }

    public FridgeContext()
    {
        var folder = Environment.SpecialFolder.LocalApplicationData;
        var path = Environment.GetFolderPath(folder);
        DbPath = System.IO.Path.Join(path, "fridge.db");
    }
    protected override void OnConfiguring(DbContextOptionsBuilder options)
        => options.UseSqlite($"Data Source={DbPath}");
}