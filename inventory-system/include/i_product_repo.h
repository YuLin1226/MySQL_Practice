#pragma once
#include <string>
#include "product.h"

class IProductRepository
{
private:
    /* data */
public:
    IProductRepository(/* args */){};
    virtual ~IProductRepository(){};

    virtual bool create(Product p) = 0;
    virtual Product findProductByCode(std::string code) = 0;
    virtual bool update(Product p) = 0;
};

