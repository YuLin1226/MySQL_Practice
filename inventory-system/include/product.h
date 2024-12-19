#pragma once
#include <string>

struct Product {
    std::string code;        // SKU
    std::string name;        // 產品名稱
    std::string description; // 產品描述
    std::string category;    // 產品類別
    double unit_price;       // 單價
    std::string unit;        // 單位(個/箱/公斤等)
    std::string status;      // 狀態(active/inactive)
    int min_stock;          // 最小庫存
    int max_stock;          // 最大庫存
};
