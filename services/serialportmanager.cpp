#include "serialportmanager.h"
#include "../services/database.h"

class DataVisualizerWindow;

QQueue<QString> SerialPortReader::portMessages = QQueue<QString>();

SerialPortReader::SerialPortReader() {
    this->initialize();
}

SerialPortReader::SerialPortReader(const QString portName, const QString baudRate) {
    this->initialize();
    this->setPortName(portName);
    this->setBaudRate(baudRate.toUInt());
}

void SerialPortReader::initialize() {
    this->setDataBits(QSerialPort::Data8);
    this->setParity(QSerialPort::NoParity);
    this->setStopBits(QSerialPort::OneStop);
    this->connectionState = false;
    this->mSerialTimer    = QSharedPointer<QTimer>(new QTimer(this));
    // connect(this->mSerialTimer.get(), &QTimer::timeout, this, &SerialPortReader::onSerialPortReadyRead);
    connect(this->mSerialTimer.data(), &QTimer::timeout, this, &SerialPortReader::onTimeout);
    connect(this, &QSerialPort::readyRead, this, &SerialPortReader::onSerialPortReadyRead);
    this->mSerialTimer->setSingleShot(true);

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
    // const auto bytesReceived = this->bytesAvailable();
    this->mSerialTimer->start(timeoutConnection);
    QByteArray data = this->readAll();
    auto statusCom = [&](const QByteArray& data, const QTime& timeStatus ) -> void {
        if(timeStatus.second() >= timeoutConnection && data.isEmpty()) {
            emit this->CheckSerialPort(SerialPortReader::Status::INACTIVE);
            this->connectionState = false;
        } else if(timeStatus.second() < timeoutConnection && !data.isEmpty()) {
            emit this->CheckSerialPort(SerialPortReader::Status::ACTIVE);
            this->connectionState = true;
        }
    };

    statusCom(data, this->timeStatus);
    if (!data.isEmpty()) {/*
        this->timeStatus = QTime(0, 0, 0, 0);*/
        this->serialToStation(data);
        this->clear();
    }
    // } else { this->timeStatus = this->timeStatus.addMSecs(ms_); }
    this->autoMessageSender();
}

// bool SerialPortReader::statusPort() {
//     bool state = this->isOpen();
//     if(state && !this->portState)      { this->portState = true; }
//     else if(!state && this->portState) { this->portState = false; }
//     return state && this->connectionState;
// }

void SerialPortReader::sendMessage(const QByteArray &message) {
    this->clear();
    this->write(message);
    this->waitForBytesWritten(-1);
}

void SerialPortReader::openPort() {
    QString serialName;
    uint baudRate;
    SerialPortReader::read(serialName, baudRate);
    if(serialName.isEmpty()) {
        /*
         * Throw error
        */
        // QMessageBox msgBox(QMessageBox::Warning , "Configuración", "Puerto Serial no configurado!");
        // msgBox.exec();
        return;
    }
    this->setBaudRate(baudRate);
    this->setPortName(serialName);
    bool portState = !this->open(QIODevice::ReadWrite);
    if(portState) { return; }
    this->connectionState = false;
    emit this->CheckSerialPort(SerialPortReader::Status::OPEN);
    // this->mSerialTimer->start(ms_);
    return;
}

void SerialPortReader::closePort() {
    this->close();
    if(this->isOpen()) { return; }
    this->mSerialTimer->stop();
    emit this->CheckSerialPort(SerialPortReader::Status::CLOSE);
}

bool SerialPortReader::isActive() { return this->isOpen() && this->connectionState; }

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

void SerialPortReader::onTimeout() {
    qDebug() << "oops, something went wrong";
}

void SerialPortReader::serialToStation(QByteArray& msg) {
    auto Msgs = QString(msg).split(this->serialParsing["Separator"]);
    for(QString& serialMsg : Msgs) {
        if(this->serialParsing["Dummy"].match(serialMsg).hasMatch()) { continue; }
        else if(this->serialParsing["StartPLC"].match(serialMsg).hasMatch()) {
            QRegularExpressionMatch resultMatch = this->serialParsing["StartPLC"].match(serialMsg);
            const uint idStation = resultMatch.captured("station").toInt();
            const auto pressure = resultMatch.captured("pressure"), temperature = resultMatch.captured("temperature"), ambient = resultMatch.captured("ambient");
            if(pressure != "xx.xx" && temperature != "xx.xx" && ambient != "xx.xx") {
                QSharedPointer<Station> myStation =Station::myStations[idStation];
                if(myStation->getTestID() == 0)
                    myStation->hasStarted();
                myStation->refresh(pressure.toDouble(), temperature.toDouble(), ambient.toDouble());
                Manager::myDatabases->insertData(idStation, pressure.toDouble(), temperature.toDouble(), ambient.toDouble());
            }
            SerialPortReader::portMessages.enqueue((QString("start|%1|xx|xx\n").arg(idStation)));
        } else if(this->serialParsing["StopPLC"].match(serialMsg).hasMatch()) {
            QRegularExpressionMatch resultMatch = this->serialParsing["StopPLC"].match(serialMsg);
            try {
                const uint idStation = resultMatch.captured("station").toInt();
                const auto pressure  = resultMatch.captured("pressure"), temperature = resultMatch.captured("temperature"), ambient = resultMatch.captured("ambient");
                QSharedPointer<Station> myStation = Station::myStations[idStation];
                if(pressure != "xx.xx" && temperature != "xx.xx" && ambient != "xx.xx") {
                    myStation->refresh(pressure.toDouble(), temperature.toDouble(), ambient.toDouble());
                    Manager::myDatabases->insertData(idStation, pressure.toDouble(), temperature.toDouble(), ambient.toDouble());
                }
                SerialPortReader::portMessages.enqueue((QString("stop|%1|xx|xx\n").arg(idStation)));
                Manager::myDatabases->unlinkStationTest(idStation);
                myStation->hasStoped();
            } catch(...) {}
        } else if(this->serialParsing["ErrorPLC"].match(serialMsg).hasMatch()) {
            QRegularExpressionMatch resultMatch = this->serialParsing["ErrorPLC"].match(serialMsg);
            const uint idStation = resultMatch.captured("station").toInt();
            QSharedPointer<Station> myStation = Station::myStations[idStation];
            Manager::myDatabases->unlinkStationTest(idStation);
            SerialPortReader::portMessages.enqueue((QString("error|%1|xx|xx\n").arg(idStation)));
            emit myStation->hoopErrorCode(resultMatch.captured("status_code").toInt());
        }
    }
}
