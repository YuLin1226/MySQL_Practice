#include <iostream>
#include "database.h"
#include "database_operation.h"

#define TEST_CRUD_OPERATIONS 1

void printBook(const Book& book) {
    std::cout << "----------------------------------------\n";
    std::cout << "書籍 ID: " << book.id << "\n";
    std::cout << "QR 代碼: " << book.qr_code << "\n";
    std::cout << "書名: " << book.title << "\n";
    std::cout << "作者: " << book.author << "\n";
    std::cout << "ISBN: " << book.isbn << "\n";
    std::cout << "出版年份: " << book.publication_year << "\n";
    std::cout << "狀態: " << book.status << "\n";
    std::cout << "----------------------------------------\n";
}

int main() {
    try {
        DatabaseConnection::initialize("127.0.0.1", "library_user", "password123", "My_First_DB", 3306);
        DatabaseConnection& db = DatabaseConnection::getInstance();
        
        if (!db.connect()) {
            std::cerr << "無法連接到資料庫: " << db.getLastError() << std::endl;
            return 1;
        }
        
        std::cout << "成功連接到資料庫！\n";
        std::cout << "Server info: " << db.getServerInfo() << std::endl;
        std::cout << "Client info: " << db.getClientInfo() << std::endl;
        std::cout << "Current database: " << db.getCurrentDatabase() << std::endl;

#if TEST_CRUD_OPERATIONS
        DatabaseOperations ops;
        
        // // 1. 新增書籍測試
        // std::cout << "\n=== 測試新增書籍 ===\n";
        // Book newBook{
        //     0,              // id 會由資料庫自動分配
        //     "",            // qr_code 會由資料庫生成
        //     "C++ 程式設計", // 書名
        //     "張三",        // 作者
        //     "9789571234567", // ISBN
        //     2024,           // 出版年
        //     "available"     // 狀態
        // };
        
        // if (ops.createBook(newBook)) {
        //     std::cout << "成功新增書籍！\n";
        // } else {
        //     std::cout << "新增書籍失敗！\n";
        // }

        // 2. 查詢書籍測試
        std::cout << "\n=== 測試查詢書籍 ===\n";
        std::string qr_code = "BOOK00000001";  
        auto book = ops.getBook(qr_code);
        
        if (book) {
            // std::cout << "找到書籍：\n";
            // printBook(*book);

            // // 3. 測試更新書籍
            // std::cout << "\n=== 測試更新書籍 ===\n";
            // Book updateBook = *book;  // 複製找到的書籍資訊
            // updateBook.title = "更新後的書名";
            // updateBook.author = "新作者";
            
            // if (ops.updateBook(updateBook)) {
            //     std::cout << "成功更新書籍！\n";
            //     // 顯示更新後的資訊
            //     auto updatedBook = ops.getBook(qr_code);
            //     if (updatedBook) {
            //         std::cout << "更新後的書籍資訊：\n";
            //         printBook(*updatedBook);
            //     }
            // } else {
            //     std::cout << "更新書籍失敗！\n";
            // }

            // 4. 測試刪除書籍
            std::cout << "\n=== 測試刪除書籍 ===\n";
            std::string delete_qr = "BOOK00000011";  // 選擇一個要刪除的書籍
            if (ops.deleteBook(delete_qr)) {
                std::cout << "成功刪除書籍！\n";
            } else {
                std::cout << "刪除書籍失敗！\n";
            }
        } else {
            std::cout << "找不到 QR 碼為 " << qr_code << " 的書籍\n";
        }

        // // 5. 查詢所有書籍
        // std::cout << "\n=== 測試查詢所有書籍 ===\n";
        // auto all_books = ops.getAllBooks();
        
        // if (all_books.empty()) {
        //     std::cout << "資料庫中沒有任何書籍\n";
        // } else {
        //     std::cout << "資料庫中共有 " << all_books.size() << " 本書：\n";
        //     for (const auto& book : all_books) {
        //         printBook(book);
        //     }
        // }
#endif

        // 斷開連接
        db.disconnect();
        std::cout << "已斷開資料庫連接\n";
        
    } catch (const std::exception& e) {
        std::cerr << "發生錯誤：" << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}