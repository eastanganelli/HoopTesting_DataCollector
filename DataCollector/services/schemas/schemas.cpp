#include "defines.h"
#include "services/database.h"
#include <QSqlError>
#include "schemas.h"

Schemas::Data::Data(QLabel *dbstatus, QAction *dbconnect) {
    this->lblConnectDB = dbconnect;
    this->lblConnectionStatus = dbstatus;
    this->myDB = QSqlDatabase::addDatabase("QMYSQL", DATA_SCHEMA_NAME);
    this->myDB.setHostName("");
    this->myDB.setPort(0);
    this->myDB.setUserName("");
    this->myDB.setPassword("");
    this->myDB.setDatabaseName(DATA_DB_NAME);

    try {
        this->loadConfiguration();
    }
    catch(...) {}
}

Schemas::Data::~Data() { this->close(); }

void Schemas::Data::changingLblConnectionState(const QString state_, const QString color_) {
    this->lblConnectionStatus->setText("Base de Datos: " + state_);
    this->lblConnectionStatus->setStyleSheet("color:" + color_ +";");
}

void Schemas::Data::loadConfiguration() {
    DatabaseManager::loadConfiguration(this->myDB);
}

void Schemas::Data::open() {
    try {
        this->loadConfiguration();
        this->prevState = !this->myDB.open();
        qDebug() << this->myDB.lastError().text();
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
    QSqlDatabase::removeDatabase(DATA_SCHEMA_NAME);
    this->lblConnectDB->setText("Conectar");
}

bool Schemas::Data::isOpen() { return this->myDB.isOpen(); }

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

Schemas::Static::Static() { this->myDB.setDatabaseName("static_db"); }

Schemas::Static::~Static() { this->close(); }

bool Schemas::Static::open() {
    bool isOpen = false;
    try {
        this->myDB = QSqlDatabase::addDatabase("QMYSQL", STATIC_SCHEMA_NAME);
        this->myDB.setDatabaseName(STATIC_DB_NAME);
        DatabaseManager::loadConfiguration(this->myDB);
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
    QSqlDatabase::removeDatabase(STATIC_SCHEMA_NAME);
}
