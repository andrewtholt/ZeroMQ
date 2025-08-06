USE zmq_db;

-- Insert data from the "INPUT" section of the JSON
INSERT INTO input (url, start_value, stop_value, status)
VALUES ('http://127.0.0.1:1234', false, false, 'UNKNOWN');

-- Insert data from the "OUTPUT" section of the JSON
INSERT INTO output (url, motor_value, status)
VALUES ('http://127.0.0.1:1235', false, 'UNKNOWN');
