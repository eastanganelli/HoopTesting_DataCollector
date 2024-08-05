#include "setserial.h"
#include "ui_setserial.h"
#include "../services/serialportmanager.h"

serialConfig::serialConfig(QWidget *parent) : QDialog(parent), ui(new Ui::serialConfig) {
    ui->setupUi(this);
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts()) { if(SerialPortReader::test(port.portName(), QString::number(9600))) { this->ui->cboxSerialSelector->addItem(port.portName()); } }
    Q_FOREACH(qint32 baudRate, QSerialPortInfo::standardBaudRates())   { this->ui->cboxBaudRate->addItem(QString::number(baudRate)); }
    this->loadSaveData();


}

serialConfig::~serialConfig() { delete ui; }

void serialConfig::loadSaveData() {
    QString portName;
    uint baudRate;
    SerialPortReader::read(portName, baudRate);
    if(!portName.isEmpty() && baudRate != 0) {
        this->ui->cboxSerialSelector->addItem(portName);
        this->ui->cboxBaudRate->setCurrentText(QString::number(baudRate));
    }
}

void serialConfig::on_btnTest_clicked() {
    QMessageBox msg;
    msg.setWindowTitle("Puerto Serial: Prueba");
    msg.setIcon(QMessageBox::Information);
    msg.setWindowIcon(QIcon(":icon/logo"));
    if(SerialPortReader::test(this->ui->cboxSerialSelector->currentText(), this->ui->cboxBaudRate->currentText())) { msg.setText("Esta disponible!"); }
    else { msg.setText("No esta disponible!"); }
    msg.exec();
}

void serialConfig::on_btnSave_clicked()   { if(!this->ui->cboxSerialSelector->currentText().isEmpty()) { SerialPortReader::save(this->ui->cboxSerialSelector->currentText(), this->ui->cboxBaudRate->currentText()); this->close(); } }

void serialConfig::on_btnCancel_clicked() { this->close(); }

