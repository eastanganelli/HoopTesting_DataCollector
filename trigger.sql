-- INSERT INTO test (id, createdAt, updatedAt) VALUES (2, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP);
-- UPDATE station SET testID = 2, updatedAt = CURRENT_TIMESTAMP WHERE id = 2;

-- SELECT s.testID AS TestID FROM station s WHERE s.id=1;

CREATE TRIGGER insertNewTest BEFORE INSERT ON data
WHEN NOT EXISTS(SELECT s.testID FROM station s WHERE s.id = NEW.station_id)
BEGIN
	INSERT INTO test (createdAt) VALUES (CURRENT_TIMESTAMP);
	UPDATE station SET testID = (SELECT id FROM test ORDER by id) WHERE station.id = new.station_id
END