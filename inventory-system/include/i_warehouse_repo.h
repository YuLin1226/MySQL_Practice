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
    virtual bool create(const Warehouse& w) = 0;
    virtual Warehouse findWarehouseByCode(const std::string& code) = 0;
    virtual bool update(const Warehouse& w) =0;


};
