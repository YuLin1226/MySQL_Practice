// database_operations.h
#ifndef DATABASE_OPERATIONS_H
#define DATABASE_OPERATIONS_H

#include "database.h"
#include <vector>
#include <optional>
#include <string>

struct Book {
    int id;
    std::string qr_code;
    std::string title;
    std::string author;
    std::string isbn;
    int publication_year;
    std::string status;
};

struct User {
    int id;
    std::string card_id;
    std::string name;
    std::string email;
    std::string phone;
};

struct BorrowRecord {
    int record_id;
    int book_id;
    int user_id;
    std::string borrow_date;
    std::string due_date;
    std::optional<std::string> return_date;
};

class DatabaseOperations {
public:
    // Book operations
    bool createBook(const Book& book);
    std::optional<Book> getBook(const std::string& qr_code);
    std::vector<Book> getAllBooks();
    bool updateBook(const Book& book);
    bool deleteBook(const std::string& qr_code);
    
    // User operations
    bool createUser(const User& user);
    std::optional<User> getUser(const std::string& card_id);
    std::vector<User> getAllUsers();
    bool updateUser(const User& user);
    bool deleteUser(const std::string& card_id);
    
    // Borrow record operations
    bool createBorrowRecord(const std::string& book_qr, const std::string& user_card);
    bool returnBook(const std::string& book_qr);
    std::vector<BorrowRecord> getUserBorrowHistory(const std::string& user_card);
    std::vector<BorrowRecord> getBookBorrowHistory(const std::string& book_qr);

private:
    bool executeQuery(const std::string& query);
    MYSQL_RES* executeSelectQuery(const std::string& query);
    std::string escapeString(const std::string& str);
};

#endif // DATABASE_OPERATIONS_H