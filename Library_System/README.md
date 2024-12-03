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


## 借書程序講解

#### [DELIMITER](https://blog.csdn.net/pan_junbiao/article/details/86291722)

```sql
DELIMITER //
DELIMITER ;
```
這個用於改變 SQL 的行句結尾符，因為一般的 MySQL 是 `;` 結尾就會執行該行指令。
而這個 `DELIMITER` 可以先暫停中間不執行，等到你把所有部份定義完再執行。

#### PROCEDURE

有點像是在定義函數的概念，語法是
```sql
CREATE PROCEDURE borrow_book(
    IN p_card_id VARCHAR(50),    -- 使用者卡號
    IN p_qr_code VARCHAR(50),    -- 書籍 QR 碼
    OUT p_status VARCHAR(100)    -- 執行狀態
)
BEGIN
    -- 省略中間。
END
```
- `IN` 表 Input
- `OUT` 表 Output
- 執行該 Procedure 方式為 
    ```sql
    CALL borrow_book('USER00000001', 'BOOK00000001', @status);
    SELECT @status;
    ```

內部程序的內容大概分成
1. 參數宣告：`DECLARE`。
2. 根據需要的邏輯去賦值參數，大概就是`SELECT, FROM, WHERE`的操作。
3. `IF ELSE` 的寫法和 Python 挺像的。
4. `TRANSACTION` 的[用法](https://medium.com/@martin87713/mysql-transaction-a-k-a-%E4%BA%A4%E6%98%93or%E4%BA%8B%E5%8B%99-70b19b3c9953)則是讓定義的操作變得一致，表示該內容全部執行（成功），或者全部不執行（失敗）。
    > 我有附上相關參考連結。

在這個程序的定義裡面，我認為 `COUNT` 或許可以利用修改 `USER` 的結構來避免掉這個操作，畢竟一個使用者借了幾本書應該也算是滿容易被定義在資料庫裡的。但基於我仍是一名初學者，我不確定以下兩種方法的效率何者較佳：
- 利用 `COUNT` 來慢慢數一個 `USER` 來借了幾本書，內容涉及搜尋`borrow_records` 的操作。
- 直接在 `USER` 結構裡面新增一個 `Number of borrowed books`，然後每次借閱、還書的時候，都會涵蓋修改該資料的操作。
 

## 還書程序講解

基本上與借書差不多，所以雷同語法跳過。
比較不一樣的用法在於

```sql
-- 檢查是否有未完成的借閱記錄
SELECT record_id 
INTO v_borrow_record_id
FROM borrow_records
WHERE book_id = v_book_id 
AND return_date IS NULL
ORDER BY borrow_date DESC
LIMIT 1;
```

這邊的 `ORDER ...` 與 `LIMIT 1`
- `ORDER` 表示排序，根據 `borrow_date` 來 `DESC` 降序排列（從大到小，新到舊）。

    `ASC`：升序排列（從小到大，舊到新）
- `LIMIT 1` 取第一筆資料。

## Trigger 用法

```sql
CREATE TRIGGER after_return_update_stock
AFTER UPDATE ON borrow_records
FOR EACH ROW
BEGIN
    -- 變數宣告必須在最開始
    DECLARE v_reservation_id INT;
    DECLARE v_user_id INT;
END //
```

基本上長這樣。
然後裡面比較特別的地方是和一般的語言一樣，變數要作用域的一開始就定義，不然在 IF ELSE 裡面才宣告的話，可能會有`NEAR v_reservation_id INT` 這種錯誤。特別記一下，因為 Claude 一開始給我的範本就犯了這個錯誤。

另外，`NEW` 和 `OLD` 代表的是：
- `NEW`: 資料變更後的狀態。
- `OLD`: 資料變更前的狀態。

假設我們有這樣一筆借閱記錄：
```sql
-- 原始資料（還沒還書時）
record_id | book_id | user_id | borrow_date | due_date    | return_date
1         | 101     | 201     | 2024-02-01  | 2024-02-15  | NULL
```

當使用者還書時，我們執行：
```sql
UPDATE borrow_records
SET return_date = '2024-02-10'
WHERE record_id = 1;
```

在觸發器中：
- `OLD` 代表更新前的資料行：
```sql
OLD.record_id = 1
OLD.book_id = 101
OLD.user_id = 201
OLD.borrow_date = '2024-02-01'
OLD.due_date = '2024-02-15'
OLD.return_date = NULL  -- 更新前是 NULL
```

- `NEW` 代表更新後的資料行：
```sql
NEW.record_id = 1
NEW.book_id = 101
NEW.user_id = 201
NEW.borrow_date = '2024-02-01'
NEW.due_date = '2024-02-15'
NEW.return_date = '2024-02-10'  -- 更新後有了日期
```

所以在觸發器中的這行判斷：
```sql
IF NEW.return_date IS NOT NULL AND OLD.return_date IS NULL THEN
```
就是在檢查：
1. 更新前 return_date 是 NULL（OLD.return_date IS NULL）
2. 更新後 return_date 有值（NEW.return_date IS NOT NULL）
3. 這兩個條件同時成立，就代表這是一個還書的動作

不同的觸發時機可以使用的變數：
1. INSERT 觸發器：只能使用 NEW（因為是新增資料）
2. DELETE 觸發器：只能使用 OLD（因為是刪除資料）
3. UPDATE 觸發器：可以同時使用 NEW 和 OLD（因為是修改資料）