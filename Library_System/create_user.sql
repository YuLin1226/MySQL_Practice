-- 創建用戶
CREATE USER 'library_user'@'localhost' IDENTIFIED BY 'password123';

-- 只給予圖書館數據庫的權限
GRANT ALL PRIVILEGES ON My_First_DB.* TO 'library_user'@'localhost';