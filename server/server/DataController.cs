interface IDataController {
    public int AddProduct(string identifier);
    public int RemoveProduct(string identifier);
}

class DataController : IDataController
{
    private Dictionary<string, int> products = new Dictionary<string, int>();
    public int AddProduct(string identifier)
    {
        if (products.ContainsKey(identifier)) {
            ++products[identifier];
        }
        else {
            products.Add(identifier, 1);
        }
        return products[identifier];
    }

    public int RemoveProduct(string identifier)
    {
        if (products.ContainsKey(identifier)) {
            return --products[identifier];
        }
        else {
            return 0;
        }
    }
}