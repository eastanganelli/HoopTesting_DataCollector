#ifndef STATION_H
#define STATION_H
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QDateTime>
#include <QException>
#include "../services/schemas/schemaData.h"
#include "../services/pressuretempgraph.h"

#define OFFSET_UPPER 1

const QDateTime DEFAULT_DATETIME = QDateTime(QDate(1970, 1, 1), QTime(0,0,0,0));

enum class StationStatus { WAITING, RUNNING, READY };

typedef struct {
    uint key;
    float pressure;
} pressurePoint;

class StationUI {
public:
    StationUI();
    ~StationUI();
    void startUI(QLabel* pressureLabel, QLabel* temperatureLabel, QLabel* timeLabel, QPushButton* configBtn, QPushButton *runBtn, QTabWidget* myTab, PressureTempGraph* myGraph);
    void updateUI(QString txtPre = "", QString txtTemp = "", QString txtTime = "", int key = -1, float Pressure = -1, float Temperature = -1);
    void resetUI();
    void refreshPlot(const double &yAxisDesviation, const QString &pressureColor, const QString &temperatureColor);
    void selectGraph(uint index);
    void waiting();
    void running();
    void stop();

private:
    QLabel* pressureLabel, * temperatureLabel, * timeLabel;
    QPushButton* configBtn, * runBtn;
    QTabWidget* myTab;
    PressureTempGraph* myGraph;
};

class Station {
    void setVisibleGraph(uint index);
    void saveCache();
    void clearCache();
    float getPressureMinimal();
    void slope();

    static uint autoMaxPressure();
    static bool IsFinished(const QDateTime actual, const QDateTime end);
    static uint stations;

    StationUI myUI;
    StationStatus status;
    const uint ID;
    const float MaxPressure;
    float pressureDesviation, sumPressure;
    uint testDuration, countPressure;
    QDateTime timer, initTest, finishTest;
    QVector<pressurePoint> pressurePoints;
    QSharedPointer<Data::NodeSample>   mySample;
    QSharedPointer<Data::NodeSpecimen> mySpecimen;

public:
    Station();
    ~Station();

    static void read(Station& myStation);
    static void set(Station& myStation);
    static void set(QSharedPointer<Station> selectedStation, QSharedPointer<Data::NodeSample> newSample, QSharedPointer<Data::NodeSpecimen> newSpecimen);
    static void set(QSharedPointer<Station> selectedStation, QSharedPointer<Data::NodeSample> mySample,  QSharedPointer<Data::NodeSpecimen> mySpecimen, const uint time_);

    void set(QLabel* pressureLabel, QLabel* temperatureLabel, QLabel* timeLabel, QPushButton* configBtn, QPushButton *runBtn, QTabWidget* myTab, PressureTempGraph* myGraph);
    void ini(QSharedPointer<Data::NodeSample> inSample = nullptr, QSharedPointer<Data::NodeSpecimen> inSpecimen = nullptr, const uint time_ = 0);
    void ini(QSharedPointer<Data::NodeSample> inSample = nullptr, QSharedPointer<Data::NodeSpecimen> inSpecimen = nullptr, const QDateTime initTime = DEFAULT_DATETIME, const QDateTime finisTime = DEFAULT_DATETIME);
    void start();
    void stop();
    void updateStatus(const float pressureInput, const float temperatureInput);
    void refresh(const float& pressureDesviation, const double &yAxisDesviation, const QString &pressureColor, const QString &temperatureColor);
    uint getID();
    uint getIDSample();
    uint getIDSpecimen();
    void setIDSample(const uint id);
    void setIDSpecimen(const uint id, const uint idSample);
    void setPressureDesviation(const float pressure);
    QSharedPointer<Data::NodeSample>   getSample();
    QSharedPointer<Data::NodeSpecimen> getSpecimen();
    const QDateTime getInitDateTime();
    const QDateTime getFinishDateTime();
    uint getTargetPressure();
    uint getTargetTemperature();
    StationStatus getStatus();
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
