# C++ 編譯問題


目前使用 MySQL 的 API，在編譯上的時候可能會沒抓到 API 的函式呼叫，例如：
```cpp
connection_ = mysql_init(nullptr);
```
導致 `undefined reference to 'mysql_init'` 這個錯誤語句。

原因是因為沒有連結到 mysqlclient 這個 lib，所以在 `CMakeLists.txt` 裡面改一下即可：
```txt
target_link_libraries(library_system PRIVATE
    mysqlclient
)
```


# C++ MySQL Query Guide

## Basic Query Pattern

在 C++ 中執行 SQL 查詢的基本模式如下：

1. 構建 SQL 查詢字串（使用 `std::string` 或 `std::stringstream`）
2. 執行查詢（使用 `mysql_query()`）
3. 處理結果（如果有返回結果的話）

## Query Types and Examples

### 1. INSERT 查詢

```sql
INSERT INTO users (card_id, name, email, phone) 
VALUES ('USER001', '張三', 'zhang@example.com', '0912345678')
```

C++ 實現：
```cpp
std::stringstream ss;
ss << "INSERT INTO users (card_id, name, email, phone) VALUES ('"
   << escapeString(user.card_id) << "', '"
   << escapeString(user.name) << "', '"
   << escapeString(user.email) << "', '"
   << escapeString(user.phone) << "')";

// 執行查詢
if (mysql_query(connection, ss.str().c_str()) != 0) {
    // 處理錯誤
    return false;
}
```

### 2. SELECT 查詢

```sql
SELECT * FROM books WHERE qr_code = 'BOOK001'
```

C++ 實現：
```cpp
std::string query = "SELECT * FROM books WHERE qr_code = '" + escapeString(qr_code) + "'";

// 執行查詢
if (mysql_query(connection, query.c_str()) != 0) {
    return false;
}

// 獲取結果集
MYSQL_RES* result = mysql_store_result(connection);
if (result) {
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        // 處理數據
        book.id = std::stoi(row[0]);
        book.title = row[1];
        // ... 處理其他欄位
    }
    mysql_free_result(result);
}
```

### 3. UPDATE 查詢

```sql
UPDATE books 
SET title = '新書名', author = '新作者' 
WHERE qr_code = 'BOOK001'
```

C++ 實現：
```cpp
std::stringstream ss;
ss << "UPDATE books SET "
   << "title = '" << escapeString(book.title) << "', "
   << "author = '" << escapeString(book.author) << "' "
   << "WHERE qr_code = '" << escapeString(book.qr_code) << "'";

if (mysql_query(connection, ss.str().c_str()) != 0) {
    return false;
}
```

### 4. DELETE 查詢

```sql
DELETE FROM books WHERE qr_code = 'BOOK001'
```

C++ 實現：
```cpp
std::string query = "DELETE FROM books WHERE qr_code = '" + 
                    escapeString(qr_code) + "'";

if (mysql_query(connection, query.c_str()) != 0) {
    return false;
}
```

## 重要注意事項

1. **SQL 注入防護**
   - 所有用戶輸入都必須使用 `mysql_real_escape_string()` 進行轉義
   ```cpp
   char* escaped = new char[str.length() * 2 + 1];
   mysql_real_escape_string(connection, escaped, str.c_str(), str.length());
   std::string result(escaped);
   delete[] escaped;
   ```

2. **錯誤處理**
   - 每次查詢後都要檢查返回值
   - 使用 `mysql_error()` 獲取詳細錯誤信息
   ```cpp
   if (mysql_query(connection, query.c_str()) != 0) {
       std::cerr << "Error: " << mysql_error(connection) << std::endl;
   }
   ```

3. **資源管理**
   - 務必釋放結果集
   ```cpp
   MYSQL_RES* result = mysql_store_result(connection);
   // 使用結果集...
   mysql_free_result(result);
   ```

4. **交易處理**
   ```cpp
   mysql_query(connection, "START TRANSACTION");
   // 執行多個查詢...
   if (/* 所有操作成功 */) {
       mysql_query(connection, "COMMIT");
   } else {
       mysql_query(connection, "ROLLBACK");
   }
   ```


# SQL Escaped String Guide

## 什麼是 Escaped String？

Escaped string 是一種將特殊字符轉換為安全形式的處理方式，主要用於防止 SQL 注入攻擊。在 SQL 查詢中，某些字符具有特殊含義，如單引號(')、反斜線(\)等，這些字符需要被正確處理以避免安全問題。

## 為什麼需要 Escaped String？

考慮以下情況：

```cpp
// 危險的代碼 - 不要使用！
std::string title = "My's Book'; DROP TABLE books; --";
std::string query = "INSERT INTO books (title) VALUES ('" + title + "')";
```

未經處理的 SQL 查詢會變成：
```sql
INSERT INTO books (title) VALUES ('My's Book'; DROP TABLE books; --')
```

這可能導致：
1. SQL 語法錯誤
2. SQL 注入攻擊（在這個例子中，books 表會被刪除！）

## 正確的 Escaped String 處理

### 1. 使用 mysql_real_escape_string()

```cpp
std::string escapeString(const std::string& str) {
    DatabaseConnection& db = DatabaseConnection::getInstance();
    MYSQL* conn = db.getRawConnection();
    
    // 分配足夠的空間（原字符串長度的兩倍加1）
    char* escaped = new char[str.length() * 2 + 1];
    
    // 進行轉義
    mysql_real_escape_string(conn, escaped, str.c_str(), str.length());
    
    // 轉換為 string 並釋放記憶體
    std::string result(escaped);
    delete[] escaped;
    
    return result;
}
```

### 2. 轉義的效果

| 原始字符 | 轉義後 |
|---------|--------|
| `'`     | `\'`   |
| `"`     | `\"`   |
| `\`     | `\\`   |
| `\n`    | `\n`   |
| `\r`    | `\r`   |
| `\t`    | `\t`   |

### 3. 實際使用例子

```cpp
// 用戶輸入
std::string userInput = "O'Reilly's Book";

// 錯誤的方式
std::string badQuery = "INSERT INTO books (title) VALUES ('" + userInput + "')";
// 結果: INSERT INTO books (title) VALUES ('O'Reilly's Book')
// 這會導致 SQL 語法錯誤！

// 正確的方式
std::string safeInput = escapeString(userInput);
std::string goodQuery = "INSERT INTO books (title) VALUES ('" + safeInput + "')";
// 結果: INSERT INTO books (title) VALUES ('O\'Reilly\'s Book')
// 這是安全的 SQL 查詢
```

## 最佳實踐

1. **永遠不要信任用戶輸入**
   - 所有來自用戶的輸入都必須進行轉義

2. **使用預處理語句**
   - 更好的方法是使用預處理語句（Prepared Statements）
   - MySQL C API 提供 `mysql_stmt_prepare()`

3. **檢查轉義結果**
   ```cpp
   if (!escaped) {
       // 處理錯誤
       throw std::runtime_error("Failed to escape string");
   }
   ```

4. **記憶體管理**
   - 使用 RAII 原則管理轉義字符串的記憶體
   - 使用智能指針或確保適當的記憶體釋放

## 注意事項

- 記得為轉義字符串分配足夠的空間（原字符串長度的兩倍加1）
- 不要對已經轉義的字符串再次進行轉義
- 確保在建立資料庫連接後再使用 mysql_real_escape_string


# Diff between `executeQuery` & `executeSelectQuery`

讓我解釋這兩個函數的主要差異。關鍵在於它們處理的 SQL 操作類型和回傳值不同：

`executeQuery` 函數：
```cpp
bool executeQuery(const std::string& query) {
    DatabaseConnection& db = DatabaseConnection::getInstance();
    return mysql_query(db.getRawConnection(), query.c_str()) == 0;
}
```
- 用於執行不需要返回結果的 SQL 語句（INSERT、UPDATE、DELETE）
- 返回 bool 值表示操作是否成功
- 只關心執行成功與否，不需要處理結果集

`executeSelectQuery` 函數：
```cpp
MYSQL_RES* executeSelectQuery(const std::string& query) {
    DatabaseConnection& db = DatabaseConnection::getInstance();
    if (mysql_query(db.getRawConnection(), query.c_str()) != 0) {
        return nullptr;
    }
    return mysql_store_result(db.getRawConnection());
}
```
- 專門用於執行 SELECT 查詢
- 返回 MYSQL_RES* （結果集指針）
- 需要額外處理返回的數據結果集

使用範例：

1. executeQuery 的使用（INSERT 操作）：
```cpp
bool createUser(const User& user) {
    std::stringstream ss;
    ss << "INSERT INTO users (name, email) VALUES ('"
       << escapeString(user.name) << "', '"
       << escapeString(user.email) << "')";
    
    return executeQuery(ss.str());  // 只需要知道是否成功
}
```

2. executeSelectQuery 的使用（SELECT 操作）：
```cpp
std::optional<User> getUser(const std::string& id) {
    std::string query = "SELECT * FROM users WHERE id = '" + escapeString(id) + "'";
    
    MYSQL_RES* result = executeSelectQuery(query);  // 需要處理返回的結果集
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
    user.name = row[1];
    user.email = row[2];
    
    mysql_free_result(result);
    return user;
}
```

主要區別：
1. **用途不同**：
   - executeQuery：用於修改數據（INSERT/UPDATE/DELETE）
   - executeSelectQuery：用於查詢數據（SELECT）

2. **返回值不同**：
   - executeQuery：返回布爾值（成功/失敗）
   - executeSelectQuery：返回結果集指針

3. **記憶體管理**：
   - executeQuery：不需要額外的記憶體管理
   - executeSelectQuery：需要手動釋放結果集（mysql_free_result）

4. **使用場景**：
   - executeQuery：適用於不需要回傳資料的操作
   - executeSelectQuery：適用於需要獲取資料的查詢