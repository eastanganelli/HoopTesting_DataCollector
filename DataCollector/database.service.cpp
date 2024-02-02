#include "database.service.h"

Database::Database() : QSqlDatabase("QMYSQL") {
    this->setHostName("localhost");
    this->setPort(33060);
    this->setUserName("root");
    this->setPassword("root");
    this->lblConnectionStatus = nullptr;
}

Database::Database(const Database &me) : QSqlDatabase("QMYSQL") {
    this->setHostName(me.hostName());
    this->setPort(me.port());
    this->setUserName(me.userName());
    this->setPassword(me.password());
    this->lblConnectionStatus = nullptr;
}

Database::Database(const QString host = "localhost", const uint port = 3306, const QString user = "root", const QString password = "root", QLabel* dbstatus = nullptr, QAction* dbconnect = nullptr) : QSqlDatabase("QMYSQL") {
    this->setHostName(host);
    this->setPort(port);
    this->setUserName(user);
    this->setPassword(password);
    this->lblConnectionStatus = dbstatus;
    this->lblConnectDB        = dbconnect;
}

Database::Database(const QString host, const uint port, const QString user, const QString password, const QString dbName, QLabel* dbstatus = nullptr, QAction* dbconnect = nullptr) : QSqlDatabase("QMYSQL") {
    this->setHostName(host);
    this->setPort(port);
    this->setUserName(user);
    this->setPassword(password);
    this->setDatabaseName(dbName);
    this->lblConnectionStatus = dbstatus;
    this->lblConnectDB        = dbconnect;
}

void Database::openDatabase()  {
    this->myState = !this->open();
    if(!this->myState) { this->lblConnectDB->setText("Desconectar"); }
}

void Database::closeDatabase() {
    if(!this->isOpen()) { return; }
    this->close();
    this->lblConnectDB->setText("Conectar");
}

void Database::changingLblConnectionState(const QString state_, const QString color_) {
    this->lblConnectionStatus->setText("Base de Datos: " + state_);
    this->lblConnectionStatus->setStyleSheet("color:" + color_ +";");
}

bool Database::status() {
    bool state = false;
    if(this->lblConnectionStatus != nullptr) {
        state = this->isOpen();
        if(!state && this->myState) {
            #if CONSOLEDEBUGMODE == ConsoleConsoleDebugOn
                qDebug() << "Base de datos: Desconectado";
            #endif
            this->changingLblConnectionState("Desconectado", StatusRed);
            this->myState = false;
        } else if (state && !this->myState) {
            #if CONSOLEDEBUGMODE == ConsoleConsoleDebugOn
                qDebug() << "Base de datos: Conectado";
            #endif
            this->changingLblConnectionState("Conectado", StatusGreen);
            this->myState = true;
        }
    }
    return state;
}

bool Database::test(const QString hostname, const uint port, const QString username, const QString password) {
    Database dbTest(hostname, port, username, password);
    dbTest.open();
    bool state = dbTest.isOpen();
    if(state) { dbTest.close(); }
    return state;
}

bool Database::test(Database testDB) {
    testDB.open();
    bool state = testDB.isOpen();
    if(state) { testDB.close(); }
    return state;
}

void Database::save(const Database myDB) {
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

void Database::save(const QString hostname, const uint port, const QString username, const QString password) {
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

Database* Database::read() {
    SimpleCrypt myDecrypt;
    myDecrypt.setKey(Q_UINT64_C(0x3453049));
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("DBConfig");
    const QString hostName = mySettings.value("hostname", QString()).toString(),
                  userName = mySettings.value("username", QString()).toString(),
                  password = myDecrypt.decryptToString(mySettings.value("password", QString()).toString());
    const uint port = mySettings.value("port", QString()).toUInt();
    mySettings.endGroup();
    return new Database(hostName, port, userName, password);
}

FrontClases::NodeSetting::NodeSetting(const uint id_, const uint times_, const uint temperature_) : id(id_) {
    this->times       = times_;
    this->temperature = temperature_;
}

QList<QSharedPointer<FrontClases::NodeSetting>> FrontClases::NodeSetting::getList(const Database &myDB, const uint idStandard, const uint idMaterial, const uint idSpecification) {
    const QString Script = "CALL get_SettingsBySpecification(" + QString::number(idStandard) + "," + QString::number(idMaterial) + "," + QString::number(idSpecification) + ");";
    QSqlQuery mySettingsQuery = myDB.exec(Script);
    QList<QSharedPointer<FrontClases::NodeSetting>> auxList;
    while(mySettingsQuery.next()) {
        auxList.append(QSharedPointer<FrontClases::NodeSetting>(new FrontClases::NodeSetting(mySettingsQuery.value(0).toUInt(), mySettingsQuery.value(1).toUInt(), mySettingsQuery.value(2).toUInt())));
    } return auxList;
}

uint FrontClases::NodeSetting::getID() const { return this->id; }

uint FrontClases::NodeSetting::getTimes() const { return this->times; }

uint FrontClases::NodeSetting::getTemperature() const { return this->temperature; }

FrontClases::NodeSpecification::NodeSpecification(const uint id_, const QString specification_) : id(id_) {
    this->specification = specification_;
}

QList<QSharedPointer<FrontClases::NodeSpecification>> FrontClases::NodeSpecification::getList(const Database &myDB, const uint idStandard, const uint idMaterial) {
    const QString Script = "CALL get_SpecificationByStandard(" + QString::number(idStandard) + "," + QString::number(idMaterial) + ");";
    QSqlQuery mySpecificationsQuery = myDB.exec(Script);
    QList<QSharedPointer<FrontClases::NodeSpecification>> auxList;
    while(mySpecificationsQuery.next()) {
        auxList.append(QSharedPointer<FrontClases::NodeSpecification>(new FrontClases::NodeSpecification(mySpecificationsQuery.value(1).toUInt(), mySpecificationsQuery.value(2).toString())));
    } return auxList;
}

uint FrontClases::NodeSpecification::getID() const { return this->id; }

QString FrontClases::NodeSpecification::getSpecification() const { return this->specification; }

FrontClases::NodeMaterial::NodeMaterial(const uint id_, const QString material_, const QString type_) : id(id_) {
    this->material = material_;
    this->type     = type_;
}

QList<QSharedPointer<FrontClases::NodeMaterial>> FrontClases::NodeMaterial::getList(const Database &myDB, const uint idStandard) {
    const QString Script = "CALL get_MaterialsByStandard(" + QString::number(idStandard) + ");";
    QSqlQuery myMaterialsQuery = myDB.exec(Script);
    QList<QSharedPointer<FrontClases::NodeMaterial>> auxList;
    while(myMaterialsQuery.next()) {
        auxList.append(QSharedPointer<FrontClases::NodeMaterial>(new FrontClases::NodeMaterial(myMaterialsQuery.value(0).toUInt(), myMaterialsQuery.value(1).toString(), myMaterialsQuery.value(2).toString())));
    } return auxList;
}

uint FrontClases::NodeMaterial::getID() const { return this->id; }

QString FrontClases::NodeMaterial::getMaterial() const { return this->material; }

QString FrontClases::NodeMaterial::getType() const { return this->type; }

FrontClases::NodeConditionalPeriod::NodeConditionalPeriod(const uint id_, const uint minwall_, const uint maxwall_, const QString condPeriod_) : id(id_) {
    this->minWall = minwall_;
    this->maxWall = maxwall_;
    this->conditionalPeriod = condPeriod_;
}

QList<QSharedPointer<FrontClases::NodeConditionalPeriod>> FrontClases::NodeConditionalPeriod::getList(const Database &myDB, const uint idStandard) {
    const QString Script = "CALL get_ConditionalPeriodsbyStandard(" + QString::number(idStandard) + ")";
    QSqlQuery myCondPeriods = myDB.exec(Script);
    QList<QSharedPointer<FrontClases::NodeConditionalPeriod>> auxList;
    while(myCondPeriods.next()) {
        auxList.append(QSharedPointer<FrontClases::NodeConditionalPeriod>(new FrontClases::NodeConditionalPeriod(myCondPeriods.value(0).toUInt(), myCondPeriods.value(1).toUInt(), myCondPeriods.value(2).toUInt(), myCondPeriods.value(3).toString())));
    } return auxList;
}

uint FrontClases::NodeConditionalPeriod::getID() const { return this->id; }

uint FrontClases::NodeConditionalPeriod::getMinWall() const { return this->minWall; }

uint FrontClases::NodeConditionalPeriod::getMaxWall() const { return this->maxWall; }

QString FrontClases::NodeConditionalPeriod::getConditionalPeriod() const { return this->conditionalPeriod; }

FrontClases::NodeStandard::NodeStandard(const uint id_, const QString standard_) : id(id_) { this->standard = standard_; }

QList<QSharedPointer<FrontClases::NodeStandard>> FrontClases::NodeStandard::getList(const Database &myDB) {
    const QString Script = "CALL get_Standards();";
    QSqlQuery myStandardsQuery = myDB.exec(Script);
    QList<QSharedPointer<FrontClases::NodeStandard>> auxList;
    while(myStandardsQuery.next()) {
        auxList.append(QSharedPointer<FrontClases::NodeStandard>(new FrontClases::NodeStandard(myStandardsQuery.value(0).toUInt(), myStandardsQuery.value(1).toString())));
    } return auxList;
}

uint FrontClases::NodeStandard::getID() const { return this->id; }

QString FrontClases::NodeStandard::getStandard() const { return this->standard; }

Data::NodeSample::NodeSample(const uint id_, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp, const QString condPeriod) : id(id_) {
    this->standard       = standard;
    this->material       = material;
    this->specification  = specification;
    this->diamNom        = diamNom;
    this->diamReal       = diamReal;
    this->wallthick      = wallthick;
    this->lenFree        = lenFree;
    this->lenTotal       = lenTotal;
    this->targetPressure = targetPressure;
    this->targetTemp     = targetTemp;
    this->condPeriod     = condPeriod;
}

Data::NodeSample::~NodeSample() { }

QSharedPointer<Data::NodeSample> Data::NodeSample::get(const Database &myDB, const uint idSample) {
    const QString Script = "CALL getSample(" + QString::number(idSample) + ");";
    QSqlQuery getSample = myDB.exec(Script);
    getSample.next();
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQl Script: " << Script;
    #endif
    const QString standard = getSample.value("standard").toString(),
                  material = getSample.value("material").toString(),
                  specification = getSample.value("specification").toString(),
                  condPeriod    = getSample.value("condPeriod").toString();
    const uint diamNom   = getSample.value("diamnom").toUInt(),
               diamReal  = getSample.value("diamreal").toUInt(),
               wallthick = getSample.value("wallthick").toUInt(),
               lenFree   = getSample.value("lenfree").toUInt(),
               lenTotal  = getSample.value("lentotal").toUInt(),
               targetPressure = getSample.value("targetpressure").toUInt(),
               targetTemp     = getSample.value("targettemp").toUInt();
    return QSharedPointer<Data::NodeSample>(new Data::NodeSample(idSample, standard, material, specification, diamNom, diamReal, wallthick, lenFree, lenTotal, targetPressure, targetTemp, condPeriod));
}

QSharedPointer<Data::NodeSample> Data::NodeSample::add(const Database &myDB, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp, const QString condPeriod) {
    const QString Script = "CALL insertSample('" + standard + "','" + material + "','" + specification + "'," + QString::number(diamNom) + "," + QString::number(diamReal) + "," + QString::number(wallthick)  + "," + QString::number(lenFree) + "," + QString::number(lenTotal) + "," + QString::number(targetPressure) + "," + QString::number(targetTemp) + ",'" + condPeriod + "');";
    QSqlQuery newSample = myDB.exec(Script);
    newSample.next();
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQl Script: " << Script;
        qDebug() << "ID Sample: "  << newSample.value("idSample").toUInt();
    #endif
    const uint id = newSample.value("idSample").toUInt();
    return QSharedPointer<Data::NodeSample>(new Data::NodeSample(id, standard, material, specification, diamNom, diamReal, wallthick, lenFree, lenTotal, targetPressure, targetTemp, condPeriod));
}

uint Data::NodeSample::insert(const Database &myDB, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp, const QString condPeriod) {
    const QString Script = "CALL insertSample('" + standard + "','" + material + "','" + specification + "'," + QString::number(diamNom) + "," + QString::number(diamReal) + "," + QString::number(wallthick)  + "," + QString::number(lenFree) + "," + QString::number(lenTotal) + "," + QString::number(targetPressure) + "," + QString::number(targetTemp) + ",'" + condPeriod + "');";
    QSqlQuery newSample = myDB.exec(Script);
    newSample.next();
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQl Script: " << Script;
        qDebug() << "ID Sample: "  << newSample.value("idSample").toUInt();
    #endif
    return newSample.value("idSample").toUInt();
}

uint Data::NodeSample::exists(const Database &myDB, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp) {
    const QString Script = "CALL get_idSample('" + standard + "','" + material + "','" + specification + "'," + QString::number(diamNom) + "," + QString::number(diamReal) + "," + QString::number(wallthick)  + "," + QString::number(lenFree) + "," + QString::number(lenTotal) + "," + QString::number(targetPressure) + "," + QString::number(targetTemp) + ");";
    QSqlQuery sampleExists = myDB.exec(Script);
    sampleExists.next();
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQL Script: " << Script;
        qDebug() << "ID Sample: "  << sampleExists.value("idSample").toInt();
    #endif
    return sampleExists.value("idSample").toUInt();
}

const uint Data::NodeSample::getID() { return this->id; }

const int Data::NodeSample::getTargetPressure() { return this->targetPressure; }

const int Data::NodeSample::getTargetTemperature() { return this->targetTemp; }

Data::NodeSpecimen::NodeSpecimen(const uint id, const uint idSample, const uint idOperator, const QString enviromental) : id(id), idSample(idSample), idOperator(idOperator) {
    this->enviromental = enviromental;
}

Data::NodeSpecimen::~NodeSpecimen() { }

QSharedPointer<Data::NodeSpecimen> Data::NodeSpecimen::get(const Database &myDB, const uint idSpecimen) {
    const QString Script = "CALL getSpecimen(" + QString::number(idSpecimen) + ");";
    QSqlQuery getSpecimen = myDB.exec(Script);
    getSpecimen.next();
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQL Script: "  << Script;
    #endif
    const uint idSample = getSpecimen.value("idSample").toUInt(),
               idOperator = getSpecimen.value("operator").toUInt();
    const QString enviromental = getSpecimen.value("enviromental").toString();
    return QSharedPointer<Data::NodeSpecimen>(new Data::NodeSpecimen(idSpecimen, idSample, idOperator, enviromental));
}

QSharedPointer<Data::NodeSpecimen> Data::NodeSpecimen::add(const Database &myDB, const uint idSample, const uint idOperator, const QString enviromental) {
    const QString Script = "CALL insertSpecimen(" + QString::number(idSample) + "," + QString::number(idOperator) + ",'" + enviromental + "');";
    QSqlQuery newSpecimen = myDB.exec(Script);
    newSpecimen.next();
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQL Script: "  << Script;
        qDebug() << "ID Specimen: " << newSpecimen.value("idSpecimen").toUInt();
    #endif
    const uint idSpecimen = newSpecimen.value("idSpecimen").toUInt();
    return QSharedPointer<Data::NodeSpecimen>(new Data::NodeSpecimen(idSpecimen, idSample, idOperator, enviromental));
}

uint Data::NodeSpecimen::insert(const Database &myDB, const uint idSample, const uint idOperator, const QString enviromental) {
    const QString Script = "CALL insertSpecimen(" + QString::number(idSample) + "," + QString::number(idOperator) + ",'" + enviromental + "');";
    QSqlQuery newSpecimen = myDB.exec(Script);
    newSpecimen.next();
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQL Script: "  << Script;
        qDebug() << "ID Specimen: " << newSpecimen.value("idSpecimen").toUInt();
    #endif
    return newSpecimen.value("idSpecimen").toUInt();
}

uint Data::NodeSpecimen::count(const Database &myDB, const uint idSample) {
    const QString Script = "CALL countSpecimens(" + QString::number(idSample) + ");";
    QSqlQuery countSpecimens = myDB.exec(Script);
    countSpecimens.next();
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQL Script: " << Script;
        qDebug() << "Specimens counts: " << countSpecimens.value("counts").toUInt();
    #endif
        return countSpecimens.value("counts").toUInt();
}

const uint Data::NodeSpecimen::getID() { return this->id; }

const uint Data::NodeSpecimen::getIDSample() { return this->idSample; }

const QString Data::NodeSpecimen::getEnviroment() { return this->enviromental; }

const QDateTime Data::NodeSpecimen::getStartTime() { return this->start; }

const QDateTime Data::NodeSpecimen::getEndTime() { return this->end; }

const uint Data::NodeSpecimen::getIDOperator() { return this->idOperator; }

Data::NodeData::NodeData(const uint idSpecimen, const double pressure, const double temperature) : idSpecimen(idSpecimen) {
    this->pressure    = pressure;
    this->temperature = temperature;
}

Data::NodeData::~NodeData() { }

const uint Data::NodeData::getIDSpecimen() { return this->idSpecimen; }

const double Data::NodeData::getTemperature() { return this->temperature; }

const double Data::NodeData::getPressure() { return this->pressure; }

void Data::NodeData::insert(Database &myDB, Data::NodeData &myData) {
    const QString Script = "CALL insertData(" + QString::number(myData.getIDSpecimen()) + "," + QString::number(myData.getPressure()) + "," + QString::number(myData.getTemperature()) + ");";
    QSqlQuery insertData = myDB.exec(Script);
    insertData.next();
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQL Script: " << Script;
    #endif
}
