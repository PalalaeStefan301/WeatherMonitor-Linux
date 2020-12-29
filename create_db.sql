
CREATE TABLE IF NOT EXISTS regions (
	region_id INTEGER PRIMARY KEY,
	region_name TEXT NOT NULL	
);

CREATE TABLE IF NOT EXISTS cities (
	city_id INTEGER PRIMARY KEY,
	city_name TEXT NOT NULL,
	region_id TEXT NOT NULL,
    temperature INTEGER NOT NULL,
    humidity INTEGER NOT NULL,
    uv_index INTEGER NOT NULL,
    sunrise TEXT NOT NULL,
    sunset TEXT NOT NULL,
    wind INTEGER NOT NULL,
    FOREIGN KEY(region_id)
        REFERENCES cities(region_id)
            ON DELETE CASCADE
            ON UPDATE NO ACTION
);

DELETE FROM regions;
DELETE FROM cities;

INSERT INTO regions(region_name) VALUES("California");
INSERT INTO regions(region_name) VALUES("Florida");
INSERT INTO regions(region_name) VALUES("Pennsylvania");
INSERT INTO regions(region_name) VALUES("Illinois");
INSERT INTO regions(region_name) VALUES("Texas");

INSERT INTO cities(city_name,region_id,temperature,humidity,uv_index,sunrise,sunset,wind)
VALUES ("Los Angeles",1,63,47,3,"6:58 AM","4:52 PM", 10);
INSERT INTO cities(city_name,region_id,temperature,humidity,uv_index,sunrise,sunset,wind)
VALUES ("San Francisco",1,56,63,1,"7:25 AM","4:43 PM", 8);


