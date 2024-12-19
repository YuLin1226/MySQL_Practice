-- 清除舊的資料庫（如果存在）
DROP DATABASE IF EXISTS inventory;
CREATE DATABASE inventory
    DEFAULT CHARACTER SET = 'utf8mb4';

USE inventory;

-- 使用者管理
CREATE TABLE users (
    user_id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    status ENUM('active', 'inactive') NOT NULL DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- 建立倉庫表
CREATE TABLE warehouses (
    warehouse_id INT PRIMARY KEY AUTO_INCREMENT,
    code VARCHAR(50) UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    address TEXT,
    status ENUM('active', 'inactive') NOT NULL DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_by INT NOT NULL,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    updated_by INT NOT NULL,
    FOREIGN KEY (created_by) REFERENCES users(user_id),
    FOREIGN KEY (updated_by) REFERENCES users(user_id)
);

-- 建立供應商表
CREATE TABLE suppliers (
    supplier_id INT PRIMARY KEY AUTO_INCREMENT,
    code VARCHAR(50) UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    contact_person VARCHAR(100),
    phone VARCHAR(20),
    email VARCHAR(100),
    address TEXT,
    status ENUM('active', 'inactive') NOT NULL DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_by INT NOT NULL,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    updated_by INT NOT NULL,
    FOREIGN KEY (created_by) REFERENCES users(user_id),
    FOREIGN KEY (updated_by) REFERENCES users(user_id)
);

-- 建立產品表
CREATE TABLE products (
    product_id INT PRIMARY KEY AUTO_INCREMENT,
    sku VARCHAR(50) UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    category VARCHAR(50),
    unit_price DECIMAL(10,2) NOT NULL,
    unit VARCHAR(20) NOT NULL,
    status ENUM('active', 'inactive') NOT NULL DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_by INT NOT NULL,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    updated_by INT NOT NULL,
    FOREIGN KEY (created_by) REFERENCES users(user_id),
    FOREIGN KEY (updated_by) REFERENCES users(user_id)
);

-- 供應商產品關聯表
CREATE TABLE supplier_products (
    supplier_id INT NOT NULL,
    product_id INT NOT NULL,
    supplier_sku VARCHAR(50),
    unit_price DECIMAL(10,2),
    is_primary BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_by INT NOT NULL,
    PRIMARY KEY (supplier_id, product_id),
    FOREIGN KEY (supplier_id) REFERENCES suppliers(supplier_id),
    FOREIGN KEY (product_id) REFERENCES products(product_id),
    FOREIGN KEY (created_by) REFERENCES users(user_id)
);

-- 建立庫存表
CREATE TABLE inventory (
    inventory_id INT PRIMARY KEY AUTO_INCREMENT,
    warehouse_id INT NOT NULL,
    product_id INT NOT NULL,
    quantity INT NOT NULL DEFAULT 0,
    min_quantity INT NOT NULL DEFAULT 0,
    max_quantity INT,
    location VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_by INT NOT NULL,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    updated_by INT NOT NULL,
    UNIQUE KEY uk_warehouse_product (warehouse_id, product_id),
    FOREIGN KEY (warehouse_id) REFERENCES warehouses(warehouse_id),
    FOREIGN KEY (product_id) REFERENCES products(product_id),
    FOREIGN KEY (created_by) REFERENCES users(user_id),
    FOREIGN KEY (updated_by) REFERENCES users(user_id)
);

-- 建立庫存變更記錄表
CREATE TABLE inventory_movements (
    movement_id INT PRIMARY KEY AUTO_INCREMENT,
    warehouse_id INT NOT NULL,
    product_id INT NOT NULL,
    type ENUM('purchase', 'sale', 'transfer_in', 'transfer_out', 'adjustment', 'return_in', 'return_out') NOT NULL,
    quantity INT NOT NULL,
    before_quantity INT NOT NULL,
    after_quantity INT NOT NULL,
    reference_no VARCHAR(50),
    reason VARCHAR(100),
    note TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_by INT NOT NULL,
    FOREIGN KEY (warehouse_id) REFERENCES warehouses(warehouse_id),
    FOREIGN KEY (product_id) REFERENCES products(product_id),
    FOREIGN KEY (created_by) REFERENCES users(user_id)
);

-- 建立庫存調撥表
CREATE TABLE inventory_transfers (
    transfer_id INT PRIMARY KEY AUTO_INCREMENT,
    from_warehouse_id INT NOT NULL,
    to_warehouse_id INT NOT NULL,
    product_id INT NOT NULL,
    quantity INT NOT NULL,
    status ENUM('pending', 'in_transit', 'completed', 'cancelled') NOT NULL DEFAULT 'pending',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_by INT NOT NULL,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    updated_by INT NOT NULL,
    FOREIGN KEY (from_warehouse_id) REFERENCES warehouses(warehouse_id),
    FOREIGN KEY (to_warehouse_id) REFERENCES warehouses(warehouse_id),
    FOREIGN KEY (product_id) REFERENCES products(product_id),
    FOREIGN KEY (created_by) REFERENCES users(user_id),
    FOREIGN KEY (updated_by) REFERENCES users(user_id)
);

-- 建立庫存警報表
CREATE TABLE inventory_alerts (
    alert_id INT PRIMARY KEY AUTO_INCREMENT,
    warehouse_id INT NOT NULL,
    product_id INT NOT NULL,
    alert_type ENUM('low_stock', 'over_stock') NOT NULL,
    quantity INT NOT NULL,
    threshold INT NOT NULL,
    status ENUM('active', 'acknowledged', 'resolved') NOT NULL DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    acknowledged_at TIMESTAMP NULL,
    acknowledged_by INT NULL,
    FOREIGN KEY (warehouse_id) REFERENCES warehouses(warehouse_id),
    FOREIGN KEY (product_id) REFERENCES products(product_id),
    FOREIGN KEY (acknowledged_by) REFERENCES users(user_id)
);

-- 觸發器：當有庫存變更時自動更新庫存數量
DELIMITER //
CREATE TRIGGER before_movement_insert
BEFORE INSERT ON inventory_movements
FOR EACH ROW
BEGIN
    DECLARE current_quantity INT;
    
    -- 獲取當前庫存
    SELECT quantity INTO current_quantity
    FROM inventory
    WHERE warehouse_id = NEW.warehouse_id AND product_id = NEW.product_id;
    
    -- 設置變更前後數量
    SET NEW.before_quantity = current_quantity;
    
    IF NEW.type IN ('purchase', 'transfer_in', 'return_in') THEN
        SET NEW.after_quantity = current_quantity + NEW.quantity;
    ELSE
        SET NEW.after_quantity = current_quantity - NEW.quantity;
        -- 檢查庫存是否足夠
        IF NEW.after_quantity < 0 THEN
            SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Insufficient inventory';
        END IF;
    END IF;
END //

CREATE TRIGGER after_movement_insert
AFTER INSERT ON inventory_movements
FOR EACH ROW
BEGIN
    -- 更新庫存數量
    UPDATE inventory 
    SET quantity = NEW.after_quantity
    WHERE warehouse_id = NEW.warehouse_id AND product_id = NEW.product_id;
    
    -- 檢查是否需要產生警報
    INSERT INTO inventory_alerts (warehouse_id, product_id, alert_type, quantity, threshold)
    SELECT 
        NEW.warehouse_id,
        NEW.product_id,
        CASE 
            WHEN NEW.after_quantity <= min_quantity THEN 'low_stock'
            WHEN max_quantity IS NOT NULL AND NEW.after_quantity >= max_quantity THEN 'over_stock'
        END,
        NEW.after_quantity,
        CASE 
            WHEN NEW.after_quantity <= min_quantity THEN min_quantity
            ELSE max_quantity
        END
    FROM inventory
    WHERE warehouse_id = NEW.warehouse_id 
    AND product_id = NEW.product_id
    AND (
        NEW.after_quantity <= min_quantity
        OR (max_quantity IS NOT NULL AND NEW.after_quantity >= max_quantity)
    );
END //
DELIMITER ;