#pragma once
#include "i_product_repo.h"

class ProductRepository : public IProductRepository
{
private:
    /* data */
public:
    ProductRepository(/* args */);
    ~ProductRepository();
    bool create(const Product& p) override;
    Product findProductByCode(const std::string& code) override;
    bool update(const Product& p) override;
};

