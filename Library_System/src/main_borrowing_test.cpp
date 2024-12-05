#include <iostream>
#include "database.h"
#include "database_operation.h"
#include <iomanip>

void printBorrowRecord(const BorrowRecord& record) {
    std::cout << "----------------------------------------\n";
    std::cout << "借閱記錄 ID: " << record.record_id << "\n";
    std::cout << "書籍 ID: " << record.book_id << "\n";
    std::cout << "使用者 ID: " << record.user_id << "\n";
    std::cout << "借閱日期: " << record.borrow_date << "\n";
    std::cout << "應還日期: " << record.due_date << "\n";
    std::cout << "實際還書日期: " << (record.return_date ? *record.return_date : "尚未歸還") << "\n";
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
        DatabaseOperations ops;

        // 1. 測試借書
        std::cout << "\n=== 測試借書功能 ===\n";
        std::string test_book_qr = "BOOK00000001";
        std::string test_user_card = "USER00000001";
        
        // if (ops.createBorrowRecord(test_book_qr, test_user_card)) {
        //     std::cout << "成功借出書籍！\n";
            
        //     // 2. 測試查詢使用者的借閱歷史
        //     std::cout << "\n=== 測試查詢使用者借閱歷史 ===\n";
        //     auto user_history = ops.getUserBorrowHistory(test_user_card);
            
        //     if (user_history.empty()) {
        //         std::cout << "該使用者沒有借閱記錄\n";
        //     } else {
        //         std::cout << "使用者借閱記錄如下：\n";
        //         for (const auto& record : user_history) {
        //             printBorrowRecord(record);
        //         }
        //     }
            
        //     // 3. 測試查詢書籍的借閱歷史
        //     std::cout << "\n=== 測試查詢書籍借閱歷史 ===\n";
        //     auto book_history = ops.getBookBorrowHistory(test_book_qr);
            
        //     if (book_history.empty()) {
        //         std::cout << "該書籍沒有借閱記錄\n";
        //     } else {
        //         std::cout << "書籍借閱記錄如下：\n";
        //         for (const auto& record : book_history) {
        //             printBorrowRecord(record);
        //         }
        //     }
            
            // 4. 測試還書
            std::cout << "\n=== 測試還書功能 ===\n";
            if (ops.returnBook(test_book_qr)) {
                std::cout << "成功歸還書籍！\n";
                
                // 再次查詢確認還書記錄
                std::cout << "\n=== 確認還書記錄 ===\n";
                auto updated_history = ops.getBookBorrowHistory(test_book_qr);
                if (!updated_history.empty()) {
                    std::cout << "更新後的借閱記錄：\n";
                    printBorrowRecord(updated_history[0]);  // 顯示最新的記錄
                }
            } else {
                std::cout << "還書失敗！\n";
            }
            
        // } else {
        //     std::cout << "借書失敗！\n";
        // }

        // 斷開連接
        db.disconnect();
        std::cout << "\n已斷開資料庫連接\n";
        
    } catch (const std::exception& e) {
        std::cerr << "發生錯誤：" << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}