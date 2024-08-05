#include "../services/global.h"
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
    activeStation++;

    /**
     * Check if the station is active in the SQLite DB
     * If was load the id of the test and the started time
     * Else set the idTest to 0 and the started time to the default time
     */
}

Station::~Station() { }

uint Station::getID() { return this->ID; }

void Station::refresh(float pressure, float temperature, float ambient) {
    qDebug() << "Station::refresh(float pressure, float temperature, float ambient)";
}

void Station::refreshPlot()
{

}

void Station::refreshLabels()
{

}

void Station::setHoopParameters()
{

}

void Station::openMyHelp() {
    QMessageBox msgBox;
    msgBox.setText("Configuración de la estación " + QString::number(this->ID));
    msgBox.exec();
}
