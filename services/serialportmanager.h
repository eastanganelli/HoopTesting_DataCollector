#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H
#include <QMap>
#include <QLabel>
#include <QQueue>
#include <QStack>
#include <QAction>
#include <QByteArray>
#include <QSerialPort>
#include <QSharedPointer>
#include <QRegularExpression>

#include "../utils/station.h"


#define timeoutConnection 15000 // In miliseconds
// #define ms_ 250                // In miliseconds

class SerialPortReader : public QSerialPort {
    Q_OBJECT
    // https://forum.qt.io/topic/139144/running-just-one-slot-in-a-different-thread/11
    void initialize();
    void serialToStation(QByteArray& data);
    void sendMessage(const QByteArray& message);
    void autoMessageSender();
    void stationStop(QSharedPointer<Station> auxStation);

    QSharedPointer<QTimer> mSerialTimer;
    QByteArray buffer;
    bool connectionState;
    QTime timeStatus;
    QMap<QString, QRegularExpression> serialParsing;

    static QQueue<QString> portMessages;

private slots:
    void onTimeout();
    void onSerialPortReadyRead();

public:
    enum class Status { OPEN, CLOSE, ACTIVE, INACTIVE };

    SerialPortReader();
    SerialPortReader(const QString portName, const QString baudRate);

    static bool test(const QString portName, const QString baudRate);
    static bool test(SerialPortReader testPort);
    static void save(const QString portName, const QString baudRate);
    static void save(const SerialPortReader myPort);
    static void read(QString& serialName, uint& baudRate);

    void openPort();
    void closePort();
    bool isActive();
    // bool statusPort();

    Q_SIGNAL void CheckSerialPort(const SerialPortReader::Status& Status);
};
#endif // SERIALPORTMANAGER_H
