#pragma once
#include "i_warehouse_repo.h"


class WarehouseRepository : public IWarehouseRepository
{
private:
    /* data */
public:
    WarehouseRepository(/* args */);
    ~WarehouseRepository();
    bool create(const Warehouse& w) override;
    Warehouse findWarehouseByCode(const std::string& code) override;
    bool update(const Warehouse& w) override;
};

