#include <QObject>
#include "station.h"

uint Station::activeStation = 0;

Station::Station(QLabel *pressure, QLabel *temperature, QLabel *time, QLabel *statusHoop, QPushButton *config, QPushButton *saveClear, QTabWidget *tabs, PressureTempGraph *graph): ID(activeStation) {
    this->lblPressure    = pressure;
    this->lblTemperature = temperature;
    this->lblTime        = time;
    this->lblStatusHoop  = statusHoop;
    this->btnConfig      = config;
    this->btnSaveClear   = saveClear;
    this->graph          = graph;
    this->started        = DEFAULT_DATETIME;
    this->timer          = DEFAULT_DATETIME;
    this->idTest         = 0;
    activeStation++;

    /**
     * Check if the station is active in the SQLite DB
     * If was load the id of the test and the started time
     * Else set the idTest to 0 and the started time to the default time
     */
}

Station::~Station() { }

void Station::reloadTestParameters(const uint testID, const QDateTime started) {
    this->idTest   = testID;
    this->started  = started;
    this->timer    = started;
    this->changeBtnsVisibility(false);
    this->lblStatusHoop->setText(".");
    this->clear();
}

uint Station::getID() { return this->ID; }

void Station::setTestID(const uint testID) { this->idTest = testID; }

uint Station::getTestID() const { return this->idTest; }

void Station::reloadPlotSettings() { PressureTempGraph::plotRangeConfigurations(this->graph); }

void Station::clear() {
    this->lblPressure->setText("0 Bar");
    this->lblTemperature->setText("0 °C");
    this->lblTime->setText("00:00:00");
    this->lblStatusHoop->setText(".");
    this->changeBtnsVisibility(false);
}

void Station::refresh(double pressure, double temperature, double ambient) {
    QDateTime actualTime = QDateTime::currentDateTime();
    if(this->started == DEFAULT_DATETIME) {
        this->started = actualTime;
    }
    uint key  = this->started.secsTo(actualTime);
    this->hasStarted();
    this->refreshPlot(key,   pressure, temperature);
    this->refreshLabels(key, pressure, temperature);
}

void Station::hasStarted() {
    if(this->btnSaveClear->isVisible())
        this->changeBtnsVisibility(false);
}

void Station::hasStoped() { this->changeBtnsVisibility(true); }

void Station::hoopErrorCode(const int codeError) {
    try {
        try {
            this->checkErrorCode(codeError);
        } catch(StationError::InitPressureLoad& ex)    { throw ex.what(); }
        catch(StationError::PressureLoose& ex)         { throw ex.what(); }
        catch(StationError::RecurrentPressureLoad& ex) { throw ex.what(); }
    } catch(QString& ex) {
        this->lblStatusHoop->setText(ex);
        this->lblStatusHoop->setStyleSheet("color: red;");
    }
}

void Station::checkErrorCode(const int codeError) {
    switch(codeError) {
        case (int)StationError::errorCodes::eInitPressureLoad: {
            throw StationError::InitPressureLoad(this->ID);
            break;
        }
        case (int)StationError::errorCodes::ePressureLoose: {
            throw StationError::PressureLoose(this->ID);
            break;
        }
        case (int)StationError::errorCodes::eRecurrentPressureLoad: {
            throw StationError::RecurrentPressureLoad(this->ID);
            break;
        }
    }
}

void Station::changeBtnsVisibility(const bool state) {
    this->btnSaveClear->setVisible(state);
    this->btnSaveClear->setEnabled(state);
    this->btnConfig->setVisible(!state);
}

void Station::refreshPlot(const uint key, const double pressure, const double temperature) { this->graph->insert(key, pressure, temperature); }

void Station::refreshLabels(const uint key, const double pressure, const double temperature) {
    QDateTime lblText = this->timer.addSecs(key);
    this->lblPressure->setText(QString("%1 Bar").arg(QString::number(pressure, 'f', 2)));
    this->lblTemperature->setText(QString("%1 °C").arg(QString::number(temperature, 'f', 2)));
    this->lblTime->setText(QString::number((lblText.date().day() - 1) * 24 + lblText.time().hour()) + ":" + lblText.toString("mm:ss"));
}
