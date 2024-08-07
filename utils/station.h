#ifndef STATION_H
#define STATION_H
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QDateTime>
#include <QException>
#include <QPushButton>
#include "../services/pressuretempgraph.h"

#define OFFSET_UPPER 1

const QDateTime DEFAULT_DATETIME = QDateTime(QDate(1970, 1, 1), QTime(0,0,0,0));

enum class StationStatus { WAITING, RUNNING, READY };

class Station {
    const uint ID;

    QLabel* lblPressure, *lblTemperature, *lblTime;
    QPushButton* btnConfig, *btnStartStop;
    PressureTempGraph* graph;

    uint idTest;
    QDateTime timer, started;

    static uint activeStation;

    void checkIfStationPopulated();
    void refreshPlot(const uint key, const double pressure, const double temperature);
    void refreshLabels(const uint key, const double pressure, const double temperature);
    void setHoopParameters();

public:
    Station(QLabel* pressure, QLabel* temperature, QLabel* time, QPushButton* config, QPushButton* startStop, QTabWidget* tabs, PressureTempGraph* graph);
    ~Station();
    uint getID();
    void reloadPlotSettings();
    void refresh(double pressure, double temperature, double ambient);
    void hasStoped();
    void checkErrorCode(const int codeError);
};

namespace StationError {
    enum class errorCodes { eInitPressureLoad = 1, ePressureLoose, eRecurrentPressureLoad };

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
            this->v_errMsg = QString("Se produjo una falla en la estación %1 por recargas recurrentes de presión").arg(station_ID);
        }
        const QString what() { return this->v_errMsg; }
        void raise() const override { throw *this; }
        RecurrentPressureLoad *clone() const override { return new RecurrentPressureLoad(*this); }
    };
}
#endif // STATION_H
