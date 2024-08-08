#include "setdb.h"
#include "ui_setdb.h"

DBConfig::DBConfig(QWidget *parent) : QDialog(parent), ui(new Ui::DBConfig) {
    ui->setupUi(this);

    {
        connect(this->ui->inputHostname, SIGNAL(textChanged(QString)), this, SLOT(dataIsComplete()));
        connect(this->ui->inputUser,     SIGNAL(textChanged(QString)), this, SLOT(dataIsComplete()));
        connect(this->ui->inputPassword, SIGNAL(textChanged(QString)), this, SLOT(dataIsComplete()));
    }
    this->loadSave();
}

DBConfig::~DBConfig() { delete ui; }

void DBConfig::on_btnSave_clicked() {
    QMessageBox msgBox(QMessageBox::Warning, "Configuración", "Desea guardar cambios?");
    msgBox.addButton(QMessageBox::Yes)->setText(tr("Si"));
    msgBox.addButton(QMessageBox::No)->setText(tr("No"));

    switch(msgBox.exec()) {
        case QMessageBox::Yes : {
            Manager::save(this->myDB);
            this->close();
            break;
        }
        default: break;
    }
}

void DBConfig::loadSave() {
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("DBConfig");
    const QString hostName = mySettings.value("hostname", QString()).toString(),
        userName = mySettings.value("username", QString()).toString();
    const uint port = mySettings.value("port", QString()).toUInt();
    mySettings.endGroup();

    if(hostName != "" && userName != "" && port != 0) {
        this->ui->inputHostname->setText(hostName);
        this->ui->inputPort->setValue(port);
        this->ui->inputUser->setText(userName);
    }
}

void DBConfig::on_btnDBTest_clicked() {
    this->myDB = QSqlDatabase::addDatabase("QMYSQL", "testDB");
    this->myDB.setHostName(this->ui->inputHostname->text());
    this->myDB.setPort(this->ui->inputPort->value());
    this->myDB.setUserName(this->ui->inputUser->text());
    this->myDB.setPassword(this->ui->inputPassword->text());

    const bool state = Manager::test(this->myDB);
    const QString response = state ? "Prueba exitosa" : "Falló la prueba";
    QMessageBox msgBox(QMessageBox::Information, "Prueba Conexión", response, QMessageBox::Ok);
    msgBox.exec();
}

void DBConfig::on_btnCancel_clicked() { this->close(); }

void DBConfig::dataIsComplete() {
    if(!this->ui->inputHostname->text().isEmpty() && !this->ui->inputPassword->text().isEmpty() && !this->ui->inputUser->text().isEmpty()) {
        this->ui->btnDBTest->setEnabled(true);
        this->ui->btnSave->setEnabled(true);
        {
            this->myDB.setHostName(this->ui->inputHostname->text());
            this->myDB.setPort(this->ui->inputPort->value());
            this->myDB.setUserName(this->ui->inputUser->text());
            this->myDB.setPassword(this->ui->inputPassword->text());
        }
    } else {
        this->ui->btnDBTest->setEnabled(false);
        this->ui->btnSave->setEnabled(false);
    }
}
