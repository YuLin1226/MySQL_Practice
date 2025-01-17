// database_operations.cpp
#include "database_operation.h"
#include <iostream>
#include <sstream>

bool DatabaseOperations::executeQuery(const std::string& query) {
    DatabaseConnection& db = DatabaseConnection::getInstance();
    MYSQL* conn = db.getRawConnection();
    
    // 檢查連接狀態
    if (mysql_ping(conn) != 0) {
        std::cerr << "Connection lost. Attempting to reconnect..." << std::endl;
        if (!db.connect()) {
            std::cerr << "Reconnection failed: " << mysql_error(conn) << std::endl;
            return false;
        }
    }

    if (mysql_query(conn, query.c_str()) != 0) {
        std::cerr << "SQL Error: " << mysql_error(conn) << std::endl;
        std::cerr << "Query was: " << query << std::endl;
        return false;
    }
    return true;
}

MYSQL_RES* DatabaseOperations::executeSelectQuery(const std::string& query) {
    
    DatabaseConnection& db = DatabaseConnection::getInstance();
    MYSQL* conn = db.getRawConnection();
    
    // 檢查連接狀態
    if (mysql_ping(conn) != 0) {
        std::cerr << "Connection lost. Attempting to reconnect..." << std::endl;
        if (!db.connect()) {
            std::cerr << "Reconnection failed: " << mysql_error(conn) << std::endl;
            return nullptr;
        }
    }
    
    // 先清除任何之前的結果集
    while (mysql_next_result(conn) == 0) {
        MYSQL_RES* result = mysql_store_result(conn);
        if (result) {
            mysql_free_result(result);
        }
    }
    
    // 執行查詢
    if (mysql_query(conn, query.c_str()) != 0) {
        std::cerr << "SQL Error: " << mysql_error(conn) << std::endl;
        std::cerr << "Query was: " << query << std::endl;
        return nullptr;
    }
    
    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) {
        std::cerr << "Failed to store result: " << mysql_error(conn) << std::endl;
    }
    
    return result;
}

std::string DatabaseOperations::escapeString(const std::string& str) {
    DatabaseConnection& db = DatabaseConnection::getInstance();
    char* escaped = new char[str.length() * 2 + 1];
    mysql_real_escape_string(db.getRawConnection(), escaped, str.c_str(), str.length());
    std::string result(escaped);
    delete[] escaped;
    return result;
}

// Book Operations
bool DatabaseOperations::createBook(const Book& book) {
    std::stringstream ss;
    ss << "INSERT INTO books (title, author, isbn, publication_year, qr_code) "  // 加入 qr_code 欄位
       << "SELECT "
       << "'" << escapeString(book.title) << "', "
       << "'" << escapeString(book.author) << "', "
       << "'" << escapeString(book.isbn) << "', "
       << book.publication_year << ", "  // 這裡的逗號是正確的
       << "CONCAT('BOOK', LPAD((SELECT COALESCE(MAX(book_id) + 1, 1) FROM books b), 8, '0'))";  // 最後一個值不需要逗號
    
    return executeQuery(ss.str());
}

std::optional<Book> DatabaseOperations::getBook(const std::string& qr_code) {
    std::string query = "SELECT * FROM books WHERE qr_code = '" + escapeString(qr_code) + "'";
    MYSQL_RES* result = executeSelectQuery(query);
    
    if (!result) {
        return std::nullopt;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return std::nullopt;
    }
    
    Book book;
    book.id = std::stoi(row[0]);
    book.qr_code = row[1];
    book.title = row[2];
    book.author = row[3];
    book.isbn = row[4];
    book.publication_year = std::stoi(row[5]);
    book.status = row[6];
    
    mysql_free_result(result);
    return book;
}

std::vector<Book> DatabaseOperations::getAllBooks() {
    std::vector<Book> books;
    MYSQL_RES* result = executeSelectQuery("SELECT * FROM books");
    
    if (!result) {
        return books;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        Book book;
        book.id = std::stoi(row[0]);
        book.qr_code = row[1];
        book.title = row[2];
        book.author = row[3];
        book.isbn = row[4];
        book.publication_year = std::stoi(row[5]);
        book.status = row[6];
        books.push_back(book);
    }
    
    mysql_free_result(result);
    return books;
}

bool DatabaseOperations::updateBook(const Book& book) {
    std::stringstream ss;
    ss << "UPDATE books SET "
       << "title = '" << escapeString(book.title) << "', "
       << "author = '" << escapeString(book.author) << "', "
       << "isbn = '" << escapeString(book.isbn) << "', "
       << "publication_year = " << book.publication_year << ", "
       << "status = '" << escapeString(book.status) << "' "
       << "WHERE qr_code = '" << escapeString(book.qr_code) << "'";
    
    return executeQuery(ss.str());
}

bool DatabaseOperations::deleteBook(const std::string& qr_code) {
    std::string query = "DELETE FROM books WHERE qr_code = '" + escapeString(qr_code) + "'";
    return executeQuery(query);
}

// Borrow Operations
bool DatabaseOperations::createBorrowRecord(const std::string& book_qr, const std::string& user_card) {
    
    // 1. 先執行 procedure
    std::stringstream ss;
    ss << "CALL borrow_book('" 
       << escapeString(user_card) << "', '" 
       << escapeString(book_qr) << "', @status)";
    
    if (!executeQuery(ss.str())) {
        return false;
    }

    // 2. 再查詢結果
    MYSQL_RES* result = executeSelectQuery("SELECT @status");
    if (!result) {
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    std::string status = row ? row[0] : "";
    mysql_free_result(result);
    
    return status == "Success";

}

// User Operations
bool DatabaseOperations::createUser(const User& user) {
    std::stringstream ss;
    ss << "INSERT INTO users (name, email, phone, card_id) "  
       << "SELECT "
       << "'" << escapeString(user.name) << "', "
       << "'" << escapeString(user.email) << "', "
       << "'" << escapeString(user.phone) << "', "
       << "CONCAT('USER', LPAD((SELECT COALESCE(MAX(user_id) + 1, 1) FROM users u), 8, '0'))";
    
    return executeQuery(ss.str());
}

std::optional<User> DatabaseOperations::getUser(const std::string& card_id) {
    std::string query = "SELECT * FROM users WHERE card_id = '" + escapeString(card_id) + "'";
    MYSQL_RES* result = executeSelectQuery(query);
    
    if (!result) {
        return std::nullopt;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return std::nullopt;
    }
    
    User user;
    user.id = std::stoi(row[0]);
    user.card_id = row[1];
    user.name = row[2];
    user.email = row[3];
    user.phone = row[4];
    
    mysql_free_result(result);
    return user;
}

std::vector<User> DatabaseOperations::getAllUsers() {
    std::vector<User> users;
    MYSQL_RES* result = executeSelectQuery("SELECT * FROM users");
    
    if (!result) {
        return users;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        User user;
        user.id = std::stoi(row[0]);
        user.card_id = row[1];
        user.name = row[2];
        user.email = row[3];
        user.phone = row[4];
        users.push_back(user);
    }
    
    mysql_free_result(result);
    return users;
}

bool DatabaseOperations::updateUser(const User& user) {
    std::stringstream ss;
    ss << "UPDATE users SET "
       << "name = '" << escapeString(user.name) << "', "
       << "email = '" << escapeString(user.email) << "', "
       << "phone = '" << escapeString(user.phone) << "' "
       << "WHERE card_id = '" << escapeString(user.card_id) << "'";
    
    return executeQuery(ss.str());
}

bool DatabaseOperations::deleteUser(const std::string& card_id) {
    std::string query = "DELETE FROM users WHERE card_id = '" + escapeString(card_id) + "'";
    return executeQuery(query);
}

bool DatabaseOperations::returnBook(const std::string& book_qr) {
    
    // 1. 執行 return_book procedure
    std::stringstream ss;
    ss << "CALL return_book('" 
       << escapeString(book_qr) << "', @status)";
    
    if (!executeQuery(ss.str())) {
        return false;
    }

    // 2. 檢查執行結果
    MYSQL_RES* result = executeSelectQuery("SELECT @status");
    if (!result) {
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    std::string status = row ? row[0] : "";
    mysql_free_result(result);
    
    return status == "Success";
}

std::vector<BorrowRecord> DatabaseOperations::getUserBorrowHistory(const std::string& user_card) {
    std::vector<BorrowRecord> records;
    std::string query = 
        "SELECT br.* FROM borrow_records br "
        "JOIN users u ON br.user_id = u.user_id "
        "WHERE u.card_id = '" + escapeString(user_card) + "' "
        "ORDER BY br.borrow_date DESC";
        
    MYSQL_RES* result = executeSelectQuery(query);
    
    if (!result) {
        return records;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        BorrowRecord record;
        record.record_id = std::stoi(row[0]);
        record.book_id = std::stoi(row[1]);
        record.user_id = std::stoi(row[2]);
        record.borrow_date = row[3];
        record.due_date = row[4];
        record.return_date = row[5] ? std::optional<std::string>(row[5]) : std::nullopt;
        records.push_back(record);
    }
    
    mysql_free_result(result);
    return records;
}

std::vector<BorrowRecord> DatabaseOperations::getBookBorrowHistory(const std::string& book_qr) {
    std::vector<BorrowRecord> records;
    std::string query = 
        "SELECT br.* FROM borrow_records br "
        "JOIN books b ON br.book_id = b.book_id "
        "WHERE b.qr_code = '" + escapeString(book_qr) + "' "
        "ORDER BY br.borrow_date DESC";
        
    MYSQL_RES* result = executeSelectQuery(query);
    
    if (!result) {
        return records;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        BorrowRecord record;
        record.record_id = std::stoi(row[0]);
        record.book_id = std::stoi(row[1]);
        record.user_id = std::stoi(row[2]);
        record.borrow_date = row[3];
        record.due_date = row[4];
        record.return_date = row[5] ? std::optional<std::string>(row[5]) : std::nullopt;
        records.push_back(record);
    }
    
    mysql_free_result(result);
    return records;
}