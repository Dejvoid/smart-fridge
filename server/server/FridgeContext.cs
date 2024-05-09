using Microsoft.EntityFrameworkCore;

/// <summary>
/// DB description for EntityFramework
/// </summary>
class FridgeContext : DbContext {
    /// <summary>
    /// Table of alergens
    /// </summary>
    public DbSet<Allergen> Allergens { get; set; }
    
    /// <summary>
    /// Table of products
    /// </summary>
    public DbSet<Product> Products { get; set; }
    
    /// <summary>
    /// Table of recipes
    /// </summary>
    public DbSet<Recipe> Recipes { get; set; }
    
    /// <summary>
    /// Path to the DB file
    /// </summary>
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