#include <QThread>
#include "serialportmanager.h"
#include "../defines.h"
#include "../components/datavisualizer.h"

class DataVisualizerWindow;

QQueue<QString> SerialPortReader::portMessages = QQueue<QString>();

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

    this->serialParsing.insert("Separator",QRegularExpression("(?=[$!#])"));
    this->serialParsing.insert("Dummy",    QRegularExpression("^[$]\\/n\\/r"));
    this->serialParsing.insert("StartPLC", QRegularExpression("^#(?<station>[1-6])\\|(?<state>Run)\\|(?<pressure>\\d{1,3}.\\d{1,4})\\|(?<timediff>\\d{1,2}[0-9]:\\d{1,2}[0-9])\\|(?<temperature>\\d{1,2}.\\d{1,2})\\|(?<ambient>\\d{1,2}.\\d{1,2})\\/n\\/r"));
    this->serialParsing.insert("StopPLC",  QRegularExpression("^#(?<station>[1-6])\\|(?<state>Stop)\\|(?<pressure>\\d{1,3}.\\d{1,4}|xx.xx)\\|(?<timediff>\\d{1,2}[0-9]:\\d{1,2}[0-9]|xx:xx)\\|(?<temperature>\\d{1,2}.\\d{1,2}|xx.xx)\\|(?<ambient>\\d{1,2}.\\d{1,2}|xx.xx)\\/n\\/r"));
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
    this->status(data);
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
        this->changingLblPortState("Conectado", StatusGreen);
        this->portState = true;
    }
    else if(!state && this->portState) {
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
            this->changingLblConnectionState("Cerrado", StatusRed);
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
    while(!(SerialPortReader::portMessages.isEmpty())) {
        this->sendMessage((SerialPortReader::portMessages.dequeue()).toUtf8());
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
    auto Msgs = QString(msg).split(this->serialParsing["Separator"]);
    for(QString& serialMsg : Msgs) {
        if(this->serialParsing["Dummy"].match(serialMsg).hasMatch()) {
            continue;
        }
        else if(this->serialParsing["StartPLC"].match(serialMsg).hasMatch()) {
            QRegularExpressionMatch resultMatch = this->serialParsing["StartPLC"].match(serialMsg);
            DataVisualizerWindow::myStations[resultMatch.captured("station").toInt()]->refresh(resultMatch.captured("pressure").toDouble(), resultMatch.captured("temperature").toDouble(), resultMatch.captured("ambient").toDouble());
            SerialPortReader::portMessages.enqueue((QString("start|%1|xx|xx\n").arg(resultMatch.captured("station").toInt())));
        } else if(this->serialParsing["StopPLC"].match(serialMsg).hasMatch()) {
            QRegularExpressionMatch resultMatch = this->serialParsing["StopPLC"].match(serialMsg);
            try {
                DataVisualizerWindow::myStations[resultMatch.captured("station").toInt()]->refresh(resultMatch.captured("pressure").toDouble(), resultMatch.captured("temperature").toDouble(), resultMatch.captured("ambient").toDouble());
            } catch(...) {}
            SerialPortReader::portMessages.enqueue((QString("stop|%1|xx|xx\n").arg(resultMatch.captured("station").toInt())));
        } else if(this->serialParsing["ErrorPLC"].match(serialMsg).hasMatch()) {
            QRegularExpressionMatch resultMatch = this->serialParsing["ErrorPLC"].match(serialMsg);
            DataVisualizerWindow::myStations[resultMatch.captured("station").toInt()]->hoopErrorCode(resultMatch.captured("status_code").toInt());
        }

    }
}

void SerialPortReader::status(const QByteArray& data) {
    if(this->timeStatus.second() >= timeoutConnection && data.isEmpty()/*this->connectionState*/) {
        this->connectionState = false;
        this->changingLblConnectionState("Cerrado", StatusRed);
    } else if(this->timeStatus.second() < timeoutConnection && !data.isEmpty()/* && !this->connectionState*/) {
        this->connectionState = true;
        this->changingLblConnectionState("Abierto", StatusGreen);
    }
}
