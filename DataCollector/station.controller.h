#ifndef STATION_H
#define STATION_H
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QDateTime>
#include "defines.h"
#include "database.service.h"
#include "pressuretempgraph.controller.h"

const QDateTime DEFAULT_DATETIME = QDateTime(QDate(1970, 1, 1), QTime(0,0,0,0));

class StationUI {
public:
    StationUI();
    ~StationUI();
    void startUI(QLabel* pressureLabel, QLabel* temperatureLabel, QLabel* timeLabel, QPushButton* configBtn, QPushButton *runBtn, QTabWidget* myTab, PressureTempGraph* myGraph);
    void updateUI(QString txtPre = "", QString txtTemp = "", QString txtTime = "", int key = -1, float Pressure = -1, float Temperature = -1);
    void resetUI();
    void graphMaxYAxis(const u_int maxPressure = -1, const u_int maxTemperature = -1);
    void selectGraph(uint index);
private:
    QLabel* pressureLabel, * temperatureLabel, * timeLabel;
    QPushButton* configBtn,
               * runBtn;
    QTabWidget* myTab;
    PressureTempGraph* myGraph;
};

class Station {
    static void save(Station& myStation);
    static void clear(const uint idStation);
    static uint autoMaxPressure();
    static void setCacheOpenStation();
    static bool IsFinished(const QDateTime actual, const QDateTime end);
    static uint stations;

    StationUI myUI;
    const uint ID;
    const float MaxPressure;
    uint testDuration;
    QDateTime timer, initTest, finishTest;
    QSharedPointer<Database> dataDB;
    QSharedPointer<Data::NodeSample>   mySample;
    QSharedPointer<Data::NodeSpecimen> mySpecimen;
public:
    Station();
    ~Station();
    void start(QSharedPointer<Data::NodeSample> inSample = nullptr, QSharedPointer<Data::NodeSpecimen> inSpecimen = nullptr, const uint time_ = 0);
    void start(QSharedPointer<Data::NodeSample> inSample = nullptr, QSharedPointer<Data::NodeSpecimen> inSpecimen = nullptr, const QDateTime initTime = DEFAULT_DATETIME, const QDateTime finisTime = DEFAULT_DATETIME);
    void start(QSharedPointer<Database> dataDB, QLabel* pressureLabel, QLabel* temperatureLabel, QLabel* timeLabel, QPushButton* configBtn, QPushButton *runBtn, QTabWidget* myTab, PressureTempGraph* myGraph);
    void stop();
    bool updateStatus(const float pressureInput, const float temperatureInput);
    void setMaxTemperature(const unsigned int temperature);
    void setVisibleGraph(uint index);
    const uint getID();
    const uint getIDSample();
    const uint getIDSpecimen();
    const QDateTime getInitDateTime();
    const QDateTime getFinishDateTime();
    const uint getTargetPressure();
    const uint getTargetTemperature();
    const bool isFree();

    static void read(const Database& myDB, Station& myStation);
    static void configure(QSharedPointer<Station> selectedStation, QSharedPointer<Data::NodeSample> mySample, QSharedPointer<Data::NodeSpecimen> mySpecimen, const uint time_);
};
#endif // STATION_H
