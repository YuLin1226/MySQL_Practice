#pragma once
#include "i_inventory_repo.h"

class InventoryRepository : public IInventoryRepository
{
private:
    /* data */
public:
    InventoryRepository(/* args */);
    ~InventoryRepository();
    InventoryItem getStock(std::string _1, std::string _2) override;
    bool updateStock(std::string _1, std::string _2, int _3) override;
    std::vector<InventoryItem> getLowStock() override;
};

