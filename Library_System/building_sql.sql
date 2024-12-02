-- Active: 1733128601746@@127.0.0.1@3306
CREATE DATABASE My_First_DB
    DEFAULT CHARACTER SET = 'utf8mb4';

USE My_First_DB;

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

SELECT DATABASE();

SHOW TABLES;

DESCRIBE books;
DESCRIBE users;
DESCRIBE borrow_records;

INSERT INTO books (title, author, isbn, publication_year) VALUES
('哈利波特：神秘的魔法石', 'J.K. 羅琳', '9573317249', 1997),
('魔戒首部曲：魔戒現身', 'J.R.R. 托爾金', '9573318187', 1954),
('小王子', '安東尼·德聖修伯里', '9573275235', 1943);

INSERT INTO users (name, email, phone) VALUES
('陳小明', 'ming@example.com', '0912345678'),
('王小華', 'wang@example.com', '0923456789'),
('林小美', 'mei@example.com', '0934567890');


DELETE FROM borrow_records;
DELETE FROM users;
DELETE FROM books;

-- 在已有資料的情況下，新增欄位的方法
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

ALTER Table users
ADD COLUMN card_id VARCHAR(50) UNIQUE AFTER user_id;

UPDATE users SET card_id = CONCAT('USER', LPAD(user_id, 8, '0'));

ALTER TABLE users
MODIFY COLUMN card_id VARCHAR(50) UNIQUE NOT NULL;


-- 一次新增多筆資料
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