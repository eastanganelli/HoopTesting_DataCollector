#include "services/database.h"
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

QSharedPointer<Data::NodeSample> Data::NodeSample::get(const uint idSample) {
    const QString Script = "CALL selectSample(" + QString::number(idSample) + ");";
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    QSqlQuery getSample(myDB);
    getSample.exec(Script);
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

uint Data::NodeSample::insert(QSharedPointer<Data::NodeSample> sample) {
    const QString Script = "CALL insertSample('" + sample->standard + "','" + sample->material + "','" + sample->specification + "'," + QString::number(sample->diamNom) + "," + QString::number(sample->diamReal) + "," + QString::number(sample->wallthick)  + "," + QString::number(sample->lenFree) + "," + QString::number(sample->lenTotal) + "," + QString::number(sample->targetPressure) + "," + QString::number(sample->targetTemp) + ",'" + sample->condPeriod + "');";
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    QSqlQuery newSample(myDB);
    newSample.exec(Script);
    newSample.next();
    {
#if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQl Script: " << Script;
        qDebug() << "ID Sample: "  << newSample.value("idSample").toUInt();
#endif
    }
    return newSample.value("idSample").toUInt();
}

uint Data::NodeSample::exists(const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp) {
    const QString Script = "CALL selectSampleID('" + standard + "','" + material + "','" + specification + "'," + QString::number(diamNom) + "," + QString::number(diamReal) + "," + QString::number(wallthick)  + "," + QString::number(lenFree) + "," + QString::number(lenTotal) + "," + QString::number(targetPressure) + "," + QString::number(targetTemp) + ");";
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    try {
        QSqlQuery sampleExists(myDB);
        sampleExists.exec(Script);
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

QSharedPointer<Data::NodeSpecimen> Data::NodeSpecimen::get(const uint idSpecimen) {
    const QString Script = "CALL selectSpecimen(" + QString::number(idSpecimen) + ");";
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    QSqlQuery getSpecimen(myDB);
    getSpecimen.exec(Script);
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

uint Data::NodeSpecimen::insert(QSharedPointer<Data::NodeSpecimen> specimen) {
    const QString Script = "CALL insertSpecimen(" + QString::number(specimen->idSample) + ",'" +specimen->operatorName + "','" + specimen->enviroment + "','" + specimen->testName + "','" + specimen->endCap + "');";
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    QSqlQuery newSpecimen(myDB);
    newSpecimen.exec(Script);
    newSpecimen.next();
    {
#if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQL Script: "  << Script;
        qDebug() << "ID Specimen: " << newSpecimen.value("idSpecimen").toUInt();
#endif
    }
    return newSpecimen.value("idSpecimen").toUInt();
}

uint Data::NodeSpecimen::insert(QSharedPointer<NodeSpecimen> specimen, const uint idSample) {
    const QString Script = "CALL insertSpecimen(" + QString::number(idSample) + ",'" +specimen->operatorName + "','" + specimen->enviroment + "','" + specimen->testName + "','" + specimen->endCap + "');";
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    QSqlQuery newSpecimen(myDB);
    newSpecimen.exec(Script);
    newSpecimen.next();
    {
#if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "SQL Script: "  << Script;
        qDebug() << "ID Specimen: " << newSpecimen.value("idSpecimen").toUInt();
#endif
    }
    return newSpecimen.value("idSpecimen").toUInt();
}

uint Data::NodeSpecimen::count(const uint idSample) {
    const QString Script = "CALL countSpecimens(" + QString::number(idSample) + ");";
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    try {
        QSqlQuery countSpecimens(myDB);
        countSpecimens.exec(Script);
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

void Data::NodeData::insert(Data::NodeData &myData) {
    const QString Script = "CALL insertData(" + QString::number(myData.getIDSpecimen()) + "," + QString::number(myData.getPressure()) + "," + QString::number(myData.getTemperature()) + ");";
    QSqlDatabase myDB = QSqlDatabase::database(DATA_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "no esta abierto";
    }
    QSqlQuery insertData(myDB);
    insertData.exec(Script);
    insertData.next();
#if CONSOLEDEBUGMODE == ConsoleDebugOn
    qDebug() << "SQL Script: " << Script;
#endif
}
