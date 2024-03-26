#ifndef DATABASE_H
#define DATABASE_H
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSharedPointer>
#include <QString>
#include <QList>
#include <QSettings>
#include <QDateTime>
#include <QLabel>
#include <QException>

#define datetime_format "yyyy/MM/dd hh:mm:ss"

class Database : public QSqlDatabase {
public:
    Database();
    void loadConfiguration();

    static bool test(const QString hostname, const uint port, const QString username, const QString password);
    static bool test(Database testDB);
    static void save(const QString hostname, const uint port, const QString username, const QString password);
    static void save(const Database myDB);
};

namespace Schemas {
    class Data {
        bool prevState;
        Database myDB;
        QLabel*  lblConnectionStatus;
        QAction* lblConnectDB;

        void changingLblConnectionState(const QString state_, const QString color_);
    public:
        Data(QLabel* dbstatus, QAction* dbconnect);
        ~Data();
        void open();
        void close();
        bool isOpen();
        QSqlQuery exec(QString query);
        bool status();
    };

    class Static {
        Database myDB;
    public:
        Static();
        ~Static();
        bool open();
        void close();
        QSqlQuery exec(QString query);
    };
}

namespace Data {
    class NodeSample {
        const uint id;
        QString standard, material, specification, condPeriod;
        uint diamNom, diamReal, wallthick, lenFree, lenTotal;
        int targetPressure, targetTemp;
    public:
        NodeSample(const uint id_, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp, const QString condPeriod);
        ~NodeSample();

        static QSharedPointer<NodeSample> get(Schemas::Data& myDB, const uint idSample);
        static QSharedPointer<NodeSample> add(Schemas::Data& myDB, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp, const QString condPeriod);
        static uint insert(Schemas::Data& myDB, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp, const QString condPeriod);
        static uint exists(Schemas::Data& myDB, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const int targetPressure, const int targetTemp);

        uint getID();
        int  getTargetPressure();
        int  getTargetTemperature();
    };

    class NodeSpecimen {
        const uint id, idSample;
        QString enviromental;
        QDateTime start, end;
        uint idOperator;
    public:
        NodeSpecimen(const uint id, const uint idSample, const uint idOperator, const QString enviromental);
        ~NodeSpecimen();

        static QSharedPointer<NodeSpecimen> get(Schemas::Data& myDB, const uint idSpecimen);
        static QSharedPointer<NodeSpecimen> add(Schemas::Data& myDB, const uint idSample, const uint idOperator, const QString enviromental);
        static uint insert(Schemas::Data& myDB, const uint idSample, const uint idOperator, const QString enviromental);
        static uint count(Schemas::Data& myDB, const uint idSample);

        uint getID();
        uint getIDSample();
        const QString getEnviroment();
        const QDateTime getStartTime();
        const QDateTime getEndTime();
        uint getIDOperator();
    };

    class NodeData {
        const uint idSpecimen;
        double pressure, temperature;
    public:
        NodeData(const uint idData, const uint idSpecimen, const double pressure, const double temperature);
        NodeData(const uint idSpecimen, const double pressure, const double temperature);
        ~NodeData();

        uint getIDSpecimen();
        double getTemperature();
        double getPressure();
        static void insert(Schemas::Data& myDB, NodeData &myData);
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
        static QList<QSharedPointer<FrontClases::NodeSetting>> get(Schemas::Static& myDB, const uint idSpecification);

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
        static QList<QSharedPointer<FrontClases::NodeSpecification>> get(Schemas::Static& myDB, const uint idMaterial);

        uint getID() const;
        QString getSpecification() const;
    };

    class NodeMaterial {
        const uint id;
        QString material, description, type;
    public:
        NodeMaterial(const uint id_, const QString material_, const QString description_, const QString type_);
        static QList<QSharedPointer<FrontClases::NodeMaterial>> get(Schemas::Static& myDB, const uint idStandard);

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
        static QList<QSharedPointer<FrontClases::NodeConditionalPeriod>> get(Schemas::Static& myDB, const uint idStandard);

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
        static QList<QSharedPointer<FrontClases::NodeStandard>> get(Schemas::Static &myDB);

        uint getID() const;
        QString getStandard() const;
    };
};

namespace DatabaseError {
    class ConfigurationError : public QException {
        char* error;
    public:
        ConfigurationError(char* error) { this->error = error; }
        const char* what() { return this->error; }
        void raise() const override { throw *this; }
        ConfigurationError *clone() const override { return new ConfigurationError(*this); }
    };

    class QuerySelectError : public QException {
        char* error;
    public:
        QuerySelectError(char* error) { this->error = error; }
        const char* what() { return this->error; }
        void raise() const override { throw *this; }
        QuerySelectError *clone() const override { return new QuerySelectError(*this); }
    };

    class QueryInsertError : public QException {
        char* error;
    public:
        QueryInsertError(char* error) { this->error = error; }
        const char* what() { return this->error; }
        void raise() const override { throw *this; }
        QueryInsertError *clone() const override { return new QueryInsertError(*this); }
    };

    class QueryUpdateError : public QException {
        char* error;
    public:
        QueryUpdateError(char* error) { this->error = error; }
        const char* what() { return this->error; }
        void raise() const override { throw *this; }
        QueryUpdateError *clone() const override { return new QueryUpdateError(*this); }
    };

    class QueryDeleteError : public QException {
        char* error;
    public:
        QueryDeleteError(char* error) { this->error = error; }
        const char* what() { return this->error; }
        void raise() const override { throw *this; }
        QueryDeleteError *clone() const override { return new QueryDeleteError(*this); }
    };
}
#endif // DATABASE_H
