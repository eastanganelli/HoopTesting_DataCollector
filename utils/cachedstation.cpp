#include "cachedstation.h"

void cachedStation::initialize() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("cache_test.db");

    if (!db.open()) {
        qDebug() << "Error: connection with database fail";
    } else {
        qDebug() << "Database: connection ok";
    }
}

cachedStation::cachedStation() {
    this->myCachedDB = QSqlDatabase::addDatabase("QSQLITE");
    this->myCachedDB.setDatabaseName(QDir::currentPath() + "/cache_test.db");
}

bool cachedStation::open()  { return this->open(); }

bool cachedStation::close() { return this->close(); }
