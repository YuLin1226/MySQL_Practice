#pragma once
#include <string>

struct InventoryItem
{
    std::string warehouse_code;
    std::string product_code;
    int quantity;
    int min_stock;
    int max_stock;
};
