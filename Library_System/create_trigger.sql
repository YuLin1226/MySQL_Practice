-- Active: 1733128601746@@127.0.0.1@3306@My_First_DB

DROP TRIGGER IF EXISTS after_return_update_stock;
DELIMITER //
CREATE TRIGGER after_return_update_stock
AFTER UPDATE ON borrow_records
FOR EACH ROW
BEGIN
    -- 變數宣告必須在最開始
    DECLARE v_reservation_id INT;
    DECLARE v_user_id INT;
    
    -- 當有人還書時（return_date從NULL變成有日期）
    IF NEW.return_date IS NOT NULL AND OLD.return_date IS NULL THEN
        -- 找出最早預約且狀態為waiting的預約記錄
        SELECT reservation_id, user_id 
        INTO v_reservation_id, v_user_id
        FROM book_reservations
        WHERE book_id = NEW.book_id
        AND status = 'waiting'
        ORDER BY reservation_date
        LIMIT 1;
        
        -- 如果有預約
        IF v_reservation_id IS NOT NULL THEN
            -- 更新預約狀態為available並記錄通知時間
            -- available 代表通知預約者可以來借書
            UPDATE book_reservations
            SET status = 'available',
                notification_date = CURRENT_TIMESTAMP
            WHERE reservation_id = v_reservation_id;
            
            -- 書籍狀態改為reserved（表示被預約中）
            UPDATE books 
            SET status = 'reserved'
            WHERE book_id = NEW.book_id;
        ELSE
            -- 如果沒有預約，就將書籍狀態改為可借閱
            UPDATE books 
            SET status = 'available'
            WHERE book_id = NEW.book_id;
        END IF;
    END IF;
END //
DELIMITER ;
