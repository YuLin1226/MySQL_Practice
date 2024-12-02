
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