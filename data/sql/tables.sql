CREATE TABLE IF NOT EXISTS cars 
    (
        id INTEGER PRIMARY KEY, 
        scuderia TEXT,
        max_speed FLOAT,
        acceleration UNSIGNED SMALLINT,
        downforce UNSIGNED SMALLINT
    );