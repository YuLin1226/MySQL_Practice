-- 清除舊的資料庫（如果存在）
DROP DATABASE IF EXISTS inventory;
CREATE DATABASE inventory
    DEFAULT CHARACTER SET = 'utf8mb4';

USE inventory;

-- 建立產品表
CREATE TABLE products (
    product_id INT PRIMARY KEY AUTO_INCREMENT,
    sku VARCHAR(50) UNIQUE NOT NULL,         -- 產品編號
    name VARCHAR(100) NOT NULL,              -- 產品名稱
    description TEXT,                        -- 產品描述
    category VARCHAR(50),                    -- 產品類別
    unit_price DECIMAL(10,2) NOT NULL,       -- 單價
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- 建立庫存表
CREATE TABLE inventory (
    inventory_id INT PRIMARY KEY AUTO_INCREMENT,
    product_id INT NOT NULL,
    quantity INT NOT NULL DEFAULT 0,         -- 目前數量
    min_quantity INT NOT NULL DEFAULT 0,     -- 最小庫存量
    max_quantity INT,                        -- 最大庫存量
    location VARCHAR(50),                    -- 存放位置
    FOREIGN KEY (product_id) REFERENCES products(product_id),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- 建立庫存變更記錄表
CREATE TABLE inventory_movements (
    movement_id INT PRIMARY KEY AUTO_INCREMENT,
    product_id INT NOT NULL,
    type ENUM('in', 'out') NOT NULL,        -- 入庫或出庫
    quantity INT NOT NULL,                   -- 變更數量
    reason VARCHAR(100),                     -- 變更原因
    note TEXT,                              -- 備註
    FOREIGN KEY (product_id) REFERENCES products(product_id),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 建立觸發器：當有庫存變更時自動更新庫存數量
DELIMITER //
CREATE TRIGGER after_movement_insert
AFTER INSERT ON inventory_movements
FOR EACH ROW
BEGIN
    IF NEW.type = 'in' THEN
        UPDATE inventory 
        SET quantity = quantity + NEW.quantity
        WHERE product_id = NEW.product_id;
    ELSE
        UPDATE inventory 
        SET quantity = quantity - NEW.quantity
        WHERE product_id = NEW.product_id;
    END IF;
END //
DELIMITER ;