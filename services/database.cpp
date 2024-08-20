#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include "database.h"
#include "../utils/simplecrypt.h"
#include "../components/generalsettings.h"

QSharedPointer<Manager> Manager::myDatabases = QSharedPointer<Manager>(nullptr);

Manager::Manager() {
    this->timeoutTest = 2;
    generalSettings::loadSettingsStation(this->timeoutTest);
}

Manager::RemoteDB::RemoteDB() {
    this->a_StaticDatabase = QSqlDatabase::addDatabase("QMYSQL", "RemoteStatic");
    this->a_DataDatabase   = QSqlDatabase::addDatabase("QMYSQL", "RemoteData");
    this->a_StaticDatabase.setDatabaseName("static_db");
    this->a_DataDatabase.setDatabaseName("data_db");
}

Manager::CacheDB::CacheDB() { this->a_cacheDB = QSqlDatabase::addDatabase("QSQLITE", "cachedDatabase"); }

Manager::~Manager() {}

Manager::RemoteDB::~RemoteDB() {
    this->a_DataDatabase.close();
    this->a_StaticDatabase.close();
}

Manager::CacheDB::~CacheDB() { this->a_cacheDB.close(); }

void Manager::initialize() {
    try {
        this->a_RemoteDB.initialize();
        this->a_CacheDB.initialize();
    }
    catch(ManagerErrors::ConfigurationError& e) { e.raise(); }
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
        throw ManagerErrors::ConfigurationError("Base de datos no configurada!");
    }{
        this->a_StaticDatabase.setHostName(hostName);
        this->a_StaticDatabase.setPort(port);
        this->a_StaticDatabase.setUserName(userName);
        this->a_StaticDatabase.setPassword(password);
    }{
        this->a_DataDatabase.setHostName(hostName);
        this->a_DataDatabase.setPort(port);
        this->a_DataDatabase.setUserName(userName);
        this->a_DataDatabase.setPassword(password);
    }
}

void Manager::CacheDB::initialize() {
    QDir databaseDir = QDir(QApplication::applicationDirPath());
    this->a_cacheDB.setDatabaseName(QString(databaseDir.absolutePath() + "/cacheDatabase.db"));
}

void Manager::open() {
    try {
        this->a_RemoteDB.open();
        this->a_CacheDB.open();
    } catch(ManagerErrors::ConnectionError& e) {
        e.raise();
        return;
    }
    emit this->DatabaseConnection(Manager::Status::OPEN, "");
}

void Manager::RemoteDB::open() {
    this->a_StaticDatabase.open();
    this->a_DataDatabase.open();
    if(!this->a_StaticDatabase.isOpen() || !this->a_DataDatabase.isOpen()) {
        throw ManagerErrors::ConnectionError("No se pudo conectar a la base de datos remota!");
    }
}

void Manager::CacheDB::open()  {
    this->a_cacheDB.open();
    if(!this->a_cacheDB.isOpen()) {
        throw ManagerErrors::ConnectionError("No se pudo conectar a la base de datos local!");
    }
}

bool Manager::isOpen() { return this->a_CacheDB.isOpen() && this->a_RemoteDB.isOpen(); }

bool Manager::RemoteDB::isOpen() { return this->a_StaticDatabase.isOpen() && this->a_DataDatabase.isOpen(); }

bool Manager::CacheDB::isOpen() { return this->a_cacheDB.isOpen(); }

void Manager::RemoteDB::close() {
    this->a_StaticDatabase.close();
    this->a_DataDatabase.close();
}

QSqlDatabase Manager::RemoteDB::get(const RemoteSelect &selection) {
    if(selection == RemoteSelect::STATIC)
        return this->a_StaticDatabase;
    return this->a_DataDatabase;
}

void Manager::RemoteDB::insertTest(const uint testID, const QString &standard, const QString &material, const QString &specification, const uint lenTotal, const uint lenFree, const uint diamNom, const uint diamReal, const uint thickness, const QString &testType, const QString &operatorName, const QString &endCap, const QString &enviroment, const QString &conditionalPeriod, const uint &pressureTarget, const uint &temperatureTarget, const QString& createdAt, const QString& description) {
    QSqlQuery insertSample(this->a_DataDatabase),
              insertSpecimen(this->a_DataDatabase);
    uint idSample = [&insertSample](const QString &standard, const QString &material, const QString &specification, const uint lenTotal, const uint lenFree, const uint diamNom, const uint diamReal, const uint thickness, const QString &conditionalPeriod) -> uint {
        uint a_id = 0;
        if(!(standard.isNull() || material.isNull() || specification.isNull() || lenTotal == 0 || lenFree == 0 || diamNom == 0 || diamReal == 0 || thickness == 0 || conditionalPeriod.isNull())) {
            {
                insertSample.prepare("INSERT INTO sample (standard, material, specification, diamreal, diamnom, wallthick, lentotal, lenfree, condPeriod) VALUES (:standard, :material, :specification, :diamreal, :diamnom, :wallthick, :lentotal, :lenfree, :condPeriod);");
                insertSample.bindValue(":standard", standard);
                insertSample.bindValue(":material", material);
                insertSample.bindValue(":specification", specification);
                insertSample.bindValue(":diamnom", diamNom);
                insertSample.bindValue(":diamreal", diamReal);
                insertSample.bindValue(":wallthick", thickness);
                insertSample.bindValue(":lentotal", lenTotal);
                insertSample.bindValue(":lenfree", lenFree);
                insertSample.bindValue(":condPeriod", conditionalPeriod);
                insertSample.exec();
            }{
                insertSample.prepare("SELECT s.id FROM sample s WHERE s.standard like :standard AND s.material LIKE :material AND s.specification LIKE :specification AND s.diamnom = :diamnom AND s.diamreal = :diamreal AND s.wallthick = :wallthick AND s.lenfree = :lenfree AND s.lentotal = :lentotal");
                insertSample.bindValue(":standard", standard);
                insertSample.bindValue(":material", material);
                insertSample.bindValue(":specification", specification);
                insertSample.bindValue(":diamnom", diamNom);
                insertSample.bindValue(":diamreal", diamReal);
                insertSample.bindValue(":wallthick", thickness);
                insertSample.bindValue(":lentotal", lenTotal);
                insertSample.bindValue(":lenfree", lenFree);
                insertSample.exec();
                insertSample.next();
                a_id = insertSample.value("id").toUInt();
            }
        }
        return a_id;
    }(standard, material,specification, lenTotal, lenFree, diamNom, diamReal, thickness, conditionalPeriod);

    void* _ = [&insertSpecimen](const uint& idSample, const uint& testID, const QString &testType, const QString &operatorName, const QString &endCap, const QString &enviroment, const uint& pressureTarget, const uint& temperatureTarget, const QString& createdAt, const QString& description) -> void* {
        if(idSample != 0) {
            QString script("INSERT INTO specimen (id, sample, targetPressure, targetTemperature, operator, enviroment, testName, endCap, failText, createdAt, updatedAt) \
                                    VALUES (%1, %2, %3, %4, '%5', '%6', '%7', '%8', %9, STR_TO_DATE('%10','%Y-%m-%d %H:%i:%s.%f'), DEFAULT);");
            insertSpecimen.exec(script.arg(testID).arg(idSample).arg(pressureTarget).arg(temperatureTarget).arg(operatorName).arg(enviroment).arg(testType).arg(endCap).arg((description.isEmpty() ? "DEFAULT": "'" + description + "'")).arg(createdAt));
        } else {
            QString script = "INSERT INTO specimen (ID, sample, targetPressure, targetTemperature, operator, enviroment, testName, endCap, failText, remark, createdAt, updatedAt) \
                               VALUES (%1, NULL, NULL, NULL, DEFAULT, DEFAULT, DEFAULT, DEFAULT, '%2', DEFAULT, STR_TO_DATE('%3','%Y-%m-%d %H:%i:%s.%f'), DEFAULT);";
            insertSpecimen.exec(script.arg(testID).arg((description.isEmpty() ? "DEFAULT": "'" + description + "'")).arg(createdAt));
        }
        return nullptr;
    }(idSample, testID, testType, operatorName, endCap, enviroment, pressureTarget, temperatureTarget, createdAt, description);
}

void Manager::RemoteDB::insertData(const uint& testID, const double& pressure, const double& temperature, const double& ambient, const QString& createdAt) {
    QSqlQuery insertData(this->a_DataDatabase);
    QString script("INSERT INTO data (specimen, pressure, temperature, ambient, createdAt) VALUES (%1, %2, %3, %4, STR_TO_DATE('%5','%Y-%m-%d %H:%i:%s.%f'));");
    insertData.exec(script.arg(testID).arg(pressure).arg(temperature).arg(ambient).arg(createdAt));
}

void Manager::CacheDB::close() { this->a_cacheDB.close(); }

void Manager::close() {
    this->a_RemoteDB.close();
    this->a_CacheDB.close();
    emit this->DatabaseConnection(Manager::Status::CLOSE, "");
}

uint Manager::stationIsActive(const uint station_ID) {
    QSqlQuery stationOccupy(this->a_CacheDB.get());
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

void Manager::stationsFreedom() {
    try {
        QSqlQuery stationOccupy(this->a_CacheDB.get()), stationIsFree(this->a_CacheDB.get());
        stationOccupy.prepare("SELECT \
                                 ass.Station AS StationID, \
                                 ass.testID AS TestID \
                            FROM ActiveStations ass;");
        stationOccupy.exec();
        while(stationOccupy.next()) {
            uint activeTestID = stationOccupy.value("TestID").toUInt();
            QDateTime now = QDateTime::currentDateTime();
            stationIsFree.prepare("SELECT \
                                    MAX(d.createdAt) AS lastDate \
                                  FROM data d \
                                  WHERE d.testID = :idStation;");
            stationIsFree.bindValue(":idStation", activeTestID);
            stationIsFree.exec();
            stationIsFree.next();
            QDateTime lastDate = QDateTime::fromString(stationIsFree.value("lastDate").toString(), "yyyy-MM-dd HH:mm:ss");
            qDebug() << lastDate.secsTo(now);
            if(lastDate.secsTo(now) > (this->timeoutTest * 3600)) { this->exportTestData(activeTestID); }
        }
    } catch(...) {}
}

QSqlQuery Manager::selectTest(const QString &myQuery, const QString& dbName) {
    QSqlQuery testQuery(QSqlDatabase::database(dbName));
    testQuery.prepare(myQuery);
    testQuery.exec();
    if(!testQuery.next()) {
        throw ManagerErrors::QuerySelectError("No se encontraron resultados!");
    }
    return testQuery;
}

void Manager::insertData(const uint& ID_Station, const double& pressure, const double& temperature, const double& ambient) { this->a_CacheDB.insertData(ID_Station, pressure, temperature, ambient); }

void Manager::deleteTest(const uint testID) { this->a_CacheDB.StopStation(testID); }

void Manager::unlinkStationTest(const uint station_id) { this->a_CacheDB.StopStation(station_id); }

void Manager::exportTestData(const uint& testID) {
    QSqlQuery cacheQuery(this->a_CacheDB.get()),
              remoteQuery(this->a_RemoteDB.get(RemoteDB::RemoteSelect::DATA));
    RemoteDB remoteDatabase = this->a_RemoteDB;
    void* _ = [&remoteDatabase, &cacheQuery, testID]() -> void* {
        cacheQuery.prepare("SELECT * FROM test t WHERE t.id = :testID;");
        cacheQuery.bindValue(":testID", testID);
        cacheQuery.exec();
        cacheQuery.next();
        const QString standard = cacheQuery.value("standard").toString(), material = cacheQuery.value("material").toString(), specification = cacheQuery.value("specification").toString(),
                      testType = cacheQuery.value("testType").toString(), operatorName = cacheQuery.value("operator").toString(), endCap = cacheQuery.value("endCap").toString(),
                      enviroment = cacheQuery.value("enviroment").toString(), conditionalPeriod = cacheQuery.value("conditionalPeriod").toString(),
                      createdAt = cacheQuery.value("createdAt").toString(), description = cacheQuery.value("description").toString();
        const uint lenTotal = cacheQuery.value("lenTotal").toUInt(), lenFree = cacheQuery.value("lenFree").toUInt(),
                   diamNom = cacheQuery.value("diameterNormal").toUInt(),   diamReal = cacheQuery.value("diameterReal").toUInt(), thickness = cacheQuery.value("wallthickness").toUInt(),
                   pressureTarget = cacheQuery.value("pressureTarget").toUInt(), temperatureTarget = cacheQuery.value("temperatureTarget").toUInt();
        remoteDatabase.insertTest(testID, standard, material, specification, lenTotal, lenFree, diamNom, diamReal, thickness, testType, operatorName, endCap, enviroment, conditionalPeriod, pressureTarget, temperatureTarget, createdAt, description);
        return nullptr;
    }();
    _ = [&cacheQuery, &remoteDatabase, testID]() -> void* {
        cacheQuery.prepare("SELECT d.pressure, d.temperature, d.ambient, d.createdat FROM data d WHERE testID = :testID;");
        cacheQuery.bindValue(":testID", testID);
        cacheQuery.exec();
        while(cacheQuery.next()) { remoteDatabase.insertData(testID, cacheQuery.value("pressure").toDouble(), cacheQuery.value("temperature").toDouble(), cacheQuery.value("ambient").toDouble(), cacheQuery.value("createdAt").toString()); }
        return nullptr;
    }();
    auto exportstatus = [&cacheQuery, &remoteQuery, testID]() -> bool {
        cacheQuery.prepare("SELECT COUNT(*) FROM data d WHERE d.testID = :testID;");
        cacheQuery.bindValue(":testID", testID);
        cacheQuery.exec();
        cacheQuery.next();
        remoteQuery.prepare("SELECT COUNT(*) FROM data d WHERE d.specimen = :testID;");
        remoteQuery.bindValue(":testID", testID);
        remoteQuery.exec();
        remoteQuery.next();
        return cacheQuery.value(0).toUInt() == remoteQuery.value(0).toUInt();
    };
    if(exportstatus()) { this->a_CacheDB.deleteTest(testID); }
}

void Manager::testTimeoutTime(const uint &timeoutTime) { this->timeoutTest = timeoutTime; qDebug() << "Update timeout Test: " << timeoutTime; }

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

void Manager::updateTest(const uint testID, const QString& standard, const QString& material, const QString& specification, const uint lenTotal, const uint lenFree, const uint diamNom, const uint diamReal, const uint thickness, const QString& testType, const QString& operatorName, const QString& endCap, const QString& enviroment, const QString& conditionalPeriod, const uint& pressureTarget, const uint& temperatureTarget) {
    this->a_CacheDB.updateTest(testID, standard, material, specification, lenTotal, lenFree, diamNom, diamReal, thickness, testType, operatorName, endCap, enviroment, conditionalPeriod, pressureTarget, temperatureTarget);
}

void Manager::failureTest(const uint testID, const QString &description) { this->a_CacheDB.failureTest(testID, description); }

QSqlDatabase Manager::CacheDB::get() { return this->a_cacheDB; }

void Manager::CacheDB::insertData(const uint& ID_Station, const double& pressure, const double& temperature, const double& ambient) {
    uint TestID = [](const uint Station_ID, QSqlDatabase& db) -> uint {
        QSharedPointer<Station> myStation = Station::myStations[Station_ID];
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
                stationQuery.prepare("UPDATE station SET testID = :testID WHERE id = :id_station;");
                stationQuery.bindValue(":testID",     result);
                stationQuery.bindValue(":id_station", Station_ID);
                stationQuery.exec();
            }
            myStation->setTestID(result);
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

void Manager::CacheDB::updateTest(const uint& testID, const QString& standard, const QString& material, const QString& specification, const uint lenTotal, const uint lenFree, const uint diamNom, const uint diamReal, const uint thickness, const QString& testType, const QString& operatorName, const QString& endCap, const QString& enviroment, const QString& conditionalPeriod, const uint& pressureTarget, const uint& temperatureTarget) {
    QSqlQuery updateQuery(this->a_cacheDB);
    updateQuery.prepare("UPDATE test SET standard = :standard, material = :material, specification = :specification, lenTotal = :lenTotal, lenFree = :lenFree, diameterReal = :diameterReal, diameterNormal = :diameterNormal, wallthickness = :wallthickness, endCap = :endCap, testType = :testType, operator = :operator, enviroment = :enviroment, conditionalPeriod = :conditionalPeriod, pressureTarget = :pressureTarget, temperatureTarget = :temperatureTarget, updatedAt = CURRENT_TIMESTAMP WHERE id = :id;");
    updateQuery.bindValue(":standard",          standard);
    updateQuery.bindValue(":material",          material);
    updateQuery.bindValue(":specification",     specification);
    updateQuery.bindValue(":lenTotal",          lenTotal);
    updateQuery.bindValue(":lenFree",           lenFree);
    updateQuery.bindValue(":diameterNormal",    diamReal);
    updateQuery.bindValue(":diameterReal",      diamNom);
    updateQuery.bindValue(":wallthickness",     thickness);
    updateQuery.bindValue(":endCap",            endCap);
    updateQuery.bindValue(":testType",          testType);
    updateQuery.bindValue(":operator",          operatorName);
    updateQuery.bindValue(":enviroment",        enviroment);
    updateQuery.bindValue(":conditionalPeriod", conditionalPeriod);
    updateQuery.bindValue(":pressureTarget",    pressureTarget);
    updateQuery.bindValue(":temperatureTarget", temperatureTarget);
    updateQuery.bindValue(":id",                testID);
    updateQuery.exec();
}

void Manager::CacheDB::failureTest(const uint &ID_Station, const QString &description) {
    QSqlQuery updateQuery(this->a_cacheDB);
    QString script("UPDATE test SET description = '%1' id = %2;");
    updateQuery.exec(script.arg(description).arg(QString::number(ID_Station)));
}

void Manager::CacheDB::deleteTest(const uint &testID) {
    QSqlQuery deleteQuery(this->a_cacheDB);
    deleteQuery.prepare("DELETE FROM test WHERE id = :id;");
    deleteQuery.bindValue(":id", testID);
    deleteQuery.exec();
}

void Manager::CacheDB::StopStation(const uint& ID_Station) {
    QSqlQuery stationQuery(this->a_cacheDB);
    stationQuery.prepare("UPDATE station SET testID = NULL WHERE id = :id_station;");
    stationQuery.bindValue(":id_station", ID_Station);
    stationQuery.exec();
}

void Manager::CacheDB::StopByStandby(const uint& ID_Station) {
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
