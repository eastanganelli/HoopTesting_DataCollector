#include "../defines.h"
#include "global.h"
#include "serialportmanager.h"
#include "../components/datavisualizer.h"

class DataVisualizerWindow;

SerialPortReader::SerialPortReader(const QString portName, const QString baudRate) {
    this->initialize();
    this->setPortName(portName);
    this->setBaudRate(baudRate.toUInt());
    this->lblConnectionStatus = nullptr;
    this->lblPortStatus       = nullptr;
    this->btnConnect          = nullptr;
}

SerialPortReader::SerialPortReader(QLabel *portStatus, QLabel *connectionStatus, QAction *acConnection) {
    this->initialize();
    this->lblConnectionStatus = connectionStatus;
    this->lblPortStatus       = portStatus;
    this->btnConnect          = acConnection;
}

SerialPortReader::SerialPortReader(QSerialPort* serialPortPort, QLabel *portStatus, QLabel *connectionStatus, QAction *acConnection) : QSerialPort(serialPortPort) {
    this->initialize();
    this->lblConnectionStatus = connectionStatus;
    this->lblPortStatus       = portStatus;
    this->btnConnect          = acConnection;
}

void SerialPortReader::initialize() {
    this->setDataBits(QSerialPort::Data8);
    this->setParity(QSerialPort::NoParity);
    this->setStopBits(QSerialPort::OneStop);
    this->timeStatus = QTime(0, 0, 12, 0);
    this->connectionState = false;
    this->mSerialTimer    = QSharedPointer<QTimer>(new QTimer());
    connect(this->mSerialTimer.get(), &QTimer::timeout, this, &SerialPortReader::onSerialPortReadyRead);

    this->serialParsing.insert("Dummy",    QRegularExpression("^[$]\\/n\\/r"));
    this->serialParsing.insert("StartPLC", QRegularExpression("^#(?<station>[1-6])\\|(?<state>Run)\\|(?<pressure>\\d{1,3}.\\d{1,4})\\|(?<timediff>\\d{1,2}[0-9]:\\d{1,2}[0-9])\\|(?<temperature>\\d{1,2}.\\d{1,2})\\|(?<ambient>\\d{1,2}.\\d{1,2})\\/n\\/r"));
    this->serialParsing.insert("StopPLC",  QRegularExpression("^#(?<station>[1-6])\\|(?<state>Stop)\\|(?<pressure>\\d{1,3}.\\d{1,4})\\|(?<timediff>\\d{1,2}[0-9]:\\d{1,2}[0-9])\\|(?<temperature>\\d{1,2}.\\d{1,2})\\|(?<ambient>\\d{1,2}.\\d{1,2})\\/n\\/r"));
    this->serialParsing.insert("ErrorPLC", QRegularExpression("^!(?<station>[1-6])\\|(?<state>Run)\\|ERR:(?<code_err>[1-6](?<status_code>[1-3]))"));
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
        this->serialToStation(data);
        this->clear();
    } else { this->timeStatus = this->timeStatus.addMSecs(ms_); }
    this->autoMessageSender();
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

bool SerialPortReader::openPort() {
    QString serialName;
    uint baudRate;
    this->portState = false;
    SerialPortReader::read(serialName, baudRate);
    if(serialName.isEmpty()) {
        QMessageBox msgBox(QMessageBox::Warning , "Configuración", "Puerto Serial no configurado!");
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

void SerialPortReader::serialToStation(QByteArray& msg) {
    if(this->serialParsing["Dummy"].match(msg).hasMatch()) {
        qDebug() << "Dummy";
        return;
    }
    else if(this->serialParsing["StartPLC"].match(msg).hasMatch()) {
        QRegularExpressionMatch resultMatch = this->serialParsing["StartPLC"].match(msg);
        // myData.setStartPLC(msg);
        // qDebug() << (QString("Station[%1] -> Pressure[%2] -> Time[%3] -> Temp[%4] -> Humidity[%5]")
        //              .arg(resultMatch.captured("station"))
        //              .arg(resultMatch.captured("pressure"))
        //              .arg(resultMatch.captured("timediff"))
        //              .arg(resultMatch.captured("temperature"))
        //              .arg(resultMatch.captured("ambient")));
        QSharedPointer<Station> myStation = DataVisualizerWindow::myStations[resultMatch.captured("station").toInt()];
        myStation->refresh(resultMatch.captured("pressure").toFloat(), resultMatch.captured("temperature").toFloat(), resultMatch.captured("ambient").toFloat());
        return;
    } else if(this->serialParsing["StopPLC"].match(msg).hasMatch()) {
        QRegularExpressionMatch resultMatch = this->serialParsing["StopPLC"].match(msg);
        // myData.setStopPLC(msg);
        qDebug() << (QString("Station[%1] -> Pressure[%2] -> Time[%3] -> Temp[%4] -> Humidity[%5]")
                         .arg(resultMatch.captured("station"))
                         .arg(resultMatch.captured("pressure"))
                         .arg(resultMatch.captured("timediff"))
                         .arg(resultMatch.captured("temperature"))
                         .arg(resultMatch.captured("ambient")));
        return;
    } else if(this->serialParsing["ErrorPLC"].match(msg).hasMatch()) {
        QRegularExpressionMatch resultMatch = this->serialParsing["ErrorPLC"].match(msg);
        // myData.setError
        qDebug() << (QString("Station[%1] -> Error Code: %2")
                         .arg(resultMatch.captured("station"))
                         .arg(resultMatch.captured("status_code")));
        return;
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
