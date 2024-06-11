#include "../defines.h"
#include "services/database.h"
#include "utils/simplecrypt.h"

Database::Database() : QSqlDatabase("QMYSQL") {
    this->setHostName("");
    this->setPort(0);
    this->setUserName("");
    this->setPassword("");
}

void Database::loadConfiguration() {
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
        throw new DatabaseError::ConfigurationError("Base de datos no configurada!");
    } else {
        this->setHostName(hostName);
        this->setPort(port);
        this->setUserName(userName);
        this->setPassword(password);
    }
}

bool Database::test(const QString hostname, const uint port, const QString username, const QString password) {
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

FrontClases::NodeSetting::NodeSetting(const uint id_, const uint times_, const uint temperature_) : id(id_) {
    this->times       = times_;
    this->temperature = temperature_;
}

QList<QSharedPointer<FrontClases::NodeSetting>> FrontClases::NodeSetting::get(Schemas::Static &myDB, const uint idSpecification) {
    const QString Script = "CALL selectSettingsSpecification(" + QString::number(idSpecification) + ");";
    QList<QSharedPointer<FrontClases::NodeSetting>> auxList;
    try {
        QSqlQuery mySettingsQuery = myDB.exec(Script);
        while(mySettingsQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeSetting>(new FrontClases::NodeSetting(mySettingsQuery.value(0).toUInt(), mySettingsQuery.value(1).toUInt(), mySettingsQuery.value(2).toUInt()))); }
    }
    catch(DatabaseError::QuerySelectError* ex) {}
    return auxList;
}

uint FrontClases::NodeSetting::getID() const { return this->id; }

uint FrontClases::NodeSetting::getTimes() const { return this->times; }

uint FrontClases::NodeSetting::getTemperature() const { return this->temperature; }

FrontClases::NodeSpecification::NodeSpecification(const uint id_, const QString specification_, const QString description_) : id(id_) {
    this->specification = specification_;
    this->description   = description_;
}

QList<QSharedPointer<FrontClases::NodeSpecification>> FrontClases::NodeSpecification::get(Schemas::Static &myDB, const uint idMaterial) {
    const QString Script = "CALL selectSpecifications(" + QString::number(idMaterial) + ");";
    QList<QSharedPointer<FrontClases::NodeSpecification>> auxList;
    try {
        QSqlQuery mySpecificationsQuery = myDB.exec(Script);
        while(mySpecificationsQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeSpecification>(new FrontClases::NodeSpecification(mySpecificationsQuery.value(0).toUInt(), mySpecificationsQuery.value(1).toString(), mySpecificationsQuery.value(2).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* ex) {}
    return auxList;
}

uint FrontClases::NodeSpecification::getID() const { return this->id; }

QString FrontClases::NodeSpecification::getSpecification() const { return this->specification; }

FrontClases::NodeMaterial::NodeMaterial(const uint id_, const QString material_, const QString description_, const QString type_) : id(id_) {
    this->material = material_;
    this->description = description_;
    this->type     = type_;
}

QList<QSharedPointer<FrontClases::NodeMaterial>> FrontClases::NodeMaterial::get(Schemas::Static &myDB, const uint idStandard) {
    const QString Script = "CALL selectMaterials(" + QString::number(idStandard) + ");";
    QList<QSharedPointer<FrontClases::NodeMaterial>> auxList;
    try {
        QSqlQuery myMaterialsQuery = myDB.exec(Script);
        while(myMaterialsQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeMaterial>(new FrontClases::NodeMaterial(myMaterialsQuery.value(0).toUInt(), myMaterialsQuery.value(1).toString(), myMaterialsQuery.value(2).toString(), myMaterialsQuery.value(3).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* ex) {
        qDebug() << "Database: " << ex->what();
        delete ex;
    }
    return auxList;
}

uint FrontClases::NodeMaterial::getID() const { return this->id; }

QString FrontClases::NodeMaterial::getMaterial() const { return this->material; }

QString FrontClases::NodeMaterial::getType() const { return this->type; }

FrontClases::NodeConditionalPeriod::NodeConditionalPeriod(const uint id_, const uint minwall_, const uint maxwall_, const QString condPeriod_) : id(id_) {
    this->minWall = minwall_;
    this->maxWall = maxwall_;
    this->conditionalPeriod = condPeriod_;
}

QList<QSharedPointer<FrontClases::NodeConditionalPeriod>> FrontClases::NodeConditionalPeriod::get(Schemas::Static &myDB, const uint idStandard) {
    const QString Script = "CALL selectConditionalPeriods(" + QString::number(idStandard) + ")";
    QList<QSharedPointer<FrontClases::NodeConditionalPeriod>> auxList;
    try {
        QSqlQuery myCondPeriods = myDB.exec(Script);
        while(myCondPeriods.next()) { auxList.append(QSharedPointer<FrontClases::NodeConditionalPeriod>(new FrontClases::NodeConditionalPeriod(myCondPeriods.value(0).toUInt(), myCondPeriods.value(1).toUInt(), myCondPeriods.value(2).toUInt(), myCondPeriods.value(3).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* e) {
    }
    return auxList;
}

uint FrontClases::NodeConditionalPeriod::getID() const { return this->id; }

uint FrontClases::NodeConditionalPeriod::getMinWall() const { return this->minWall; }

uint FrontClases::NodeConditionalPeriod::getMaxWall() const { return this->maxWall; }

QString FrontClases::NodeConditionalPeriod::getConditionalPeriod() const { return this->conditionalPeriod; }

FrontClases::NodeStandard::NodeStandard(const uint id_, const QString standard_) : id(id_) { this->standard = standard_; }

QList<QSharedPointer<FrontClases::NodeStandard>> FrontClases::NodeStandard::get(Schemas::Static &myDB) {
    const QString Script = "CALL selectStandards();";
    QList<QSharedPointer<FrontClases::NodeStandard>> auxList;
    try {
        QSqlQuery myStandardsQuery = myDB.exec(Script);
        while(myStandardsQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeStandard>(new FrontClases::NodeStandard(myStandardsQuery.value(0).toUInt(), myStandardsQuery.value(1).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* e) {
    }
    return auxList;
}

uint FrontClases::NodeStandard::getID() const { return this->id; }

QString FrontClases::NodeStandard::getStandard() const { return this->standard; }

Data::NodeSample::NodeSample(const NodeSample &me, const uint id_) : id(id_) {
    this->standard       = me.standard;
    this->material       = me.material;
    this->specification  = me.specification;
    this->diamNom        = me.diamNom;
    this->diamReal       = me.diamReal;
    this->wallthick      = me.wallthick;
    this->lenFree        = me.lenFree;
    this->lenTotal       = me.lenTotal;
    this->targetPressure = me.targetPressure;
    this->targetTemp     = me.targetTemp;
    this->condPeriod     = me.condPeriod;
}

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

QSharedPointer<Data::NodeSample> Data::NodeSample::get(Schemas::Data &myDB, const uint idSample) {
    const QString Script = "CALL selectSample(" + QString::number(idSample) + ");";
    QSqlQuery getSample = myDB.exec(Script);
    getSample.next();
#if CONSOLEDEBUGMODE == ConsoleDebugOn
    qDebug() << "SQl Script: " << Script;
#endif
    const QString standard = getSample.value("standard").toString(),
        material           = getSample.value("material").toString(),
        specification      = getSample.value("specification").toString(),
        condPeriod         = getSample.value("condPeriod").toString();
    const uint diamNom = getSample.value("diamnom").toUInt(),
        diamReal       = getSample.value("diamreal").toUInt(),
        wallthick      = getSample.value("wallthickness").toUInt(),
        lenFree        = getSample.value("lenfree").toUInt(),
        lenTotal       = getSample.value("lentotal").toUInt(),
        targetPressure = getSample.value("targetpressure").toUInt(),
        targetTemp     = getSample.value("targettemp").toUInt();
    return QSharedPointer<Data::NodeSample>(new Data::NodeSample(idSample, standard, material, specification, diamNom, diamReal, wallthick, lenFree, lenTotal, targetPressure, targetTemp, condPeriod));
}

uint Data::NodeSample::insert(Schemas::Data &myDB, QSharedPointer<Data::NodeSample> sample) {
    const QString Script = "CALL insertSample('" + sample->standard + "','" + sample->material + "','" + sample->specification + "'," + QString::number(sample->diamNom) + "," + QString::number(sample->diamReal) + "," + QString::number(sample->wallthick)  + "," + QString::number(sample->lenFree) + "," + QString::number(sample->lenTotal) + "," + QString::number(sample->targetPressure) + "," + QString::number(sample->targetTemp) + ",'" + sample->condPeriod + "');";
    QSqlQuery newSample = myDB.exec(Script);
    newSample.next();
    {
        #if CONSOLEDEBUGMODE == ConsoleDebugOn
            qDebug() << "SQl Script: " << Script;
            qDebug() << "ID Sample: "  << newSample.value("idSample").toUInt();
        #endif
    }
    return newSample.value("idSample").toUInt();
}

uint Data::NodeSample::exists(Schemas::Data &myDB, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp) {
    const QString Script = "CALL selectSampleID('" + standard + "','" + material + "','" + specification + "'," + QString::number(diamNom) + "," + QString::number(diamReal) + "," + QString::number(wallthick)  + "," + QString::number(lenFree) + "," + QString::number(lenTotal) + "," + QString::number(targetPressure) + "," + QString::number(targetTemp) + ");";

    try {
        QSqlQuery sampleExists = myDB.exec(Script);
        sampleExists.next();
        {
            #if CONSOLEDEBUGMODE == ConsoleDebugOn
                qDebug() << "SQL Script: " << Script;
                qDebug() << "ID Sample: "  << sampleExists.value("idSample").toInt();
            #endif
        }
        return sampleExists.value("idSample").toUInt();
    }
    catch(DatabaseError::QuerySelectError* ex) { qDebug() << "Base de Datos - Selección: " << ex->what(); }
    return NULL;
}

uint    Data::NodeSample::getID()            { return this->id; }

QString Data::NodeSample::getStandard()      { return this->standard; }

QString Data::NodeSample::getMaterial()      { return this->material; }

QString Data::NodeSample::getSpecification() { return this->specification; }

uint    Data::NodeSample::getDiamNom()       { return this->diamNom; }

uint    Data::NodeSample::getDiamReal()      { return this->diamReal; }

uint    Data::NodeSample::getWallThick()     { return this->wallthick; }

uint    Data::NodeSample::getLenFree()       { return this->lenFree; }

uint    Data::NodeSample::getLenTotal()      { return this->lenTotal; }

QString Data::NodeSample::getCondPeriod()    { return this->condPeriod; }

int     Data::NodeSample::getTargetPressure()    { return this->targetPressure; }

int     Data::NodeSample::getTargetTemperature() { return this->targetTemp; }

Data::NodeSpecimen::NodeSpecimen(const NodeSpecimen &me, const uint id, const uint idSample) : id(id), idSample(idSample) {
    this->operatorName = me.operatorName;
    this->enviroment   = me.enviroment;
    this->testName     = me.testName;
    this->endCap       = me.endCap;
}

Data::NodeSpecimen::NodeSpecimen(const uint id, const uint idSample, const QString operatorName, const QString enviroment, const QString testName, const QString endCap) : id(id), idSample(idSample), operatorName(operatorName) {
    this->enviroment = enviroment;
    this->testName   = testName;
    this->endCap     = endCap;
}

Data::NodeSpecimen::~NodeSpecimen() { }

QSharedPointer<Data::NodeSpecimen> Data::NodeSpecimen::get(Schemas::Data &myDB, const uint idSpecimen) {
    const QString Script = "CALL selectSpecimen(" + QString::number(idSpecimen) + ");";
    QSqlQuery getSpecimen = myDB.exec(Script);
    getSpecimen.next();
    {
        #if CONSOLEDEBUGMODE == ConsoleDebugOn
                qDebug() << "SQL Script: "  << Script;
        #endif
    }

    const uint idSample = getSpecimen.value("idSample").toUInt();
    const QString enviromental = getSpecimen.value("enviroment").toString(),
                  operatorName = getSpecimen.value("operator").toString(),
                  testName     = getSpecimen.value("testName").toString(),
                  endCap       = getSpecimen.value("endCap").toString();
    return QSharedPointer<Data::NodeSpecimen>(new Data::NodeSpecimen(idSpecimen, idSample, operatorName, enviromental, testName, endCap));
}

uint Data::NodeSpecimen::insert(Schemas::Data &myDB, QSharedPointer<Data::NodeSpecimen> specimen) {
    const QString Script = "CALL insertSpecimen(" + QString::number(specimen->idSample) + ",'" +specimen->operatorName + "','" + specimen->enviroment + "','" + specimen->testName + "','" + specimen->endCap + "');";
    QSqlQuery newSpecimen = myDB.exec(Script);
    newSpecimen.next();
    {
        #if CONSOLEDEBUGMODE == ConsoleDebugOn
                qDebug() << "SQL Script: "  << Script;
                qDebug() << "ID Specimen: " << newSpecimen.value("idSpecimen").toUInt();
        #endif
    }

    return newSpecimen.value("idSpecimen").toUInt();
}

uint Data::NodeSpecimen::insert(Schemas::Data &myDB, QSharedPointer<NodeSpecimen> specimen, const uint idSample) {
    const QString Script = "CALL insertSpecimen(" + QString::number(idSample) + ",'" +specimen->operatorName + "','" + specimen->enviroment + "','" + specimen->testName + "','" + specimen->endCap + "');";
    QSqlQuery newSpecimen = myDB.exec(Script);
    newSpecimen.next();
    {
        #if CONSOLEDEBUGMODE == ConsoleDebugOn
            qDebug() << "SQL Script: "  << Script;
            qDebug() << "ID Specimen: " << newSpecimen.value("idSpecimen").toUInt();
        #endif
    }

    return newSpecimen.value("idSpecimen").toUInt();
}

uint Data::NodeSpecimen::count(Schemas::Data &myDB, const uint idSample) {
    const QString Script = "CALL countSpecimens(" + QString::number(idSample) + ");";
    try {
        QSqlQuery countSpecimens = myDB.exec(Script);
        countSpecimens.next();
        {
            #if CONSOLEDEBUGMODE == ConsoleDebugOn
                qDebug() << "SQL Script: " << Script;
                qDebug() << "Specimens counts: " << countSpecimens.value("counts").toUInt();
            #endif
        }

        return countSpecimens.value("counts").toUInt();
    } catch(DatabaseError::QuerySelectError* ex) { qDebug() << "Base de Datos - Selección: " << ex->what(); }
    return NULL;
}

uint Data::NodeSpecimen::getID()                    { return this->id; }

uint Data::NodeSpecimen::getIDSample()              { return this->idSample; }

const QString Data::NodeSpecimen::getEnviroment()   { return this->enviroment; }

const QString Data::NodeSpecimen::getTestName()     { return this->testName; }

const QString Data::NodeSpecimen::getEndCap()       { return this->endCap; }

const QString Data::NodeSpecimen::getOperatorName() { return this->operatorName; }

Data::NodeData::NodeData(const uint idSpecimen, const double pressure, const double temperature) : idSpecimen(idSpecimen) {
    this->pressure    = pressure;
    this->temperature = temperature;
}

Data::NodeData::~NodeData() { }

uint Data::NodeData::getIDSpecimen() { return this->idSpecimen; }

double Data::NodeData::getTemperature() { return this->temperature; }

double Data::NodeData::getPressure() { return this->pressure; }

void Data::NodeData::insert(Schemas::Data &myDB, Data::NodeData &myData) {
    const QString Script = "CALL insertData(" + QString::number(myData.getIDSpecimen()) + "," + QString::number(myData.getPressure()) + "," + QString::number(myData.getTemperature()) + ");";
    QSqlQuery insertData = myDB.exec(Script);
    insertData.next();
#if CONSOLEDEBUGMODE == ConsoleDebugOn
    qDebug() << "SQL Script: " << Script;
#endif
}

Schemas::Data::Data(QLabel *dbstatus, QAction *dbconnect) {
    this->lblConnectDB = dbconnect;
    this->lblConnectionStatus = dbstatus;
    this->myDB.setDatabaseName("stel_db_data");
}

FrontClases::NodeOperator::NodeOperator(const uint id_, const QString name_, const QString familyName_) : id(id_) {
    this->name = name_;
    this->familyName = familyName_;
}

QList<QSharedPointer<FrontClases::NodeOperator> > FrontClases::NodeOperator::get(Schemas::Static &myDB) {
    const QString Script = "CALL selectOperators();";
    QList<QSharedPointer<FrontClases::NodeOperator>> auxList;
    try {
        QSqlQuery myOperatorQuery = myDB.exec(Script);
        while(myOperatorQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeOperator>(new FrontClases::NodeOperator(myOperatorQuery.value(0).toUInt(), myOperatorQuery.value(1).toString(), myOperatorQuery.value(2).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* e) {
    }
    return auxList;
}

uint FrontClases::NodeOperator::getID() const { return this->id; }

QString FrontClases::NodeOperator::getFullName() const { return this->name + " " + this->familyName; }

FrontClases::NodeEnviroment::NodeEnviroment(const uint id_, const QString insertFluid_, const QString outFluid_) : id(id_) {
    this->insertFluid = insertFluid_;
    this->outFluid    = outFluid_;
}

QList<QSharedPointer<FrontClases::NodeEnviroment> > FrontClases::NodeEnviroment::get(Schemas::Static &myDB, const uint idStandard) {
    const QString Script = "CALL selectEnviroment(" + QString::number(idStandard) + ");";
    QList<QSharedPointer<FrontClases::NodeEnviroment>> auxList;
    try {
        QSqlQuery myStandardsQuery = myDB.exec(Script);
        while(myStandardsQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeEnviroment>(new FrontClases::NodeEnviroment(myStandardsQuery.value(0).toUInt(), myStandardsQuery.value(1).toString(), myStandardsQuery.value(2).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* e) {
    }
    return auxList;
}

uint FrontClases::NodeEnviroment::getID() const { return this->id; }

QString FrontClases::NodeEnviroment::getEnviroment() const { return this->insertFluid + " en " + this->outFluid; }

FrontClases::NodeTestType::NodeTestType(const uint id_, const QString testType_) : id(id_) { this->testType = testType_; }

QList<QSharedPointer<FrontClases::NodeTestType> > FrontClases::NodeTestType::get(Schemas::Static &myDB, const uint idStandard) {
    const QString Script = "CALL selectTestType(" + QString::number(idStandard) + ");";
    QList<QSharedPointer<FrontClases::NodeTestType>> auxList;
    try {
        QSqlQuery myTestTypeQuery = myDB.exec(Script);
        while(myTestTypeQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeTestType>(new FrontClases::NodeTestType(myTestTypeQuery.value(0).toUInt(), myTestTypeQuery.value(1).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* e) {
    }
    return auxList;
}

uint FrontClases::NodeTestType::getID() const { return this->id; }

QString FrontClases::NodeTestType::getTestType() const { return this->testType; }

Schemas::Data::~Data() { this->close(); }

void Schemas::Data::changingLblConnectionState(const QString state_, const QString color_) {
    this->lblConnectionStatus->setText("Base de Datos: " + state_);
    this->lblConnectionStatus->setStyleSheet("color:" + color_ +";");
}

void Schemas::Data::open() {
    try {
        this->myDB.loadConfiguration();
        this->prevState = !this->myDB.open();
        if(!this->prevState) { this->lblConnectDB->setText("Desconectar"); }
    }
    catch(DatabaseError::ConfigurationError* ex) {
        QMessageBox myMsgBox(QMessageBox::Warning, "Configuración", ex->what());
        myMsgBox.exec();
    }
}

void Schemas::Data::close() {
    if(!this->myDB.isOpen()) { return; }
    this->myDB.close();
    this->lblConnectDB->setText("Conectar");
}

bool Schemas::Data::isOpen() { return this->myDB.isOpen(); }

QSqlQuery Schemas::Data::exec(QString query) {
    QSqlQuery myQuery(query, this->myDB);
    return myQuery;
}

bool Schemas::Data::status() {
    bool state = false;
    if(this->lblConnectionStatus != nullptr) {
        state = this->myDB.isOpen();
        if(!state && this->prevState) {
#if CONSOLEDEBUGMODE == ConsoleConsoleDebugOn
            qDebug() << "Base de datos: Desconectado";
#endif
            this->changingLblConnectionState("Desconectado", StatusRed);
            this->prevState = false;
        } else if (state && !this->prevState) {
#if CONSOLEDEBUGMODE == ConsoleConsoleDebugOn
            qDebug() << "Base de datos: Conectado";
#endif
            this->changingLblConnectionState("Conectado", StatusGreen);
            this->prevState = true;
        }
    }
    return state;
}

Schemas::Static::Static() { this->myDB.setDatabaseName("stel_db_static"); }

Schemas::Static::~Static() { this->close(); }

bool Schemas::Static::open() {
    bool isOpen = false;
    try {
        this->myDB.loadConfiguration();
        isOpen = this->myDB.open();
    }
    catch(...) {
        // QMessageBox myMsgBox(QMessageBox::Warning, "Configuración", ex->what());
        // myMsgBox.exec();
    }
    return isOpen;
}

void Schemas::Static::close() {
    if(!this->myDB.isOpen()) { return; }
    this->myDB.close();
}

QSqlQuery Schemas::Static::exec(QString query) {
    QSqlQuery myQuery(query, this->myDB);
    return myQuery;
}

FrontClases::NodeEndCap::NodeEndCap(const uint id_, const QString endCap_) : id(id_) { this->endCap = endCap_; }

QList<QSharedPointer<FrontClases::NodeEndCap> > FrontClases::NodeEndCap::get(Schemas::Static &myDB, const uint idStandard) {
    const QString Script = "CALL selectEndCaps(" + QString::number(idStandard) + ");";
    QList<QSharedPointer<FrontClases::NodeEndCap>> auxList;
    try {
        QSqlQuery myTestTypeQuery = myDB.exec(Script);
        while(myTestTypeQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeEndCap>(new FrontClases::NodeEndCap(myTestTypeQuery.value("id").toUInt(), myTestTypeQuery.value("endCap").toString()))); }
    }
    catch(DatabaseError::QuerySelectError* e) {
    }
    return auxList;
}

uint FrontClases::NodeEndCap::getID() const { return this->id; }

QString FrontClases::NodeEndCap::getEndCap() const { return this->endCap; }
