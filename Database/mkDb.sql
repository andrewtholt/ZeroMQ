CREATE DATABASE IF NOT EXISTS zmq_db;

USE zmq_db;

CREATE TABLE IF NOT EXISTS input (
    id INT AUTO_INCREMENT PRIMARY KEY,
    url VARCHAR(255),
    start_value BOOLEAN,
    stop_value BOOLEAN,
    status VARCHAR(50)
);

CREATE TABLE IF NOT EXISTS output (
    id INT AUTO_INCREMENT PRIMARY KEY,
    url VARCHAR(255),
    motor_value BOOLEAN,
    status VARCHAR(50)
);
