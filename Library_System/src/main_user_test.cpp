#include <iostream>
#include "database.h"
#include "database_operation.h"

void printUser(const User& user) {
    std::cout << "----------------------------------------\n";
    std::cout << "使用者 ID: " << user.id << "\n";
    std::cout << "卡片 ID: " << user.card_id << "\n";
    std::cout << "姓名: " << user.name << "\n";
    std::cout << "信箱: " << user.email << "\n";
    std::cout << "電話: " << user.phone << "\n";
    std::cout << "----------------------------------------\n";
}

int main() {
    try {
        // 初始化資料庫連線
        DatabaseConnection::initialize("127.0.0.1", "library_user", "password123", "My_First_DB", 3306);
        DatabaseConnection& db = DatabaseConnection::getInstance();
        
        if (!db.connect()) {
            std::cerr << "無法連接到資料庫: " << db.getLastError() << std::endl;
            return 1;
        }
        
        std::cout << "成功連接到資料庫！\n";
        DatabaseOperations ops;
        
        // // 1. 測試新增使用者
        // std::cout << "\n=== 測試新增使用者 ===\n";
        // User newUser{
        //     0,                    // id 會由資料庫自動分配
        //     "",                  // card_id 會由資料庫自動生成
        //     "李小明",           // name
        //     "lee@example.com",  // email
        //     "0912345678"       // phone
        // };

        // if (ops.createUser(newUser)) {
        //     std::cout << "成功新增使用者！\n";
            
        //     // 查詢所有使用者來找到新增的使用者及其 card_id
        //     auto all_users = ops.getAllUsers();
        //     for (const auto& u : all_users) {
        //         if (u.email == newUser.email) {  // 用 email 來找到新增的使用者
        //             std::cout << "新使用者資訊：\n";
        //             printUser(u);
        //             newUser.card_id = u.card_id;  // 儲存生成的 card_id 以供後續測試使用
        //             break;
        //         }
        //     }
        // } else {
        //     std::cout << "新增使用者失敗！\n";
        // }

        // // 2. 測試查詢使用者
        // std::cout << "\n=== 測試查詢使用者 ===\n";
        // auto user = ops.getUser("USER00000004");
        
        // if (user) {
        //     std::cout << "找到使用者：\n";
        //     printUser(*user);

        //     // 3. 測試更新使用者
        //     std::cout << "\n=== 測試更新使用者 ===\n";
        //     user->name = "李大明";  // 修改姓名
        //     user->phone = "0987654321";  // 修改電話
            
        //     if (ops.updateUser(*user)) {
        //         std::cout << "成功更新使用者！\n";
        //         // 顯示更新後的資訊
        //         auto updatedUser = ops.getUser("USER00000004");
        //         if (updatedUser) {
        //             std::cout << "更新後的使用者資訊：\n";
        //             printUser(*updatedUser);
        //         }
        //     } else {
        //         std::cout << "更新使用者失敗！\n";
        //     }
        // } else {
        //     std::cout << "找不到此使用者\n";
        // }

        // 4. 測試查詢所有使用者
        std::cout << "\n=== 測試查詢所有使用者 ===\n";
        auto all_users = ops.getAllUsers();
        
        if (all_users.empty()) {
            std::cout << "資料庫中沒有任何使用者\n";
        } else {
            std::cout << "資料庫中共有 " << all_users.size() << " 位使用者：\n";
            for (const auto& u : all_users) {
                printUser(u);
            }
        }

        // // 5. 測試刪除使用者
        // std::cout << "\n=== 測試刪除使用者 ===\n";
        // if (ops.deleteUser("USER00000004")) {
        //     std::cout << "成功刪除使用者！\n";
            
        //     // 確認使用者是否真的被刪除
        //     auto deleted_user = ops.getUser("USER00000004");
        //     if (!deleted_user) {
        //         std::cout << "確認：使用者已經不存在於資料庫中\n";
        //     }
        // } else {
        //     std::cout << "刪除使用者失敗！\n";
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