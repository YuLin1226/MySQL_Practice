#pragma once
#include "i_inventory_repo.h"
#include "product_srv.h"
#include "warehouse_srv.h"
#include <memory>
#include <vector>

class InventoryService
{
private:
    std::shared_ptr<ProductService> ps_;
    std::shared_ptr<WarehouseService> ws_;
    std::shared_ptr<IInventoryRepository> ir_;

public:
    InventoryService(/* args */);
    ~InventoryService();

    bool adjustStock(std::string _1, std::string _2, int _3, std::string _4);
    InventoryItem getStock(std::string _1, std::string _2);
    std::vector<InventoryItem> checkLowStock();
    std::vector<InventoryItem> checkHighStock();


};
