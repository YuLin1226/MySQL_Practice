#pragma once
#include "inventory_item.h"
#include <vector>
#include <string>

class IinventoryRepository
{
private:
public:
    IinventoryRepository(/* args */){};
    virtual ~IinventoryRepository(){};
    virtual InventoryItem getStock(std::string _1, std::string _2) = 0;
    virtual bool updateStock(std::string _1, std::string _2, int _3) = 0;
    virtual std::vector<InventoryItem> getLowStock() = 0;
};

