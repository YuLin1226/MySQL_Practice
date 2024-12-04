#include <iostream>
#include "database.h"
#include "database_operation.h"

// 定義一個常數來控制要使用哪個版本
#define USE_CRUD_VERSION 1

int main() {
#if USE_CRUD_VERSION
    try {
        // CRUD 版本的 main
        DatabaseConnection::initialize("localhost", "library_user", "password123", "My_First_DB", 3306);
        DatabaseOperations ops;
        
        // 新增一本書
        Book newBook{
            0,              // id 會由資料庫自動分配
            "BOOK00000005", // QR code
            "C++ 程式設計", // 書名
            "張三",        // 作者
            "9789571234567", // ISBN
            2024,           // 出版年
            "available"     // 狀態
        };
        
        if (ops.createBook(newBook)) {
            std::cout << "成功新增書籍！\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "錯誤：" << e.what() << std::endl;
        return 1;
    }
#else
    try {
        // 原始版本的 main
        DatabaseConnection::initialize("localhost", "library_user", "password123", "My_First_DB", 3306);
        
        DatabaseConnection& db = DatabaseConnection::getInstance();
        
        if (!db.connect()) {
            std::cerr << "Failed to connect to database: " << db.getLastError() << std::endl;
            return 1;
        }
        
        std::cout << "Successfully connected to database!" << std::endl;
        std::cout << "Server info: " << db.getServerInfo() << std::endl;
        std::cout << "Client info: " << db.getClientInfo() << std::endl;
        std::cout << "Current database: " << db.getCurrentDatabase() << std::endl;
        
        db.disconnect();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
#endif
    
    return 0;
}