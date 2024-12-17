#pragma once
#include "i_warehouse_repo.h"
#include <memory>
#include <string>

class WarehouseService
{
private:
    std::shared_ptr<IWarehouseRepository> wr_;

public:
    WarehouseService(/* args */);
    ~WarehouseService();
    bool createWarehouse(Warehouse w);
    Warehouse getWarehouse(std::string name);
    bool updateWarehouse(Warehouse w);
};
