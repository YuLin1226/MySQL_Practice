#pragma once
#include <string>
#include "warehouse.h"

class IWarehouseRepository
{
private:
    /* data */
public:
    IWarehouseRepository(/* args */){};
    virtual ~IWarehouseRepository(){};
    virtual bool create(Warehouse w) = 0;
    virtual Warehouse findWarehouseByCode(std::string code) = 0;
    virtual bool update(Warehouse w) =0;


};
