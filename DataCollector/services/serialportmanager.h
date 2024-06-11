#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H
#include <QtSerialPort>
#include <QByteArray>
#include <QSharedPointer>
#include <QStack>
#include <QAction>
#include <QLabel>

#include "utils/station.h"

#define ms_ 10              // In miliseconds
#define timeoutConnection 3 // In seconds

class SerialPortReader : public QSerialPort {
    Q_OBJECT

    void serialToStation();
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
