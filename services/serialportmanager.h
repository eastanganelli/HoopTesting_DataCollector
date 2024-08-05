#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H
#include <QSerialPort>
#include <QByteArray>
#include <QSharedPointer>
#include <QRegularExpression>
#include <QStack>
#include <QAction>
#include <QLabel>
#include <QMap>

#include "../utils/station.h"

#define timeoutConnection 30 // In seconds
#define ms_ 250                // In miliseconds

class SerialPortReader : public QSerialPort {
    Q_OBJECT

    void initialize();
    void serialToStation(QByteArray& data);
    void sendMessage(const QByteArray& message);
    void autoMessageSender();
    void changingLblConnectionState(const QString state_, const QString color_);
    void changingLblPortState(const QString state_, const QString color_);
    void status();
    void stationStop(QSharedPointer<Station> auxStation);

    QSharedPointer<QTimer> mSerialTimer;
    QByteArray buffer;
    bool portState, connectionState;
    QLabel* lblConnectionStatus,
          * lblPortStatus;
    QAction* btnConnect;
    QTime timeStatus;
    QMap<QString, QRegularExpression> serialParsing;

    static QQueue<QString> portMessages;

public slots:
    void onSerialPortReadyRead();

public:
    SerialPortReader(const QString portName, const QString baudRate);
    SerialPortReader(QLabel* portStatus, QLabel* connectionStatus, QAction* acConnection);
    SerialPortReader(QSerialPort *serialPort, QLabel* portStatus, QLabel* connectionStatus, QAction* acConnection);

    static bool test(const QString portName, const QString baudRate);
    static bool test(SerialPortReader testPort);
    static void save(const QString portName, const QString baudRate);
    static void save(const SerialPortReader myPort);
    static void read(QString& serialName, uint& baudRate);

    bool openPort();
    bool closePort();
    bool statusPort();
};
#endif // SERIALPORTMANAGER_H

// QQueue<QString> SerialPortReader::portMessages = QQueue<QString>();
