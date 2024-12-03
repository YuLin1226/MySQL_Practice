DELIMITER //
CREATE PROCEDURE borrow_book(
    IN p_card_id VARCHAR(50),    -- 使用者卡號
    IN p_qr_code VARCHAR(50),    -- 書籍 QR 碼
    OUT p_status VARCHAR(100)    -- 執行狀態
)
BEGIN
    DECLARE v_book_id INT;
    DECLARE v_user_id INT;
    DECLARE v_book_status VARCHAR(20);
    DECLARE v_user_borrowed INT;
    
    -- 先找出對應的 book_id 和 user_id
    SELECT book_id, status INTO v_book_id, v_book_status
    FROM books 
    WHERE qr_code = p_qr_code;
    
    SELECT user_id INTO v_user_id
    FROM users 
    WHERE card_id = p_card_id;
    
    -- 檢查用戶已借數量
    SELECT COUNT(*) INTO v_user_borrowed 
    FROM borrow_records 
    WHERE user_id = v_user_id AND return_date IS NULL;
    
    -- 驗證並執行借書
    IF v_book_id IS NULL THEN
        SET p_status = 'Book not found';
    ELSEIF v_user_id IS NULL THEN
        SET p_status = 'User not found';
    ELSEIF v_book_status = 'borrowed' THEN
        SET p_status = 'Book is already borrowed';
    ELSEIF v_user_borrowed >= 5 THEN
        SET p_status = 'User has reached maximum borrowing limit (5 books)';
    ELSE
        START TRANSACTION;
        
        -- 更新書籍狀態
        UPDATE books 
        SET status = 'borrowed'
        WHERE qr_code = p_qr_code;
        
        -- 創建借閱記錄
        INSERT INTO borrow_records (
            book_id, 
            user_id, 
            borrow_date, 
            due_date
        )
        VALUES (
            v_book_id, 
            v_user_id, 
            CURRENT_DATE, 
            DATE_ADD(CURRENT_DATE, INTERVAL 14 DAY)
        );
        
        COMMIT;
        SET p_status = 'Success';
    END IF;

END //
DELIMITER ;

-- 借書示例
CALL borrow_book('USER00000001', 'BOOK00000001', @status);
SELECT @status;

CALL borrow_book('USER00000002', 'BOOK00000002', @status);
SELECT @status;