#ifndef SCHEMAFRONT_H
#define SCHEMAFRONT_H

#define datetime_format "yyyy/MM/dd hh:mm:ss"

namespace FrontClases {
class NodeSetting {
    const uint id;
    uint times, temperature;
public:
    NodeSetting(const uint id_, const uint times_, const uint temperature_);
    static QList<QSharedPointer<FrontClases::NodeSetting>> get(const uint idSpecification);

    uint getID() const;
    uint getTimes() const;
    uint getTemperature() const;
};

class NodeSpecification {
    const uint id;
    QString specification,
        description;
public:
    NodeSpecification(const uint id_, const QString specification_, const QString description_);
    static QList<QSharedPointer<FrontClases::NodeSpecification>> get(const uint idMaterial);

    uint getID() const;
    QString getSpecification() const;
};

class NodeMaterial {
    const uint id;
    QString material, description, type;
public:
    NodeMaterial(const uint id_, const QString material_, const QString description_, const QString type_);
    static QList<QSharedPointer<FrontClases::NodeMaterial>> get(const uint idStandard);

    uint getID() const;
    QString getMaterial() const;
    QString getType() const;
};

class NodeEnviroment {
    const uint id;
    QString insertFluid, outFluid;
public:
    NodeEnviroment(const uint id_, const QString insertFluid_, const QString outFluid_);
    static QList<QSharedPointer<FrontClases::NodeEnviroment>> get(const uint idStandard);

    uint getID() const;
    QString getEnviroment() const;
};

class NodeTestType {
    const uint id;
    QString testType;
public:
    NodeTestType(const uint id_, const QString testType_);
    static QList<QSharedPointer<FrontClases::NodeTestType>> get(const uint idStandard);

    uint getID() const;
    QString getTestType() const;
};

class NodeConditionalPeriod {
    const uint id;
    uint minWall, maxWall;
    QString conditionalPeriod;
public:
    NodeConditionalPeriod(const uint id_, const uint minwall_, const uint maxwall_, const QString condPeriod_);
    static QList<QSharedPointer<FrontClases::NodeConditionalPeriod>> get(const uint idStandard);

    uint getID() const;
    uint getMinWall() const;
    uint getMaxWall() const;
    QString getConditionalPeriod() const;
};

class NodeEndCap {
    const uint id;
    QString endCap;
public:
    NodeEndCap(const uint id_, const QString endCap_);
    static QList<QSharedPointer<FrontClases::NodeEndCap>> get(const uint idStandard);

    uint getID() const;
    QString getEndCap() const;
};

class NodeStandard {
    const uint id;
    QString standard;
public:
    NodeStandard(const uint id_, const QString standard_);
    static QList<QSharedPointer<FrontClases::NodeStandard>> get();

    uint getID() const;
    QString getStandard() const;
};

class NodeOperator {
    const uint id;
    QString name, familyName;
public:
    NodeOperator(const uint id_, const QString name_, const QString familyName_);
    static QList<QSharedPointer<FrontClases::NodeOperator>> get();

    uint getID() const;
    QString getFullName() const;
};
};

#endif // SCHEMAFRONT_H
