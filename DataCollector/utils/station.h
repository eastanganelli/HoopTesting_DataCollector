#ifndef STATION_H
#define STATION_H
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QDateTime>
#include "services/schemas/schemas.h"
#include "services/schemas/schemaData.h"
#include "services/pressuretempgraph.h"

#define OFFSET_UPPER 1

const QDateTime DEFAULT_DATETIME = QDateTime(QDate(1970, 1, 1), QTime(0,0,0,0));

enum class StationStatus { WAITING, RUNNING, READY };

class StationUI {
public:
    StationUI();
    ~StationUI();
    void startUI(QLabel* pressureLabel, QLabel* temperatureLabel, QLabel* timeLabel, QPushButton* configBtn, QPushButton *runBtn, QTabWidget* myTab, PressureTempGraph* myGraph);
    void updateUI(QString txtPre = "", QString txtTemp = "", QString txtTime = "", int key = -1, float Pressure = -1, float Temperature = -1);
    void resetUI();
    void graphMaxYAxis(const uint maxPressure = -1, const uint maxTemperature = -1);
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
    void setMax();
    void setVisibleGraph(uint index);
    void saveCache();
    void clearCache();

    static uint autoMaxPressure();
    static bool IsFinished(const QDateTime actual, const QDateTime end);
    static uint stations;

    StationUI myUI;
    StationStatus status;
    const uint ID;
    const float MaxPressure;
    uint testDuration;
    QDateTime timer, initTest, finishTest;
    QSharedPointer<Schemas::Data>      dataDB;
    QSharedPointer<Data::NodeSample>   mySample;
    QSharedPointer<Data::NodeSpecimen> mySpecimen;

public:
    Station();
    ~Station();

    static void read(Schemas::Data& myDB, Station& myStation);
    static void set(Schemas::Data&  myDB, Station& myStation);
    static void set(QSharedPointer<Station> selectedStation, QSharedPointer<Data::NodeSample> newSample, QSharedPointer<Data::NodeSpecimen> newSpecimen);
    static void set(QSharedPointer<Station> selectedStation, QSharedPointer<Data::NodeSample> mySample,  QSharedPointer<Data::NodeSpecimen> mySpecimen, const uint time_);

    void set(QSharedPointer<Schemas::Data> dataDB, QLabel* pressureLabel, QLabel* temperatureLabel, QLabel* timeLabel, QPushButton* configBtn, QPushButton *runBtn, QTabWidget* myTab, PressureTempGraph* myGraph);
    void ini(QSharedPointer<Data::NodeSample> inSample = nullptr, QSharedPointer<Data::NodeSpecimen> inSpecimen = nullptr, const uint time_ = 0);
    void ini(QSharedPointer<Data::NodeSample> inSample = nullptr, QSharedPointer<Data::NodeSpecimen> inSpecimen = nullptr, const QDateTime initTime = DEFAULT_DATETIME, const QDateTime finisTime = DEFAULT_DATETIME);
    void start();
    void stop();
    bool updateStatus(const float pressureInput, const float temperatureInput);
    uint getID();
    uint getIDSample();
    uint getIDSpecimen();
    void setIDSample(const uint id);
    void setIDSpecimen(const uint id, const uint idSample);
    QSharedPointer<Data::NodeSample>   getSample();
    QSharedPointer<Data::NodeSpecimen> getSpecimen();
    const QDateTime getInitDateTime();
    const QDateTime getFinishDateTime();
    uint getTargetPressure();
    uint getTargetTemperature();
    StationStatus getStatus();
};
#endif // STATION_H
