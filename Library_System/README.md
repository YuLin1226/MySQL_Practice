## 連接到特定資料庫
```sql
CREATE DATABASE My_First_DB
    DEFAULT CHARACTER SET = 'utf8mb4';

USE My_First_DB;
```


## 建立 TABLE 的方法

```sql
CREATE TABLE books (
    book_id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(100) NOT NULL,
    author VARCHAR(50),
    isbn VARCHAR(13),
    publication_year YEAR,
    status ENUM('available', 'borrowed') DEFAULT 'available',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE users (
    user_id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(50) NOT NULL,
    email VARCHAR(100) UNIQUE,
    phone VARCHAR(20),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE borrow_records (
    record_id INT PRIMARY KEY AUTO_INCREMENT,
    book_id INT,
    user_id INT, 
    borrow_date DATE NOT NULL,
    due_date DATE NOT NULL,
    return_date DATE,
    FOREIGN KEY (book_id) REFERENCES books(book_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id)
);
```

- 主鍵(Primary Key):
  - 主鍵是用來唯一標識表中每一筆資料的欄位。
  - 每個表只能有一個主鍵，而且主鍵的值不能重複也不能為空。
  - 通常我們會選擇一個具有唯一性的欄位作為主鍵，例如學生ID、商品編號等。

- 外鍵(Foreign Key):
  - 外鍵是用來建立表與表之間關係的欄位。
  - 外鍵參照其他表的主鍵，用來確保資料的一致性和完整性。
  - 一個表可以有多個外鍵。

## 選擇資料庫與查詢表格

```sql
SELECT DATABASE();

SHOW TABLES;

DESCRIBE books;
DESCRIBE users;
DESCRIBE borrow_records;
```

DELETE 和 TRUNCATE 這兩個清除資料的指令的主要差異：

1. 執行方式
   - DELETE：一筆一筆刪除資料，會在交易日誌(Transaction Log)中記錄每一筆刪除操作
   - TRUNCATE：直接釋放資料表所占用的資料頁(Data Pages)，只記錄頁面的釋放


2. 執行速度
   - DELETE：較慢，因為要一條一條執行並記錄日誌
   - TRUNCATE：較快，因為是整表刪除，不需逐筆處理


3. WHERE 條件句
   - DELETE：可以使用 WHERE 條件句選擇性刪除
       ```sql
       DELETE FROM employees WHERE department = 'IT';
       ```
   - TRUNCATE：不能使用 WHERE，只能刪除整張表
       ```sql
       TRUNCATE TABLE employees;
       ``````

4. 自動編號(Identity)處理
   - DELETE：刪除後，自動編號接續原本的序號
   - TRUNCATE：重置自動編號回初始值（通常是 1）


5. 交易(Transaction)處理
   - DELETE：可以 ROLLBACK（復原）
   - TRUNCATE：通常不能 ROLLBACK（某些資料庫系統例外）


6. 觸發器(Trigger)
   - DELETE：會觸發表上的 DELETE 觸發器
   - TRUNCATE：不會觸發任何觸發器

7. 使用建議：
   - 如果要刪除整張表的資料且不需要復原，用 TRUNCATE 較有效率
   - 如果需要條件式刪除或要保留刪除記錄，用 DELETE 較適合



## 新增資料進表格之中

```sql

INSERT INTO books (title, author, isbn, publication_year) VALUES
('哈利波特：神秘的魔法石', 'J.K. 羅琳', '9573317249', 1997),
('魔戒首部曲：魔戒現身', 'J.R.R. 托爾金', '9573318187', 1954),
('小王子', '安東尼·德聖修伯里', '9573275235', 1943);

INSERT INTO users (name, email, phone) VALUES
('陳小明', 'ming@example.com', '0912345678'),
('王小華', 'wang@example.com', '0923456789'),
('林小美', 'mei@example.com', '0934567890');
```

## 刪除資料

```sql
DELETE FROM borrow_records;
DELETE FROM users;
DELETE FROM books;
```

## 在已有資料的情況下，新增欄位的方法

```sql
    -- 1. 先新增 qr_code 欄位，但暫時允許 NULL
ALTER TABLE books
ADD COLUMN qr_code VARCHAR(50) UNIQUE AFTER book_id;

    -- 2. 更新現有資料的 qr_code
UPDATE books SET qr_code = CONCAT('BOOK', LPAD(book_id, 8, '0'));

    -- 3. 最後設定 NOT NULL 限制
ALTER TABLE books
MODIFY COLUMN qr_code VARCHAR(50) UNIQUE NOT NULL;

    -- 4. 新增一筆新資料
INSERT INTO books (title, author, isbn, publication_year, qr_code) 
SELECT 
    '一九八四',
    '喬治·歐威爾',
    '9573317249',
    1949,
    CONCAT('BOOK', LPAD((SELECT COALESCE(MAX(book_id) + 1, 1) FROM books b), 8, '0'))
;
```

操作同上
```sql
ALTER Table users
ADD COLUMN card_id VARCHAR(50) UNIQUE AFTER user_id;

UPDATE users SET card_id = CONCAT('USER', LPAD(user_id, 8, '0'));

ALTER TABLE users
MODIFY COLUMN card_id VARCHAR(50) UNIQUE NOT NULL;
```

## 一次新增多筆資料

```sql
INSERT INTO books (title, author, isbn, publication_year, qr_code) 
SELECT 
    '一九八四',
    '喬治·歐威爾',
    '9573317249',
    1949,
    CONCAT('BOOK', LPAD((SELECT COALESCE(MAX(book_id) + 1, 1) FROM books b), 8, '0'))
UNION ALL
SELECT 
    '美麗新世界',
    '阿道斯·赫胥黎',
    '9573317250',
    1932,
    CONCAT('BOOK', LPAD((SELECT COALESCE(MAX(book_id) + 2, 2) FROM books b), 8, '0'))
;
```

## 借還書方法（但我覺得這應該做在其他程式語言）

```sql

-- 關聯書籍與使用者資料，來新增借閱資料的範例：
-- 借出前要把book的狀態修改，然後再新增借閱資料。
START TRANSACTION;

UPDATE books 
SET status = 'borrowed'
WHERE qr_code = 'BOOK00000001';

INSERT INTO borrow_records (
    book_id,
    user_id,
    borrow_date,
    due_date
)
SELECT 
    b.book_id,
    u.user_id,
    CURRENT_DATE,
    DATE_ADD(CURRENT_DATE, INTERVAL 14 DAY)
FROM books b
JOIN users u
WHERE b.qr_code = 'BOOK00000001'
AND u.card_id = 'USER00000001';

COMMIT;


-- 還書：
START TRANSACTION;

-- 1. 更新借閱記錄
UPDATE borrow_records
SET return_date = CURRENT_DATE
WHERE book_id = (SELECT book_id FROM books WHERE qr_code = 'BOOK00000001')
AND return_date IS NULL;

-- 2. 更新書籍狀態
UPDATE books 
SET status = 'available'
WHERE qr_code = 'BOOK00000001';

COMMIT;
```

## 搜尋資料庫相關內容

```sql
-- 查詢某用戶的借閱歷史
SELECT b.title, br.borrow_date, br.due_date, br.return_date
FROM borrow_records br
JOIN books b ON br.book_id = b.book_id
WHERE br.user_id = (SELECT user_id FROM users WHERE card_id = 'USER00000001');

-- 查詢逾期未還的書籍
SELECT b.title, u.name, br.due_date
FROM borrow_records br
JOIN books b ON br.book_id = b.book_id
JOIN users u ON br.user_id = u.user_id
WHERE br.return_date IS NULL 
AND br.due_date < CURRENT_DATE;

-- 查詢方法的差異
    -- 未優化的查詢：列出所有欄位
SELECT * FROM books WHERE status = 'available';
    -- 優化後的查詢：只選取需要的欄位
SELECT title, author, qr_code 
FROM books 
WHERE status = 'available';
```

## 建立 INDEX 與搜尋

```sql
-- books 表格
CREATE INDEX idx_books_status ON books(status);  
-- 因為我們常常需要查詢可借閱的書籍
-- borrow_records 表格
CREATE INDEX idx_borrow_dates ON borrow_records(borrow_date, due_date);
-- 因為我們需要查詢借閱日期和到期日
CREATE INDEX idx_book_user ON borrow_records(book_id, user_id);
-- 因為我們常用這兩個欄位來JOIN查詢

-- 使用索引的複雜查詢
SELECT 
    b.title,
    u.name,
    br.borrow_date,
    br.due_date
FROM borrow_records br
FORCE INDEX (idx_book_user)  -- 強制使用特定索引
JOIN books b ON br.book_id = b.book_id
JOIN users u ON br.user_id = u.user_id
WHERE br.return_date IS NULL
AND br.due_date < CURRENT_DATE;
```

這部份有點疑慮，我把 Claude 的回答寫在這個[筆記](https://hackmd.io/@YuLin1226/S1C3pWi7yg)上。