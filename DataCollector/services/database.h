#ifndef DATABASE_H
#define DATABASE_H
#include <QApplication>
#include <QSettings>
#include <QSqlDatabase>
#include <QString>
#include <QException>
#include <QString>

#define datetime_format "yyyy/MM/dd hh:mm:ss"

class DatabaseManager {
public:
    static void loadConfiguration(QSqlDatabase& myDB);
    static bool test(const QString hostname, const uint port, const QString username, const QString password);
    static bool test(QSqlDatabase testDB);
    static void save(const QString hostname, const uint port, const QString username, const QString password);
    static void save(const QSqlDatabase myDB);
};

namespace DatabaseError {
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
