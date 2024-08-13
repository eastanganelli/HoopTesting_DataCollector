#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H
#include <QMap>
#include <QLabel>
#include <QStack>
#include <QAction>
#include <QtConcurrent/QtConcurrent>
#include <QByteArray>
#include <QSerialPort>
#include <QSharedPointer>
#include <QRegularExpression>

#include "../utils/station.h"

#define timeoutConnection 30 // In seconds
#define ms_ 250                // In miliseconds

class SerialPortReader : public QSerialPort {
    Q_OBJECT

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

public slots:
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
