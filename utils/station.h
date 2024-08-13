#ifndef STATION_H
#define STATION_H
#include <QTimer>
#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QDateTime>
#include <QException>
#include <QPushButton>
#include "../services/pressuretempgraph.h"

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

    // void changeBtnsVisibility(const bool state);

public:
    Station();
    ~Station();
    void reloadTestParameters(const uint testID, const QDateTime started);
    uint getID();
    void setTestID(const uint testID);
    uint getTestID() const;
    // void reloadPlotSettings();
    void clear();
    void refresh(double pressure, double temperature, double ambient);
    void hasStarted();
    void hasStoped();
    void setStatus(const Station::Status status);
    QDateTime getTimer();
    void setTimer(const QDateTime timer);
    Status getStatus() const;

    static void checkErrorCode(const int codeError, const uint a_ID);

    Q_SIGNAL void statusChanged(const Station::Status& myStatus);
    Q_SIGNAL void labelsUpdate(const uint v_key, const double v_pressure, const double v_temperature);
    Q_SIGNAL void plotNewPoint(const uint v_key, const double v_pressure, const double v_temperature);
    Q_SIGNAL void hoopErrorCode(const int codeError);
};

namespace StationError {
    enum class errorCodes { eInitPressureLoad = 1, ePressureLoose, eRecurrentPressureLoad };

    class InitPressureLoad : public QException {
        QString v_errMsg;
    public:
        InitPressureLoad(const uint station_ID) {
            this->v_errMsg = QString("Se produjo una falla en la estación al realizar la carga de presión inicial");
        }
        const QString what() { return this->v_errMsg; }
        void raise() const override { throw *this; }
        InitPressureLoad *clone() const override { return new InitPressureLoad(*this); }
    };

    class PressureLoose : public QException {
        QString v_errMsg;
    public:
        PressureLoose(const uint station_ID) {
            this->v_errMsg = QString("Se produjo una falla en la estación debido a una caída abrupta de presión");
        }
        const QString what() { return this->v_errMsg; }
        void raise() const override { throw *this; }
        PressureLoose *clone() const override { return new PressureLoose(*this); }
    };

    class RecurrentPressureLoad : public QException {
        QString v_errMsg;
    public:
        RecurrentPressureLoad(const uint station_ID) {
            this->v_errMsg = QString("Se produjo una falla en la estación por recargas recurrentes de presión");
        }
        const QString what() { return this->v_errMsg; }
        void raise() const override { throw *this; }
        RecurrentPressureLoad *clone() const override { return new RecurrentPressureLoad(*this); }
    };
}
#endif // STATION_H
