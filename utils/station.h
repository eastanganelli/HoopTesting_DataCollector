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
    uint idTest;
    QDateTime timer, started;

    QLabel* lblPressure, *lblTemperature, *lblTime, *lblStatusHoop;
    QPushButton* btnConfig, *btnSaveClear;
    PressureTempGraph* graph;

    static uint activeStation;

    void changeBtnsVisibility(const bool state);
    void refreshPlot(const uint key, const double pressure, const double temperature);
    void refreshLabels(const uint key, const double pressure, const double temperature);
    void checkErrorCode(const int codeError);

public:
    Station(QLabel* pressure, QLabel* temperature, QLabel* time, QLabel* statusHoop, QPushButton* config, QPushButton* saveClear, QTabWidget* tabs, PressureTempGraph* graph);
    ~Station();
    void reloadTestParameters(const uint testID, const QDateTime started);
    uint getID();
    void setTestID(const uint testID);
    uint getTestID() const;
    void reloadPlotSettings();
    void clear();
    void refresh(double pressure, double temperature, double ambient);
    void hasStarted();
    void hasStoped();
    void hoopErrorCode(const int codeError);
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
