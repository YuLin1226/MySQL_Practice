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