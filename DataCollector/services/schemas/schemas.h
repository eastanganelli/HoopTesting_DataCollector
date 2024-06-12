#ifndef SCHEMAS_H
#define SCHEMAS_H
#include <QSqlQuery>
#include <QMessageBox>
#include <QSharedPointer>
#include <QString>
#include <QList>
#include <QSettings>
#include <QDateTime>
#include <QLabel>

#define datetime_format "yyyy/MM/dd hh:mm:ss"

#define STATIC_SCHEMA_NAME "Static_Connection"
#define STATIC_DB_NAME "static_db"
#define DATA_SCHEMA_NAME "Data_Connection"
#define DATA_DB_NAME "data_db"

namespace Schemas {
    class Data {
        const QString connectionName;
        QSqlDatabase myDB;
        QLabel*  lblConnectionStatus;
        QAction* lblConnectDB;
        bool prevState;

        void changingLblConnectionState(const QString state_, const QString color_);
        void loadConfiguration();

    public:
        Data(QLabel* dbstatus, QAction* dbconnect);
        ~Data();
        void open();
        void close();
        bool isOpen();
        bool status();
    };

    class Static {
        const QString connectionName;
        QSqlDatabase myDB;

    public:
        Static();
        ~Static();
        bool open();
        void close();
        QSqlQuery exec(QString query);
    };
}

#endif // SCHEMAS_H
