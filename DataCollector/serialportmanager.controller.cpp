#include "serialportmanager.controller.h"
#include "defines.h"
#include "global.service.h"

SerialPortReader::SerialPortReader(const QString portName, const QString baudRate) {
    this->setDataBits(QSerialPort::Data8);
    this->setParity(QSerialPort::NoParity);
    this->setStopBits(QSerialPort::OneStop);
    this->timeStatus = QTime(0, 0, 12, 0);
    this->setPortName(portName);
    this->setBaudRate(baudRate.toUInt());
    this->lblConnectionStatus = nullptr;
    this->lblPortStatus       = nullptr;
    this->btnConnect          = nullptr;
    this->connectionState     = false;
    this->mSerialTimer        = QSharedPointer<QTimer>(new QTimer());
    connect(this->mSerialTimer.get(), &QTimer::timeout, this, &SerialPortReader::onSerialPortReadyRead);
}

SerialPortReader::SerialPortReader(QLabel *portStatus, QLabel *connectionStatus, QAction *acConnection) {
    this->setDataBits(QSerialPort::Data8);
    this->setParity(QSerialPort::NoParity);
    this->setStopBits(QSerialPort::OneStop);
    this->timeStatus = QTime(0, 0, 12, 0);
    this->lblConnectionStatus = connectionStatus;
    this->lblPortStatus       = portStatus;
    this->btnConnect          = acConnection;
    this->connectionState     = false;
    this->mSerialTimer        = QSharedPointer<QTimer>(new QTimer());
    connect(this->mSerialTimer.get(), &QTimer::timeout, this, &SerialPortReader::onSerialPortReadyRead);
}

SerialPortReader::SerialPortReader(QSerialPort* serialPortPort, QLabel *portStatus, QLabel *connectionStatus, QAction *acConnection) : QSerialPort(serialPortPort) {
    this->lblConnectionStatus = connectionStatus;
    this->lblPortStatus       = portStatus;
    this->btnConnect          = acConnection;
    this->connectionState     = false;
    this->mSerialTimer        = QSharedPointer<QTimer>(new QTimer());
    connect(this->mSerialTimer.get(), &QTimer::timeout, this, &SerialPortReader::onSerialPortReadyRead);
}

void SerialPortReader::save(const QString portName, const QString baudRate) {
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("SerialConfig"); {
        mySettings.setValue("portName", portName);
        mySettings.setValue("baudRate", baudRate);
    }
    mySettings.endGroup();
}

void SerialPortReader::save(const SerialPortReader myPort) {
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("SerialConfig"); {
        mySettings.setValue("portName", myPort.portName());
        mySettings.setValue("baudRate", QString::number(myPort.baudRate()));
    }
    mySettings.endGroup();
}

void SerialPortReader::read(QString& serialName, uint& baudRate) {
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("SerialConfig");{
        serialName = mySettings.value("portName", QString()).toString();
        baudRate   = mySettings.value("baudRate", QString()).toUInt();
    }
    mySettings.endGroup();
}

void SerialPortReader::onSerialPortReadyRead() {
    QByteArray data = this->readAll();
    this->status(/*data*/);
    if (!data.isEmpty()) {
        this->timeStatus = QTime(0, 0, 0, 0);
        this->buffer.append(data);
        this->serialToStation();
        this->clear();
    } else { this->timeStatus = this->timeStatus.addMSecs(ms_); }
    this->autoMessageSender();
    this->buffer.clear();
}

bool SerialPortReader::statusPort() {
    bool state = this->isOpen();
    if(state && !this->portState) {
    #if CONSOLEDEBUGMODE == ConsoleConsoleDebugOn
        qDebug() << "Puerto: Conectado";
    #endif
        this->changingLblPortState("Conectado", StatusGreen);
        this->portState = true;
    }
    else if(!state && this->portState) {
        #if CONSOLEDEBUGMODE == ConsoleConsoleDebugOn
            qDebug() << "Puerto: Desconectado";
            qDebug() << "Comunicación: Cerrado";
        #endif
        this->changingLblPortState("Desconectado",  StatusRed);
        this->changingLblConnectionState("Cerrado", StatusRed);
        this->portState = false;
    }
    return state && this->connectionState;
}

void SerialPortReader::sendMessage(const QByteArray &message) {
    this->clear();
    this->write(message);
    this->waitForBytesWritten(-1);
}

void SerialPortReader::stationStop(QSharedPointer<Station> auxStation) { auxStation->stop(); }

bool SerialPortReader::openPort() {
    QString serialName;
    uint baudRate;
    this->portState = false;
    SerialPortReader::read(serialName, baudRate);
    if(serialName.isEmpty()) {
        QMessageBox msgBox(QMessageBox::Warning , "Puerto Serial - No Configurado", "Configurar el puerto!");
        msgBox.exec();
        this->btnConnect->setText("Conectar");
    } else {
        this->setBaudRate(baudRate);
        this->setPortName(serialName);
        this->portState = !this->open(QIODevice::ReadWrite);
        if(!this->portState) {
            this->connectionState = false;
            this->btnConnect->setText("Desconectar");
            this->mSerialTimer->start(ms_);
            return true;
        }
    }
    return false;
}

bool SerialPortReader::closePort() {
    this->close();
    if(this->isOpen()) { return false; }
    this->btnConnect->setText("Conectar");
    this->mSerialTimer->stop();
    return true;
}

bool SerialPortReader::test(const QString portName, const QString baudRate) {
    SerialPortReader testPort(portName, baudRate);
    bool state = testPort.open(QSerialPort::ReadWrite);
    if(state) { testPort.close(); }
    return state;
}

bool SerialPortReader::test(SerialPortReader testPort) {
    bool state = testPort.open(QSerialPort::ReadWrite);
    if(state) { testPort.close(); }
    return state;
}

void SerialPortReader::autoMessageSender() {
    QByteArray aux = myData.getMessageSendPort();
    while (!aux.isEmpty()) {
        this->sendMessage(aux);
        aux = myData.getMessageSendPort();
    }
}

void SerialPortReader::changingLblConnectionState(const QString state_, const QString color_) {
    this->lblPortStatus->setText("Comunicación: " + state_);
    this->lblPortStatus->setStyleSheet("color:" + color_ +";");
}

void SerialPortReader::changingLblPortState(const QString state_, const QString color_) {
    this->lblConnectionStatus->setText("Puerto: " + state_);
    this->lblConnectionStatus->setStyleSheet("color:" + color_ +";");
}

void SerialPortReader::serialToStation() {
    QList<QByteArray> msgs = this->buffer.split('\n');
    if(msgs.length() < 1) {
        this->buffer.clear();
        return;
    }

    for(QByteArray& msg : msgs) {
        QList<QByteArray> substring = msg.split(',');
        if(substring.length() == 3) {
            QSharedPointer<Station> auxStation = myData.getStation(substring.at(0).toUInt());
            float bar  = substring.at(1).toFloat(),
                  temp = substring.at(2).toFloat();
            if(auxStation->getStatus() == StationStatus::RUNNING && auxStation->updateStatus(bar, temp)) { this->stationStop(auxStation); }
        }
    }
}

void SerialPortReader::status(/*const QByteArray data*/) {
    if(this->timeStatus.second() >= timeoutConnection && this->connectionState) {
        #if CONSOLEDEBUGMODE == ConsoleConsoleDebugOn
            qDebug() << "Comunicación: Cerrado";
        #endif
        this->connectionState = false;
        this->changingLblConnectionState("Cerrado", StatusRed);
    } else if(this->timeStatus.second() < timeoutConnection && !this->connectionState) {
        #if CONSOLEDEBUGMODE == ConsoleConsoleDebugOn
            qDebug() << "Comunicación: Abierta";
        #endif
        this->connectionState = true;
        this->changingLblConnectionState("Abierto", StatusGreen);
    }
}
