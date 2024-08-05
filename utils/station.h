#ifndef STATION_H
#define STATION_H
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <QException>
#include "../services/schemas/schemaData.h"
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
    QDateTime started;

    static uint activeStation;

    void refreshPlot();
    void refreshLabels();
    void setHoopParameters();

public:
    Station(QLabel* pressure, QLabel* temperature, QLabel* time, QPushButton* config, QPushButton* startStop, QTabWidget* tabs, PressureTempGraph* graph);
    ~Station();
    uint getID();
    void refresh(float pressure, float temperature, float ambient);
    void refresh();
    void hasStoped();
public slots:
    void openMyHelp();
};

namespace StationError {
    class TestOverTime : public QException {
        QString error;
    public:
        TestOverTime() { this->error = "Over Time"; }
        const QString what() { return this->error; }
        void raise() const override { throw *this; }
        TestOverTime *clone() const override { return new TestOverTime(*this); }
    };

    class HoopPressureLoose : public QException {
        QString error;
    public:
        HoopPressureLoose() { this->error = "Hoop Break"; }
        const QString what() { return this->error; }
        void raise() const override { throw *this; }
        HoopPressureLoose *clone() const override { return new HoopPressureLoose(*this); }
    };
};
#endif // STATION_H
