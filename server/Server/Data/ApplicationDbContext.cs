using Microsoft.AspNetCore.Identity.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore;

namespace Server.Data;

public class ApplicationDbContext(DbContextOptions<ApplicationDbContext> options) : IdentityDbContext<ApplicationUser>(options)
{
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

    public DbSet<RecipeProduct> RecipeProducts { get; set; }

    public DbSet<Notification> Notifications { get; set; }

    protected override void OnModelCreating(ModelBuilder builder)
    {
        base.OnModelCreating(builder);
        builder.Entity<RecipeProduct>()
               .HasKey(rp => new { rp.RecipeId, rp.ProductId }); 
    }
}
