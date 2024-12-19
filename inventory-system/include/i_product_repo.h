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

    virtual bool create(const Product& p) = 0;
    virtual Product findProductByCode(const std::string& code) = 0;
    virtual bool update(const Product& p) = 0;
};

