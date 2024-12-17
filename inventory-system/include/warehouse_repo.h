#pragma once
#include "i_warehouse_repo.h"


class WarehouseRepository : public IWarehouseRepository
{
private:
    /* data */
public:
    WarehouseRepository(/* args */);
    ~WarehouseRepository();
    bool create(Warehouse w) override;
    Warehouse findWarehouseByCode(std::string code) override;
    bool update(Warehouse w) override;
};

