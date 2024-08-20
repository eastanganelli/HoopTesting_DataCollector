CREATE TABLE `test` (
	`endCap` TEXT,
	`id` integer primary key NOT NULL UNIQUE,
	`standard` TEXT,
	`material` TEXT,
	`specification` TEXT,
	`lenTotal` INTEGER,
	`lenFree` INTEGER,
	`diameterReal` INTEGER,
	`diameterNormal` INTEGER,
	`wallthickness` INTEGER,
	`testType` TEXT,
	`operator` TEXT,
	`createdAt` REAL NOT NULL,
	`updatedAt` REAL NOT NULL
)

CREATE TABLE "data" (
	"id"	INTEGER NOT NULL UNIQUE,
	"testID"	INTEGER NOT NULL,
	"pressure"	REAL NOT NULL,
	"temperature"	REAL NOT NULL,
	"ambient"	REAL NOT NULL,
	"createdAt"	REAL NOT NULL,
	PRIMARY KEY("id" AUTOINCREMENT),
	FOREIGN KEY("testID") REFERENCES "test"("id")
)

CREATE TABLE `station` (
	`id` INTEGER UNIQUE,
	`testID` INTEGER UNIQUE,
	`createdAt` REAL NOT NULL,
	`updatedAt` REAL NOT NULL,
FOREIGN KEY(`testID`) REFERENCES `test`(`id`)
)