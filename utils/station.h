#ifndef STATION_H
#define STATION_H
#include <QTimer>
#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QDateTime>
#include <QException>
#include <QPushButton>

#define OFFSET_UPPER 1

const QDateTime DEFAULT_DATETIME = QDateTime(QDate(1970, 1, 1), QTime(0,0,0,0));

class Station: public QObject {
    Q_OBJECT

public:
    enum class Status { RUNNING, WAITING, READY };

private:
    const uint ID;
    uint idTest;
    QDateTime timer, started;

    static uint activeStation;

public:
    Station();
    ~Station();
    void reloadTestParameters(const uint testID, const QDateTime started);
    uint getID();
    void setTestID(const uint testID);
    uint getTestID() const;
    Status getStatus() const;
    void setStatus(const Station::Status status);
    QDateTime getTimer();
    void setTimer(const QDateTime timer);
    void clear();
    void refresh(double pressure, double temperature, double ambient);
    void hasStarted();
    void hasStoped();
    void hasStopError(const int &error_Code);

    static QMap<uint, QSharedPointer<Station>> myStations;

    Q_SIGNAL void statusChanged(const Station::Status& myStatus);
    Q_SIGNAL void labelsUpdate(const uint& v_key, const double& v_pressure, const double& v_temperature);
    Q_SIGNAL void plotNewPoint(const uint& v_key, const double& v_pressure, const double& v_temperature);
    Q_SIGNAL void cacheNewData(const uint& ID_Station, const double& v_pressure, const double& v_temperature, const double& v_ambient);
    Q_SIGNAL void cacheErrorTest(const uint& ID, const QString& description);
    Q_SIGNAL void hoopErrorCode(const QString& errMsg);
};

namespace StationError {
    enum class errorCodes { eInitPressureLoad = 1, ePressureLoose, eRecurrentPressureLoad };

    QString checkErrorCode(const int &codeError, const uint &a_ID);

    class InitPressureLoad : public QException {
        QString v_errMsg;
    public:
        InitPressureLoad(const uint station_ID) {
            this->v_errMsg = QString("Se produjo una falla en la estación %1 al realizar la carga de presión inicial").arg(QString::number(station_ID));
        }
        const QString what() { return this->v_errMsg; }
        void raise() const override { throw *this; }
        InitPressureLoad *clone() const override { return new InitPressureLoad(*this); }
    };

    class PressureLoose : public QException {
        QString v_errMsg;
    public:
        PressureLoose(const uint station_ID) {
            this->v_errMsg = QString("Se produjo una falla en la estación %1 debido a una caída abrupta de presión").arg(QString::number(station_ID));
        }
        const QString what() { return this->v_errMsg; }
        void raise() const override { throw *this; }
        PressureLoose *clone() const override { return new PressureLoose(*this); }
    };

    class RecurrentPressureLoad : public QException {
        QString v_errMsg;
    public:
        RecurrentPressureLoad(const uint station_ID) {
            this->v_errMsg = QString("Se produjo una falla en la estación %1 por recargas recurrentes de presión").arg(QString::number(station_ID));
        }
        const QString what() { return this->v_errMsg; }
        void raise() const override { throw *this; }
        RecurrentPressureLoad *clone() const override { return new RecurrentPressureLoad(*this); }
    };
}
#endif // STATION_H
