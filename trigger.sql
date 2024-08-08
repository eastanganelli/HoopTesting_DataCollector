-- INSERT INTO test (id, createdAt, updatedAt) VALUES (2, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP);
-- UPDATE station SET testID = 2, updatedAt = CURRENT_TIMESTAMP WHERE id = 2;

-- SELECT s.testID AS TestID FROM station s WHERE s.id=1;

CREATE TRIGGER newTest BEFORE INSERT ON data
BEGIN
	SET countTest = SELECT s.testID FROM station s WHERE s.id = new.station_id);
	 IF(countTest < 1) THEN
		INSERT INTO test (createdAt) VALUES (CURRENT_TIMESTAMP);
		SET myNewTest := SELECT id FROM test ORDER by id;
		UPDATE station SET testID = myNewTest WHERE station.id = new.station_id;
	 END IF;
END;
