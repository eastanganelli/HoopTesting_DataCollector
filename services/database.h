#ifndef DATABASE_H
#define DATABASE_H
#include <QString>
#include <QObject>
#include <QSettings>
#include <QException>
#include <QApplication>
#include <QSqlDatabase>

#define datetime_format "yyyy/MM/dd hh:mm:ss"

class Manager: public QObject {
    Q_OBJECT

    class RemoteDB {
        QSqlDatabase a_StaticDatabase, a_DataDatabase;

    public:
        enum class RemoteSelect { STATIC, DATA };

        RemoteDB();
        ~RemoteDB();
        void initialize();
        void open();
        bool isOpen();
        void close();
        bool isClose();
        void checkDBState();
        QSqlDatabase get(const RemoteSelect& selection);
        void insertTest(const uint testID, const QString& standard, const QString& material, const QString& specification, const uint lenTotal, const uint lenFree, const uint diamNom, const uint diamReal, const uint thickness, const QString& testType, const QString& operatorName, const QString& endCap, const QString& enviroment, const QString& conditionalPeriod, const uint& pressureTarget, const uint& temperatureTarget, const QString &createdAt, const QString &description);
        void insertData(const uint &testID, const double &pressure, const double &temperature, const double &ambient, const QString &createdAt);
    };

    class CacheDB {
        QSqlDatabase a_cacheDB;
        void stationIsPopulated(const uint ID_Station);

    public:
        CacheDB();
        ~CacheDB();
        void initialize();
        void open();
        bool isOpen();
        void close();
        bool isClose();
        QSqlDatabase get();
        void insertData(const uint& ID_Station, const double &pressure, const double &temperature, const double &ambient);
        void updateTest(const uint& testID, const QString& standard, const QString& material, const QString& specification, const uint lenTotal, const uint lenFree, const uint diamNom, const uint diamReal, const uint thickness, const QString& testType, const QString& operatorName, const QString& endCap, const QString& enviroment, const QString& conditionalPeriod, const uint& pressureTarget, const uint& temperatureTarget);
        void failureTest(const uint& ID_Station, const QString& description);
        void deleteTest(const uint& testID);
        void StopStation(const uint& ID_Station);
        void StopByStandby(const uint& ID_Station);
    };

    uint timeoutTest; // In Hours
    RemoteDB a_RemoteDB;
    CacheDB  a_CacheDB;

public:
    enum class Status { OPEN, CLOSE };

    Manager();
    ~Manager();
    void initialize();
    void open();
    bool isOpen();
    void close();
    void isClose();
    uint stationIsActive(const uint station_ID);
    void stationsFreedom();
    QSqlQuery selectTest(const QString& myQuery, const QString& dbName);

    static QSharedPointer<Manager> myDatabases;

    static void loadConfiguration(QSqlDatabase& myDB);
    static bool test(QSqlDatabase testDB);
    static bool test(const QString hostname, const uint port, const QString username, const QString password);
    static void save(const QSqlDatabase myDB);
    static void save(const QString hostname, const uint port, const QString username, const QString password);
    static void CacheToRemote(RemoteDB& remote, CacheDB& cache);

    Q_SIGNAL void DatabaseInitialize(const Manager::Status& v_Status,  const QString& v_Error);
    Q_SIGNAL void DatabaseConnection(const Manager::Status& v_Status,  const QString& v_Error);

public slots:
    void updateTest(const uint testID, const QString& standard, const QString& material, const QString& specification, const uint lenTotal, const uint lenFree, const uint diamNom, const uint diamReal, const uint thickness, const QString& testType, const QString& operatorName, const QString& endCap, const QString& enviroment, const QString& conditionalPeriod, const uint& pressureTarget, const uint& temperatureTarget);
    void failureTest(const uint testID, const QString& description);
    void insertData(const uint &ID_Station,  const double &pressure, const double &temperature, const double &ambient);
    void deleteTest(const uint testID);
    void unlinkStationTest(const uint station_id);
    void exportTestData(const uint &testID);
    void testTimeoutTime(const uint& timeoutTime);
};

namespace ManagerErrors {
    class ConnectionError : public QException {
        QString error;
    public:
        ConnectionError(QString error) { this->error = error; }
        const QString what() { return this->error; }
        void raise() const override { throw *this; }
        ConnectionError *clone() const override { return new ConnectionError(*this); }
    };

    class ConfigurationError : public QException {
        QString error;
    public:
        ConfigurationError(QString error) { this->error = error; }
        const QString what() { return this->error; }
        void raise() const override { throw *this; }
        ConfigurationError *clone() const override { return new ConfigurationError(*this); }
    };

    class QuerySelectError : public QException {
        QString error;
    public:
        QuerySelectError(QString error) { this->error = error; }
        const QString what() { return this->error; }
        void raise() const override { throw *this; }
        QuerySelectError *clone() const override { return new QuerySelectError(*this); }
    };

    class QueryInsertError : public QException {
        QString error;
    public:
        QueryInsertError(QString error) { this->error = error; }
        const QString what() { return this->error; }
        void raise() const override { throw *this; }
        QueryInsertError *clone() const override { return new QueryInsertError(*this); }
    };

    class QueryUpdateError : public QException {
        QString error;
    public:
        QueryUpdateError(QString error) { this->error = error; }
        const QString what() { return this->error; }
        void raise() const override { throw *this; }
        QueryUpdateError *clone() const override { return new QueryUpdateError(*this); }
    };

    class QueryDeleteError : public QException {
        QString error;
    public:
        QueryDeleteError(QString error) { this->error = error; }
        const QString what() { return this->error; }
        void raise() const override { throw *this; }
        QueryDeleteError *clone() const override { return new QueryDeleteError(*this); }
    };
}
#endif // DATABASE_H
