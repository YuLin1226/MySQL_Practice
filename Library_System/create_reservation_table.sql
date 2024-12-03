-- Active: 1733128601746@@127.0.0.1@3306@My_First_DB
CREATE TABLE book_reservations (
    reservation_id INT PRIMARY KEY AUTO_INCREMENT,
    book_id INT,
    user_id INT,
    reservation_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    status ENUM('waiting', 'available', 'reserved', 'borrowed', 'cancelled') DEFAULT 'waiting',
    notification_date TIMESTAMP NULL,
    FOREIGN KEY (book_id) REFERENCES books(book_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id)
);


