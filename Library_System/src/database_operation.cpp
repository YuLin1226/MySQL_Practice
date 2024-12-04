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
    if (mysql_query(db.getRawConnection(), query.c_str()) != 0) {
        return nullptr;
    }
    return mysql_store_result(db.getRawConnection());
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
    DatabaseConnection& db = DatabaseConnection::getInstance();
    MYSQL* conn = db.getRawConnection();
    
    // Start transaction
    if (mysql_query(conn, "START TRANSACTION") != 0) {
        return false;
    }
    
    try {
        // Update book status
        std::string updateQuery = "UPDATE books SET status = 'borrowed' WHERE qr_code = '" + 
                                escapeString(book_qr) + "'";
        if (!executeQuery(updateQuery)) {
            mysql_query(conn, "ROLLBACK");
            return false;
        }
        
        // Create borrow record
        std::stringstream ss;
        ss << "INSERT INTO borrow_records (book_id, user_id, borrow_date, due_date) "
           << "SELECT b.book_id, u.user_id, CURRENT_DATE, DATE_ADD(CURRENT_DATE, INTERVAL 14 DAY) "
           << "FROM books b JOIN users u "
           << "WHERE b.qr_code = '" << escapeString(book_qr) << "' "
           << "AND u.card_id = '" << escapeString(user_card) << "'";
        
        if (!executeQuery(ss.str())) {
            mysql_query(conn, "ROLLBACK");
            return false;
        }
        
        return mysql_query(conn, "COMMIT") == 0;
    }
    catch (...) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }
}

// User Operations
bool DatabaseOperations::createUser(const User& user) {
    std::stringstream ss;
    ss << "INSERT INTO users (card_id, name, email, phone) VALUES ('"
       << escapeString(user.card_id) << "', '"
       << escapeString(user.name) << "', '"
       << escapeString(user.email) << "', '"
       << escapeString(user.phone) << "')";
    
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
    DatabaseConnection& db = DatabaseConnection::getInstance();
    MYSQL* conn = db.getRawConnection();
    
    // Start transaction
    if (mysql_query(conn, "START TRANSACTION") != 0) {
        return false;
    }
    
    try {
        // Update borrow record
        std::string updateBorrowQuery = 
            "UPDATE borrow_records SET return_date = CURRENT_DATE "
            "WHERE book_id = (SELECT book_id FROM books WHERE qr_code = '" + 
            escapeString(book_qr) + "') "
            "AND return_date IS NULL";
            
        if (!executeQuery(updateBorrowQuery)) {
            mysql_query(conn, "ROLLBACK");
            return false;
        }
        
        // Update book status
        std::string updateBookQuery = 
            "UPDATE books SET status = 'available' "
            "WHERE qr_code = '" + escapeString(book_qr) + "'";
            
        if (!executeQuery(updateBookQuery)) {
            mysql_query(conn, "ROLLBACK");
            return false;
        }
        
        return mysql_query(conn, "COMMIT") == 0;
    }
    catch (...) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }
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