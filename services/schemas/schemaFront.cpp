#include "../../services/database.h"
#include "schemaFront.h"

FrontClases::NodeSetting::NodeSetting(const uint id_, const uint times_, const uint temperature_) : id(id_) {
    this->times       = times_;
    this->temperature = temperature_;
}

QList<QSharedPointer<FrontClases::NodeSetting>> FrontClases::NodeSetting::get(const uint idSpecification) {
    const QString Script = "CALL selectSettingsSpecification(" + QString::number(idSpecification) + ");";
    QSqlDatabase myDB = QSqlDatabase::database(STATIC_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "Ho hay conexión a la base de datos.";
    }
    QList<QSharedPointer<FrontClases::NodeSetting>> auxList;
    try {
        QSqlQuery mySettingsQuery(myDB);
        mySettingsQuery.exec(Script);
        while(mySettingsQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeSetting>(new FrontClases::NodeSetting(mySettingsQuery.value(0).toUInt(), mySettingsQuery.value(1).toUInt(), mySettingsQuery.value(2).toUInt()))); }
    }
    catch(DatabaseError::QuerySelectError* ex) { qDebug() << ex->what(); }
    return auxList;
}

uint FrontClases::NodeSetting::getID() const { return this->id; }

uint FrontClases::NodeSetting::getTimes() const { return this->times; }

uint FrontClases::NodeSetting::getTemperature() const { return this->temperature; }

FrontClases::NodeSpecification::NodeSpecification(const uint id_, const QString specification_, const QString description_) : id(id_) {
    this->specification = specification_;
    this->description   = description_;
}

QList<QSharedPointer<FrontClases::NodeSpecification>> FrontClases::NodeSpecification::get(const uint idMaterial) {
    const QString Script = "CALL selectSpecifications(" + QString::number(idMaterial) + ");";
    QSqlDatabase myDB = QSqlDatabase::database(STATIC_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "Ho hay conexión a la base de datos.";
    }
    QList<QSharedPointer<FrontClases::NodeSpecification>> auxList;
    try {
        QSqlQuery mySpecificationsQuery(myDB);
        mySpecificationsQuery.exec(Script);
        while(mySpecificationsQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeSpecification>(new FrontClases::NodeSpecification(mySpecificationsQuery.value(0).toUInt(), mySpecificationsQuery.value(1).toString(), mySpecificationsQuery.value(2).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* ex) { qDebug() << ex->what(); }
    return auxList;
}

uint FrontClases::NodeSpecification::getID() const { return this->id; }

QString FrontClases::NodeSpecification::getSpecification() const { return this->specification; }

FrontClases::NodeMaterial::NodeMaterial(const uint id_, const QString material_, const QString description_, const QString type_) : id(id_) {
    this->material = material_;
    this->description = description_;
    this->type     = type_;
}

QList<QSharedPointer<FrontClases::NodeMaterial>> FrontClases::NodeMaterial::get(const uint idStandard) {
    const QString Script = "CALL selectMaterials(" + QString::number(idStandard) + ");";
    QSqlDatabase myDB = QSqlDatabase::database(STATIC_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "Ho hay conexión a la base de datos.";
    }
    QList<QSharedPointer<FrontClases::NodeMaterial>> auxList;
    try {
        QSqlQuery myMaterialsQuery(myDB);
        myMaterialsQuery.exec(Script);
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

QList<QSharedPointer<FrontClases::NodeConditionalPeriod>> FrontClases::NodeConditionalPeriod::get(const uint idStandard) {
    const QString Script = "CALL selectConditionalPeriods(" + QString::number(idStandard) + ")";
    QSqlDatabase myDB = QSqlDatabase::database(STATIC_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "Ho hay conexión a la base de datos.";
    }
    QList<QSharedPointer<FrontClases::NodeConditionalPeriod>> auxList;
    try {
        QSqlQuery myCondPeriods(myDB);
        myCondPeriods.exec(Script);
        while(myCondPeriods.next()) { auxList.append(QSharedPointer<FrontClases::NodeConditionalPeriod>(new FrontClases::NodeConditionalPeriod(myCondPeriods.value(0).toUInt(), myCondPeriods.value(1).toUInt(), myCondPeriods.value(2).toUInt(), myCondPeriods.value(3).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* ex) { qDebug() << ex->what(); }
    return auxList;
}

uint FrontClases::NodeConditionalPeriod::getID() const { return this->id; }

uint FrontClases::NodeConditionalPeriod::getMinWall() const { return this->minWall; }

uint FrontClases::NodeConditionalPeriod::getMaxWall() const { return this->maxWall; }

QString FrontClases::NodeConditionalPeriod::getConditionalPeriod() const { return this->conditionalPeriod; }

FrontClases::NodeStandard::NodeStandard(const uint id_, const QString standard_) : id(id_) { this->standard = standard_; }

QList<QSharedPointer<FrontClases::NodeStandard>> FrontClases::NodeStandard::get() {
    const QString Script = "CALL selectStandards();";
    QSqlDatabase myDB = QSqlDatabase::database(STATIC_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "Ho hay conexión a la base de datos.";
    }
    QList<QSharedPointer<FrontClases::NodeStandard>> auxList;
    try {
        QSqlQuery myStandardsQuery(myDB);
        myStandardsQuery.exec(Script);
        while(myStandardsQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeStandard>(new FrontClases::NodeStandard(myStandardsQuery.value(0).toUInt(), myStandardsQuery.value(1).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* ex) { qDebug() << ex->what(); }
    return auxList;
}

uint FrontClases::NodeStandard::getID() const { return this->id; }

QString FrontClases::NodeStandard::getStandard() const { return this->standard; }

FrontClases::NodeOperator::NodeOperator(const uint id_, const QString name_, const QString familyName_) : id(id_) {
    this->name = name_;
    this->familyName = familyName_;
}

QList<QSharedPointer<FrontClases::NodeOperator> > FrontClases::NodeOperator::get() {
    const QString Script = "CALL selectOperators();";
    QSqlDatabase myDB = QSqlDatabase::database(STATIC_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "Ho hay conexión a la base de datos.";
    }
    QList<QSharedPointer<FrontClases::NodeOperator>> auxList;
    try {
        QSqlQuery myOperatorQuery(myDB);
        myOperatorQuery.exec(Script);
        while(myOperatorQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeOperator>(new FrontClases::NodeOperator(myOperatorQuery.value("dni").toUInt(), myOperatorQuery.value("name").toString(), myOperatorQuery.value("familyname").toString()))); }
    }
    catch(DatabaseError::QuerySelectError* ex) { qDebug() << ex->what(); }
    return auxList;
}

uint FrontClases::NodeOperator::getID() const { return this->id; }

QString FrontClases::NodeOperator::getFullName() const { return this->name + " " + this->familyName; }

FrontClases::NodeEnviroment::NodeEnviroment(const uint id_, const QString insertFluid_, const QString outFluid_) : id(id_) {
    this->insertFluid = insertFluid_;
    this->outFluid    = outFluid_;
}

QList<QSharedPointer<FrontClases::NodeEnviroment> > FrontClases::NodeEnviroment::get(const uint idStandard) {
    const QString Script = "CALL selectEnviroment(" + QString::number(idStandard) + ");";
    QSqlDatabase myDB = QSqlDatabase::database(STATIC_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "Ho hay conexión a la base de datos.";
    }
    QList<QSharedPointer<FrontClases::NodeEnviroment>> auxList;
    try {
        QSqlQuery myStandardsQuery(myDB);
        myStandardsQuery.exec(Script);
        while(myStandardsQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeEnviroment>(new FrontClases::NodeEnviroment(myStandardsQuery.value(0).toUInt(), myStandardsQuery.value(1).toString(), myStandardsQuery.value(2).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* ex) { qDebug() << ex->what(); }
    return auxList;
}

uint FrontClases::NodeEnviroment::getID() const { return this->id; }

QString FrontClases::NodeEnviroment::getEnviroment() const { return this->insertFluid + " en " + this->outFluid; }

FrontClases::NodeTestType::NodeTestType(const uint id_, const QString testType_) : id(id_) { this->testType = testType_; }

QList<QSharedPointer<FrontClases::NodeTestType> > FrontClases::NodeTestType::get(const uint idStandard) {
    const QString Script = "CALL selectTestType(" + QString::number(idStandard) + ");";
    QSqlDatabase myDB = QSqlDatabase::database(STATIC_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "Ho hay conexión a la base de datos.";
    }
    QList<QSharedPointer<FrontClases::NodeTestType>> auxList;
    try {
        QSqlQuery myTestTypeQuery(myDB);
        myTestTypeQuery.exec(Script);
        while(myTestTypeQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeTestType>(new FrontClases::NodeTestType(myTestTypeQuery.value(0).toUInt(), myTestTypeQuery.value(1).toString()))); }
    }
    catch(DatabaseError::QuerySelectError* ex) { qDebug() << ex->what(); }
    return auxList;
}

uint FrontClases::NodeTestType::getID() const { return this->id; }

QString FrontClases::NodeTestType::getTestType() const { return this->testType; }


FrontClases::NodeEndCap::NodeEndCap(const uint id_, const QString endCap_) : id(id_) { this->endCap = endCap_; }

QList<QSharedPointer<FrontClases::NodeEndCap> > FrontClases::NodeEndCap::get(const uint idStandard) {
    const QString Script = "CALL selectEndCaps(" + QString::number(idStandard) + ");";
    QSqlDatabase myDB = QSqlDatabase::database(STATIC_SCHEMA_NAME);
    if(!myDB.isOpen()) {
        throw "Ho hay conexión a la base de datos.";
    }
    QList<QSharedPointer<FrontClases::NodeEndCap>> auxList;
    try {
        QSqlQuery myTestTypeQuery(myDB);
        myTestTypeQuery.exec(Script);
        while(myTestTypeQuery.next()) { auxList.append(QSharedPointer<FrontClases::NodeEndCap>(new FrontClases::NodeEndCap(myTestTypeQuery.value("id").toUInt(), myTestTypeQuery.value("endCap").toString()))); }
    }
    catch(DatabaseError::QuerySelectError* ex) { qDebug() << ex->what(); }
    return auxList;
}

uint FrontClases::NodeEndCap::getID() const { return this->id; }

QString FrontClases::NodeEndCap::getEndCap() const { return this->endCap; }
