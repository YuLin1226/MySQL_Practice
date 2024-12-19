#include "product_repo.h"
#include "database_connection.h"
#include <soci/soci.h>
#include <stdexcept>

ProductRepository::ProductRepository() {}
ProductRepository::~ProductRepository() {}

// 建構函數和解構函數
ProductRepository::ProductRepository() {} 
ProductRepository::~ProductRepository() {}

bool ProductRepository::create(const Product& p) {
    try {
        // 取得資料庫連接
        soci::session& sql = DatabaseConnection::getInstance().getSession();
        
        // 檢查是否存在相同 SKU
        int count;
        sql << "SELECT COUNT(*) FROM products WHERE sku = :sku",
            soci::use(p.code), soci::into(count);
            
        if (count > 0) {
            // 如果已存在相同 SKU,拋出異常
            throw std::runtime_error("Product with SKU " + p.code + " already exists");
        }
        
        // 插入新產品記錄
        sql << "INSERT INTO products ("
               "sku, name, description, category, unit_price, "
               "unit, status, min_stock, max_stock, created_by, updated_by"
               ") VALUES ("
               ":sku, :name, :desc, :cat, :price, "
               ":unit, :status, :min_stock, :max_stock, :user, :user"
               ")",
            soci::use(p.code),
            soci::use(p.name),
            soci::use(p.description),
            soci::use(p.category),
            soci::use(p.unit_price),
            soci::use(p.unit),
            soci::use(p.status),
            soci::use(p.min_stock),
            soci::use(p.max_stock),
            soci::use(1);  // TODO: 應該使用當前用戶ID
            
        return true;
    }
    catch (const std::exception& e) {
        // 紀錄錯誤並重新拋出
        throw std::runtime_error("Failed to create product: " + std::string(e.what()));
    }
}

Product ProductRepository::findProductByCode(const std::string& code) {
    try {
        soci::session& sql = DatabaseConnection::getInstance().getSession();
        Product product;
        
        // 查詢產品資訊
        sql << "SELECT sku, name, description, category, unit_price, "
               "unit, status, min_stock, max_stock "
               "FROM products WHERE sku = :sku AND status = 'active'",
            soci::use(code),
            soci::into(product.code),
            soci::into(product.name),
            soci::into(product.description),
            soci::into(product.category),
            soci::into(product.unit_price),
            soci::into(product.unit),
            soci::into(product.status),
            soci::into(product.min_stock),
            soci::into(product.max_stock);
            
        if (sql.got_data()) {
            return product;
        }
        
        throw std::runtime_error("Product not found with SKU: " + code);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to find product: " + std::string(e.what()));
    }
}

bool ProductRepository::update(const Product& p) {
    try {
        soci::session& sql = DatabaseConnection::getInstance().getSession();
        
        // 檢查產品是否存在
        int count;
        sql << "SELECT COUNT(*) FROM products WHERE sku = :sku",
            soci::use(p.code), soci::into(count);
            
        if (count == 0) {
            throw std::runtime_error("Product with SKU " + p.code + " does not exist");
        }
        
        // 更新產品資訊
        sql << "UPDATE products SET "
               "name = :name, "
               "description = :desc, "
               "category = :cat, "
               "unit_price = :price, "
               "unit = :unit, "
               "min_stock = :min_stock, "
               "max_stock = :max_stock, "
               "updated_by = :user, "
               "updated_at = CURRENT_TIMESTAMP "
               "WHERE sku = :sku",
            soci::use(p.name),
            soci::use(p.description),
            soci::use(p.category),
            soci::use(p.unit_price),
            soci::use(p.unit),
            soci::use(p.min_stock),
            soci::use(p.max_stock),
            soci::use(1),  // TODO: 應該使用當前用戶ID
            soci::use(p.code);
            
        return true;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to update product: " + std::string(e.what()));
    }
}