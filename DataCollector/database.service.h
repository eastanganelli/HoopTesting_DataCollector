#ifndef DATABASE_H
#define DATABASE_H
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSharedPointer>
#include <QString>
#include <QList>
#include <QSettings>
#include <QDateTime>
#include <QLabel>
#include "defines.h"
#include "simplecrypt.h"

#define datetime_format "yyyy/MM/dd hh:mm:ss"

class Database : public QSqlDatabase {
    void changingLblConnectionState(const QString state_, const QString color_);

    QLabel*  lblConnectionStatus;
    QAction* lblConnectDB;
    bool myState;
public:
    Database();
    Database(const Database& me);
    Database(const QString host, const uint port, const QString user, const QString password, QLabel* dbstatus, QAction* dbconnect);
    Database(const QString host, const uint port, const QString user, const QString password, const QString dbName, QLabel* dbstatus, QAction* dbconnect);
    void openDatabase();
    void closeDatabase();

    static bool test(const QString hostname, const uint port, const QString username, const QString password);
    static bool test(Database testDB);
    static void save(const QString hostname, const uint port, const QString username, const QString password);
    static void save(const Database myDB);
    static Database *read();
    bool status();
};

namespace Data {
    class NodeSample {
        const uint id;
        QString standard, material, specification, condPeriod;
        uint diamNom, diamReal, wallthick, lenFree, lenTotal;
        int targetPressure, targetTemp;
    public:
        NodeSample(const uint id_, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp, const QString condPeriod);
        ~NodeSample();

        static QSharedPointer<NodeSample> get(const Database& myDB, const uint idSample);
        static QSharedPointer<NodeSample> add(const Database& myDB, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp, const QString condPeriod);
        static uint insert(const Database& myDB, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp, const QString condPeriod);
        static uint exists(const Database& myDB, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp);

       const uint getID();
       const int  getTargetPressure();
       const int  getTargetTemperature();
    };
    class NodeSpecimen {
        const uint id, idSample;
        QString enviromental;
        QDateTime start, end;
        uint idOperator;
    public:
        NodeSpecimen(const uint id, const uint idSample, const uint idOperator, const QString enviromental);
        ~NodeSpecimen();

        static QSharedPointer<NodeSpecimen> get(const Database& myDB, const uint idSpecimen);
        static QSharedPointer<NodeSpecimen> add(const Database& myDB, const uint idSample, const uint idOperator, const QString enviromental);
        static uint insert(const Database& myDB, const uint idSample, const uint idOperator, const QString enviromental);
        static uint count(const Database& myDB, const uint idSample);

        const uint getID();
        const uint getIDSample();
        const QString getEnviroment();
        const QDateTime getStartTime();
        const QDateTime getEndTime();
        const uint getIDOperator();
    };
    class NodeData {
        const uint idSpecimen;
        double pressure, temperature;
    public:
        NodeData(const uint idData, const uint idSpecimen, const double pressure, const double temperature);
        NodeData(const uint idSpecimen, const double pressure, const double temperature);
        ~NodeData();

        const uint getIDSpecimen();
        const double getTemperature();
        const double getPressure();
        static void insert(Database& myDB, NodeData &myData);
    };
    class NodeOperator {
    public:
        NodeOperator();
        ~NodeOperator();
    };
}

namespace FrontClases {
    class NodeSetting {
        const uint id;
        uint times, temperature;
    public:
        NodeSetting(const uint id_, const uint times_, const uint temperature_);
        static QList<QSharedPointer<FrontClases::NodeSetting>> getList(const Database& myDB, const uint idStandard, const uint idMaterial, const uint idSpecification);
        static void update(const Database& myDB, const NodeSetting node);

        uint getID() const;
        uint getTimes() const;
        uint getTemperature() const;
    };
    class NodeSpecification {
        const uint id;
        QString specification;
    public:
        NodeSpecification(const uint id_, const QString specification_);
        static QList<QSharedPointer<FrontClases::NodeSpecification>> getList(const Database& myDB, const uint idStandard, const uint idMaterial);
        static void update(const Database& myDB, const NodeSpecification node);

        uint getID() const;
        QString getSpecification() const;
    };
    class NodeMaterial {
        const uint id;
        QString material, type;
    public:
        NodeMaterial(const uint id_, const QString material_, const QString type_);
        static QList<QSharedPointer<FrontClases::NodeMaterial>> getList(const Database& myDB, const uint idStandard);
        static void update(const Database& myDB, const NodeMaterial node);

        uint getID() const;
        QString getMaterial() const;
        QString getType() const;
    };
    class NodeConditionalPeriod {
        const uint id;
        uint minWall, maxWall;
        QString conditionalPeriod;
    public:
        NodeConditionalPeriod(const uint id_, const uint minwall_, const uint maxwall_, const QString condPeriod_);
        static QList<QSharedPointer<FrontClases::NodeConditionalPeriod>> getList(const Database& myDB, const uint idStandard);
        static void update(const Database& myDB, const NodeConditionalPeriod node);

        uint getID() const;
        uint getMinWall() const;
        uint getMaxWall() const;
        QString getConditionalPeriod() const;
    };
    class NodeStandard {
        const uint id;
        QString standard;
    public:
        NodeStandard(const uint id_, const QString standard_);
        static QList<QSharedPointer<FrontClases::NodeStandard>> getList(const Database& myDB);
        static void update(const Database& myDB, const NodeStandard node);

        uint getID() const;
        QString getStandard() const;
    };
};
#endif // DATABASE_H
