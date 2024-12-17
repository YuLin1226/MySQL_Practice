#pragma once
#include "i_product_repo.h"

class ProductRepository : public IProductRepository
{
private:
    /* data */
public:
    ProductRepository(/* args */);
    ~ProductRepository();
    bool create(Product p) override;
    Product findProductByCode(std::string code) override;
    bool update(Product p) override;
};

