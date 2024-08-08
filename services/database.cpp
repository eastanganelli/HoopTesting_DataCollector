#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include "database.h"
#include "../utils/simplecrypt.h"

Manager::Manager() {

}

Manager::~Manager() {

}

void Manager::initialize() {
    // this->myRemoteDB.initialize();
    this->myCacheDB.initialize();
}

void Manager::open() {
    bool remoteIsOpen = this->myRemoteDB.open(),
         cacheIsOpen  = this->myCacheDB.open();
    if(/*!remoteIsOpen ||*/ !cacheIsOpen) {
        throw ManagerErrors::ConnectionError("No se pudo conectar a la base de datos!");
    }
    this->isStationActive(2);
}

void Manager::close() {
    // this->myRemoteDB.close();
    this->myCacheDB.close();
}

uint Manager::isStationActive(const uint station_ID) {
    QSqlQuery isStationOccupy(this->myCacheDB.get());
    isStationOccupy.prepare("SELECT s.testID AS TestID FROM station s WHERE s.id = :id_station;");
    isStationOccupy.bindValue(":id_station", station_ID);
    isStationOccupy.exec();
    isStationOccupy.next();
    const uint result = isStationOccupy.value("TestID").toUInt();
    if(result == 0) {
        throw ManagerErrors::QuerySelectError("Estación No ocupada!");
    }
    return result;
}

void Manager::loadConfiguration(QSqlDatabase &myDB) {
    SimpleCrypt myDecrypt;
    myDecrypt.setKey(Q_UINT64_C(0x3453049));
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("DBConfig");
    const QString hostName = mySettings.value("hostname", QString()).toString(),
        userName = mySettings.value("username", QString()).toString(),
        password = myDecrypt.decryptToString(mySettings.value("password", QString()).toString());
    const uint port = mySettings.value("port", QString()).toUInt();
    mySettings.endGroup();

    if(hostName == "" || userName == "" || password == "" || port == 0) {
        throw new ManagerErrors::ConfigurationError("Base de datos no configurada!");
    }
    myDB.setHostName(hostName);
    myDB.setPort(port);
    myDB.setUserName(userName);
    myDB.setPassword(password);
}

bool Manager::test(const QString hostname, const uint port, const QString username, const QString password) {
    QSqlDatabase dbTest;
    {
        dbTest.setHostName(hostname);
        dbTest.setPort(port);
        dbTest.setUserName(username);
        dbTest.setPassword(password);
    }
    dbTest.open();
    bool state = dbTest.isOpen();
    if(state) { dbTest.close(); }
    return state;
}

bool Manager::test(QSqlDatabase testDB) {
    bool state = testDB.open();
    if(state) { testDB.close(); }
    return state;
}

void Manager::save(const QSqlDatabase myDB) {
    SimpleCrypt myEncrypt;
    myEncrypt.setKey(Q_UINT64_C(0x3453049));
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("DBConfig");
    mySettings.setValue("hostname", myDB.hostName());
    mySettings.setValue("port",     QString::number(myDB.port()));
    mySettings.setValue("username", myDB.userName());
    mySettings.setValue("password", myEncrypt.encryptToString(myDB.password()));
    mySettings.endGroup();
}

void Manager::save(const QString hostname, const uint port, const QString username, const QString password) {
    SimpleCrypt myEncrypt;
    myEncrypt.setKey(Q_UINT64_C(0x3453049));
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("DBConfig");
    mySettings.setValue("hostname", hostname);
    mySettings.setValue("port",     QString::number(port));
    mySettings.setValue("username", username);
    mySettings.setValue("password", myEncrypt.encryptToString(password));
    mySettings.endGroup();
}

void Manager::RemoteDB::initialize() {
    SimpleCrypt myDecrypt;
    myDecrypt.setKey(Q_UINT64_C(0x3453049));
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("DBConfig");
    const QString hostName = mySettings.value("hostname", QString()).toString(),
        userName = mySettings.value("username", QString()).toString(),
        password = myDecrypt.decryptToString(mySettings.value("password", QString()).toString());
    const uint port = mySettings.value("port", QString()).toUInt();
    mySettings.endGroup();

    if(hostName == "" || userName == "" || password == "" || port == 0) {
        throw new ManagerErrors::ConfigurationError("Base de datos no configurada!");
    }
    this->a_remoteDB.setHostName(hostName);
    this->a_remoteDB.setPort(port);
    this->a_remoteDB.setUserName(userName);
    this->a_remoteDB.setPassword(password);
}

bool Manager::RemoteDB::open() { return this->a_remoteDB.open(); }

void Manager::RemoteDB::close() { this->a_remoteDB.close(); }

Manager::CacheDB::CacheDB() {
    this->a_cacheDB = QSqlDatabase::addDatabase("QSQLITE", "cachedDatabase");
}

Manager::CacheDB::~CacheDB() {

}

void Manager::CacheDB::initialize() {
    QDir databaseDir = QDir(QApplication::applicationDirPath());
    this->a_cacheDB.setDatabaseName(QString(databaseDir.absolutePath() + "/cacheDatabase.db"));
}

bool Manager::CacheDB::open()  { return this->a_cacheDB.open(); }

void Manager::CacheDB::close() { this->a_cacheDB.close(); }

QSqlDatabase Manager::CacheDB::get() { return this->a_cacheDB; }
