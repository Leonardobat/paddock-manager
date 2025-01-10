CREATE TABLE IF NOT EXISTS cars 
    (
        id INTEGER PRIMARY KEY, 
        scuderia TEXT,
        max_speed FLOAT,
        acceleration UNSIGNED SMALLINT,
        downforce UNSIGNED SMALLINT
    );

CREATE TABLE IF NOT EXISTS circuit 
    (
        id INTEGER PRIMARY KEY,
        name TEXT,
        sectors BLOB,
        total_laps UNSIGNED SMALLINT,
        pitstop_duration  UNSIGNED SMALLINT
    );