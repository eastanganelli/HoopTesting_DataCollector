#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include "database.h"
#include "../utils/simplecrypt.h"
#include "../components/datavisualizer.h"

class DataVisualizerWindow;

Manager::Manager() {}

Manager::~Manager() {}

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
}

void Manager::close() {
    // this->myRemoteDB.close();
    this->myCacheDB.close();
}

uint Manager::isStationActive(const uint station_ID) {
    QSqlQuery stationOccupy(this->myCacheDB.get());
    stationOccupy.prepare("SELECT s.testID AS TestID FROM station s WHERE s.id = :id_station;");
    stationOccupy.bindValue(":id_station", station_ID);
    stationOccupy.exec();
    stationOccupy.next();
    const uint result = stationOccupy.value("TestID").toUInt();
    if(result == 0) {
        throw ManagerErrors::QuerySelectError("Estación No ocupada!");
    }
    return result;
}

void Manager::insertData(const uint testID, const double pressure, const double temperature, const double ambient) { this->myCacheDB.insertData(testID, pressure, temperature, ambient); }

void Manager::unlinkStationTest(const uint station_id) { this->myCacheDB.StopStation(station_id); }

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

Manager::CacheDB::CacheDB() { this->a_cacheDB = QSqlDatabase::addDatabase("QSQLITE", "cachedDatabase"); }

Manager::CacheDB::~CacheDB() { }

void Manager::CacheDB::initialize() {
    QDir databaseDir = QDir(QApplication::applicationDirPath());
    this->a_cacheDB.setDatabaseName(QString(databaseDir.absolutePath() + "/cacheDatabase.db"));
}

bool Manager::CacheDB::open()  { return this->a_cacheDB.open(); }

void Manager::CacheDB::close() { this->a_cacheDB.close(); }

QSqlDatabase Manager::CacheDB::get() { return this->a_cacheDB; }

void Manager::CacheDB::insertData(const uint ID_Station, const float pressure, const float temperature, const float ambient) {
    uint TestID = [](const uint Station_ID, QSqlDatabase& db) -> uint {
        QSharedPointer<Station> myStation = DataVisualizerWindow::myStations[Station_ID];
        uint result = myStation->getTestID();
        if(result == 0) {
            QSqlQuery stationQuery(db);
            stationQuery.prepare("SELECT s.testID AS TestID FROM station s WHERE s.id = :id_station;");
            stationQuery.bindValue(":id_station", Station_ID);
            stationQuery.exec();
            stationQuery.next();

            result = stationQuery.value("TestID").toUInt();
            if(result == 0) {
                stationQuery.prepare("INSERT INTO test (createdAt) VALUES (CURRENT_TIMESTAMP);");
                stationQuery.exec();
                stationQuery.prepare("SELECT MAX(t.id) AS TestID FROM test t;");
                stationQuery.exec();
                stationQuery.next();
                result = stationQuery.value("TestID").toUInt();
                myStation->setTestID(result);
                stationQuery.prepare("UPDATE station SET testID = :testID WHERE id = :id_station;");
                stationQuery.bindValue(":testID",     result);
                stationQuery.bindValue(":id_station", Station_ID);
                stationQuery.exec();
            }
        }
        return result;
    }(ID_Station, this->a_cacheDB);

    QSqlQuery insertData(this->a_cacheDB);
    insertData.prepare("INSERT INTO data (testID, pressure, temperature, ambient) VALUES (:testID, :pressure, :temperature, :ambient);");
    insertData.bindValue(":testID",      TestID);
    insertData.bindValue(":pressure",    pressure);
    insertData.bindValue(":temperature", temperature);
    insertData.bindValue(":ambient",     ambient);
    insertData.exec();
}

void Manager::CacheDB::updateTest(const uint testID, const QString &standard, const QString &material, const QString &specification, const uint lenTotal, const uint lenFree, const uint diamNom, const uint diamReal, const uint thickness, const QString &testType, const QString &operatorName, const QString &endCap) {
    QSqlQuery updateTest(this->a_cacheDB);
    updateTest.prepare("UPDATE test SET standard = :standard, material = :material, specification = :specification, lenTotal = :lenTotal, lenFree = :lenFree, diamNom = :diamNom, diamReal = :diamReal, thickness = :thickness, testType = :testType, operator = :operator, endCap = :endCap WHERE id = :testID;");
    updateTest.bindValue(":testID",        testID);
    updateTest.bindValue(":standard",      standard);
    updateTest.bindValue(":material",      material);
    updateTest.bindValue(":specification", specification);
    updateTest.bindValue(":lenTotal",      lenTotal);
    updateTest.bindValue(":lenFree",       lenFree);
    updateTest.bindValue(":diamNom",       diamNom);
    updateTest.bindValue(":diamReal",      diamReal);
    updateTest.bindValue(":thickness",     thickness);
    updateTest.bindValue(":testType",      testType);
    updateTest.bindValue(":operator",      operatorName);
    updateTest.bindValue(":endCap",        endCap);
    updateTest.exec();
}

void Manager::CacheDB::StopStation(const uint ID_Station) {
    QSqlQuery stationQuery(this->a_cacheDB);
    stationQuery.prepare("UPDATE station SET testID = NULL WHERE id = :id_station;");
    stationQuery.bindValue(":id_station", ID_Station);
    stationQuery.exec();
}

void Manager::CacheDB::StopByStandby(const uint ID_Station) {
    QDateTime now = QDateTime::currentDateTime();
    QSqlQuery stationQuery(this->a_cacheDB);
    stationQuery.prepare("SELECT MAX(d.createdAt) AS LastInsertion FROM data d INNER JOIN station s ON s.testID = d.testID AND s.id = :id_station;");
    stationQuery.bindValue(":id_station", ID_Station);
    stationQuery.exec();
    stationQuery.next();
    QDateTime lastInsertion = stationQuery.value("LastInsertion").toDateTime();
    if(lastInsertion.secsTo(now) > 180) {
        stationQuery.prepare("UPDATE station SET testID = 0 WHERE id = :id_station;");
        stationQuery.bindValue(":id_station", ID_Station);
        stationQuery.exec();
    }
}
