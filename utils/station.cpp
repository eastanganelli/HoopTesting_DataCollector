#include <QObject>
#include "station.h"

uint Station::activeStation = 0;

Station::Station(QLabel *pressure, QLabel *temperature, QLabel *time, QPushButton *config, QPushButton *startStop, QTabWidget *tabs, PressureTempGraph *graph): ID(activeStation) {
    this->lblPressure    = pressure;
    this->lblTemperature = temperature;
    this->lblTime        = time;
    this->btnConfig      = config;
    this->btnStartStop   = startStop;
    this->graph          = graph;
    this->started        = DEFAULT_DATETIME;
    this->timer          = DEFAULT_DATETIME;
    activeStation++;

    /**
     * Check if the station is active in the SQLite DB
     * If was load the id of the test and the started time
     * Else set the idTest to 0 and the started time to the default time
     */
}

Station::~Station() { }

uint Station::getID() { return this->ID; }

void Station::reloadPlotSettings() { PressureTempGraph::plotRangeConfigurations(this->graph); }

void Station::refresh(double pressure, double temperature, double ambient) {
    QDateTime actualTime = QDateTime::currentDateTime();
    if(this->started == DEFAULT_DATETIME) {
        this->started = actualTime;
    }
    uint key  = this->started.secsTo(actualTime);
    this->refreshPlot(key, pressure, temperature);
    this->refreshLabels(key, pressure, temperature);
}

void Station::refreshPlot(const uint key, const double pressure, const double temperature) { this->graph->insert(key, pressure, temperature); }

void Station::refreshLabels(const uint key, const double pressure, const double temperature) {
    QDateTime lblText = this->timer.addSecs(key);
    // QString::number(pressureInput, 'f', 2) + " bar", QString::number(temperatureInput, 'f', 2) + " C"
    // qDebug() << "Station[" << this->ID << "] -> Pressure[" << pressure << "] -> Time[" << lblText.toString("hh:mm:ss") << "] -> Temp[" << temperature << "]";
    this->lblPressure->setText(QString("%1 Bar").arg(QString::number(pressure, 'f', 2)));
    this->lblTemperature->setText(QString("%1 °C").arg(QString::number(temperature, 'f', 2)));
    this->lblTime->setText(QString::number((lblText.date().day() - 1) * 24 + lblText.time().hour()) + ":" + lblText.toString("mm:ss"));
}

void Station::setHoopParameters() {

}
