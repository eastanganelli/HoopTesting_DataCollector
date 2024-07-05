#include "../../services/database.h"
#include <QSqlDatabase>
#include "schemaData.h"

Data::NodeSample::NodeSample(const NodeSample &me, const uint id_) : id(id_) {
    this->standard       = me.standard;
    this->material       = me.material;
    this->specification  = me.specification;
    this->diamNom        = me.diamNom;
    this->diamReal       = me.diamReal;
    this->wallthick      = me.wallthick;
    this->lenFree        = me.lenFree;
    this->lenTotal       = me.lenTotal;
    this->condPeriod     = me.condPeriod;
}

Data::NodeSample::NodeSample(const uint id_, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const QString condPeriod) : id(id_) {
    this->standard       = standard;
    this->material       = material;
    this->specification  = specification;
    this->diamNom        = diamNom;
    this->diamReal       = diamReal;
    this->wallthick      = wallthick;
    this->lenFree        = lenFree;
    this->lenTotal       = lenTotal;
    this->condPeriod     = condPeriod;
}

Data::NodeSample::~NodeSample() { }

QSharedPointer<Data::NodeSample> Data::NodeSample::get(const uint idSample) {
    const QString Script = QString("CALL selectSample(%1);").arg(idSample);
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    QSqlQuery getSample(myDB);
    getSample.exec(Script);
    getSample.next();
    const QString standard = getSample.value("standard").toString(),
        material           = getSample.value("material").toString(),
        specification      = getSample.value("specification").toString(),
        condPeriod         = getSample.value("condPeriod").toString();
    const uint diamNom = getSample.value("diamnom").toUInt(),
        diamReal       = getSample.value("diamreal").toUInt(),
        wallthick      = getSample.value("wallthickness").toUInt(),
        lenFree        = getSample.value("lenfree").toUInt(),
        lenTotal       = getSample.value("lentotal").toUInt();
    return QSharedPointer<Data::NodeSample>(new Data::NodeSample(idSample, standard, material, specification, diamNom, diamReal, wallthick, lenFree, lenTotal, condPeriod));
}

uint Data::NodeSample::insert(QSharedPointer<Data::NodeSample> sample) {
    const QString Script = QString("CALL insertSample('%1','%2','%3',%4,%5,%6,%7,%8,'%9')").arg(sample->standard).arg(sample->material).arg(sample->specification).arg(sample->diamNom).arg(sample->diamReal).arg(sample->wallthick).arg(sample->lenFree).arg(sample->lenTotal).arg(sample->condPeriod);
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    QSqlQuery newSample(myDB);
    newSample.exec(Script);
    newSample.next();
    return newSample.value("idSample").toUInt();
}

uint Data::NodeSample::exists(const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal) {
    const QString Script = QString("CALL selectSampleID('%1','%2','%3',%4,%5,%6,%7,%8)").arg(standard).arg(material).arg(specification).arg(diamNom).arg(diamReal).arg(wallthick).arg(lenFree).arg(lenTotal);
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    try {
        QSqlQuery sampleExists(myDB);
        sampleExists.exec(Script);
        sampleExists.next();
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

Data::NodeSpecimen::NodeSpecimen(const NodeSpecimen &me, const uint id, const uint idSample) : id(id), idSample(idSample) {
    this->operatorName   = me.operatorName;
    this->targetPressure = me.targetPressure;
    this->targetTemp     = me.targetTemp;
    this->enviroment     = me.enviroment;
    this->testName       = me.testName;
    this->endCap         = me.endCap;
}

Data::NodeSpecimen::NodeSpecimen(const uint id, const uint idSample, const int targetPressure, const int targetTemp, const QString &operatorName, const QString &enviroment, const QString &testName, const QString &endCap) : id(id), idSample(idSample), operatorName(operatorName) {
    this->targetPressure = targetPressure;
    this->targetTemp     = targetTemp;
    this->enviroment = enviroment;
    this->testName   = testName;
    this->endCap     = endCap;
}

Data::NodeSpecimen::~NodeSpecimen() { }

QSharedPointer<Data::NodeSpecimen> Data::NodeSpecimen::get(const uint idSpecimen) {
    const QString Script = QString("CALL selectSpecimen(%1);").arg(idSpecimen);
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) { throw "no esta abierto"; }
    QSqlQuery getSpecimen(myDB);
    getSpecimen.exec(Script);
    getSpecimen.next();

    const uint idSample = getSpecimen.value("idSample").toUInt();
    const QString enviromental = getSpecimen.value("enviroment").toString(),
        operatorName = getSpecimen.value("operator").toString(),
        testName     = getSpecimen.value("testName").toString(),
        endCap       = getSpecimen.value("endCap").toString();
    const int targetPressure = getSpecimen.value("pressure").toInt(),
              targetTemp      = getSpecimen.value("temperature").toInt();
    return QSharedPointer<Data::NodeSpecimen>(new Data::NodeSpecimen(idSpecimen, idSample, targetPressure, targetTemp, operatorName, enviromental, testName, endCap));
}

uint Data::NodeSpecimen::insert(QSharedPointer<Data::NodeSpecimen> specimen) {
    const QString Script = QString("CALL insertSpecimen(%1,%2,%3,'%4','%5','%6','%7');").arg(specimen->idSample).arg(specimen->targetPressure).arg(specimen->targetTemp).arg(specimen->operatorName).arg(specimen->enviroment).arg(specimen->testName).arg(specimen->endCap);
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    QSqlQuery newSpecimen(myDB);
    newSpecimen.exec(Script);
    newSpecimen.next();
    return newSpecimen.value("idSpecimen").toUInt();
}

uint Data::NodeSpecimen::insert(QSharedPointer<NodeSpecimen> specimen, const uint idSample) {
    const QString Script = QString("CALL insertSpecimen(%1,%2,%3,'%4','%5','%6','%7');").arg(idSample).arg(specimen->targetPressure).arg(specimen->targetTemp).arg(specimen->operatorName).arg(specimen->enviroment).arg(specimen->testName).arg(specimen->endCap);
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    QSqlQuery newSpecimen(myDB);
    newSpecimen.exec(Script);
    newSpecimen.next();
    return newSpecimen.value("idSpecimen").toUInt();
}

uint Data::NodeSpecimen::count(const uint idSample) {
    const QString Script = QString("CALL countSpecimens(%1);").arg(idSample);
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    try {
        QSqlQuery countSpecimens(myDB);
        countSpecimens.exec(Script);
        countSpecimens.next();
        return countSpecimens.value("counts").toUInt();
    } catch(DatabaseError::QuerySelectError* ex) { qDebug() << "Base de Datos - Selección: " << ex->what(); }
    return NULL;
}

uint Data::NodeSpecimen::getID()                    { return this->id; }

uint Data::NodeSpecimen::getIDSample()              { return this->idSample; }

int Data::NodeSpecimen::getTargetPressure()       { return this->targetPressure; }

int Data::NodeSpecimen::getTargetTemperature()    { return this->targetTemp; }

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

void Data::NodeData::insert(Data::NodeData &myData) {
    const QString Script = QString("CALL insertData(%1,%2,%3);").arg(myData.idSpecimen).arg(myData.pressure).arg(myData.temperature);
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    QSqlQuery insertData(myDB);
    insertData.exec(Script);
    insertData.next();
}
