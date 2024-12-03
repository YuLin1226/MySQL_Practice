-- Active: 1733128601746@@127.0.0.1@3306@My_First_DB
DELIMITER //
CREATE PROCEDURE return_book(
    IN p_qr_code VARCHAR(50),     -- 書籍 QR 碼
    OUT p_status VARCHAR(100)     -- 執行狀態
)
BEGIN
    DECLARE v_book_id INT;
    DECLARE v_book_status VARCHAR(20);
    DECLARE v_borrow_record_id INT;
    
    -- 檢查書籍是否存在及其狀態
    SELECT book_id, status 
    INTO v_book_id, v_book_status
    FROM books 
    WHERE qr_code = p_qr_code;
    
    -- 檢查是否有未完成的借閱記錄
    SELECT record_id 
    INTO v_borrow_record_id
    FROM borrow_records
    WHERE book_id = v_book_id 
    AND return_date IS NULL
    ORDER BY borrow_date DESC
    LIMIT 1;
    
    -- 驗證並執行還書
    IF v_book_id IS NULL THEN
        SET p_status = 'Book not found';
    ELSEIF v_book_status = 'available' THEN
        SET p_status = 'Book is already returned';
    ELSEIF v_borrow_record_id IS NULL THEN
        SET p_status = 'No active borrowing record found';
    ELSE
        START TRANSACTION;
        
        -- 更新書籍狀態
        UPDATE books 
        SET status = 'available'
        WHERE qr_code = p_qr_code;
        
        -- 更新借閱記錄
        UPDATE borrow_records
        SET return_date = CURRENT_DATE
        WHERE record_id = v_borrow_record_id;
        
        COMMIT;
        SET p_status = 'Success';
    END IF;

END //
DELIMITER ;

-- 還書示例
CALL return_book('BOOK00000001', @status);
SELECT @status;

CALL return_book('BOOK00000002', @status);
SELECT @status;