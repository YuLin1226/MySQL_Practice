// main.cpp
#include "database_connection.h"
#include <iostream>

void testDatabaseConnection() {
    try {
        auto& db = DatabaseConnection::getInstance();
        
        if (db.isConnected()) {
            std::cout << "Successfully connected to database!" << std::endl;
            std::cout << "Current database: " << db.getCurrentDatabase() << std::endl;
            
            // Test a simple query to check inventory table
            auto& sql = db.getSession();
            int count;
            sql << "SELECT COUNT(*) FROM inventory", soci::into(count);
            std::cout << "Total items in inventory: " << count << std::endl;
        } else {
            std::cout << "Failed to connect to database." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        // Initialize database connection with Inventory database
        DatabaseConnection::initialize(
            "127.0.0.1", 
            "inventory_user",        // 使用你的 MySQL 用戶名
            "password123",    // 使用你的 MySQL 密碼
            "inventory",   // 改為 Inventory 資料庫
            3306
        );
        
        // Run tests
        testDatabaseConnection();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}