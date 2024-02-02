#include "setserial.controller.h"
#include "ui_setserial.controller.h"

serialConfig::serialConfig(QWidget *parent) : QDialog(parent), ui(new Ui::serialConfig) {
    ui->setupUi(this);
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
        if(SerialPortReader::test(port.portName(), QString::number(9600))) {
            this->ui->cboxSerialSelector->addItem(port.portName());
        }
    }
    Q_FOREACH(qint32 baudRate, QSerialPortInfo::standardBaudRates()) {
        this->ui->cboxBaudRate->addItem(QString::number(baudRate));
    }
}

serialConfig::~serialConfig() { delete ui; }

void serialConfig::on_buttonBox_accepted() {
    if(!this->ui->cboxSerialSelector->currentText().isEmpty()) {
        SerialPortReader::save(this->ui->cboxSerialSelector->currentText(), this->ui->cboxBaudRate->currentText());
    }
}

void serialConfig::on_btnCheckSerial_clicked() {
    QMessageBox msg;
    msg.setWindowTitle("Prueba Puerto");
    msg.setIcon(QMessageBox::Information);
    if(SerialPortReader::test(this->ui->cboxSerialSelector->currentText(), this->ui->cboxBaudRate->currentText())) { msg.setText("El puerto esta disponible"); }
    else { msg.setText("El puerto no esta disponible"); }
    msg.exec();
}
