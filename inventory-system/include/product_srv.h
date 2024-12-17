#pragma once
#include "i_product_repo.h"
#include <memory>

class ProductService
{
private:
    
    std::shared_ptr<IProductRepository> pr_;

public:
    ProductService(/* args */);
    ~ProductService();
    bool createProduct(Product p);
    Product getProduct(std::string name);
    bool updateProduct(Product p);
};
