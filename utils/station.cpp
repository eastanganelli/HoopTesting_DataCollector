#include <QObject>
#include "station.h"

uint Station::activeStation = 1;

Station::Station(): ID(activeStation) {
    this->started        = DEFAULT_DATETIME;
    this->timer          = DEFAULT_DATETIME;
    this->idTest         = 0;
    this->status         = Status::READY;
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
    // this->changeBtnsVisibility(false);
    // this->lblStatusHoop->setText(".");
    this->clear();
}

uint Station::getID() { return this->ID; }

void Station::setTestID(const uint testID) { this->idTest = testID; }

uint Station::getTestID() const { return this->idTest; }

// void Station::reloadPlotSettings() {
//     PressureTempGraph::plotRangeConfigurations(this->graph);
// }

void Station::clear() {
    // this->lblPressure->setText("0 Bar");
    // this->lblTemperature->setText("0 °C");
    // this->lblTime->setText("00:00:00");
    // this->lblStatusHoop->setText(".");
    // this->changeBtnsVisibility(false);
}

void Station::refresh(double pressure, double temperature, double ambient) {
    QDateTime actualTime = QDateTime::currentDateTime();
    if(this->started == DEFAULT_DATETIME) {
        this->status = Status::RUNNING;
        this->started = actualTime;
    }
    uint key  = this->started.secsTo(actualTime);
    this->hasStarted();
    QDateTime v_timerTxt = this->timer.addSecs(key);
    emit this->labelsUpdate(key, pressure, temperature);
    emit this->plotNewPoint(key, pressure, temperature);
}

void Station::hasStarted() {
    // if(this->btnSaveClear->isVisible())
    //     this->changeBtnsVisibility(false);
}

void Station::hasStoped() { }

// void Station::hasStoped() { this->changeBtnsVisibility(true); }

void Station::setStatus(const Status status) {
    this->status = status;
    emit statusChanged();
}

QDateTime Station::getTimer() { return this->timer; }

void Station::setTimer(const QDateTime timer) { this->timer = timer; }

Status Station::getStatus() const { return this->status; }

void Station::checkErrorCode(const int codeError, const uint a_ID) {
    switch(codeError) {
        case (int)StationError::errorCodes::eInitPressureLoad: {
            throw StationError::InitPressureLoad(a_ID);
            break;
        }
        case (int)StationError::errorCodes::ePressureLoose: {
            throw StationError::PressureLoose(a_ID);
            break;
        }
        case (int)StationError::errorCodes::eRecurrentPressureLoad: {
            throw StationError::RecurrentPressureLoad(a_ID);
            break;
        }
    }
}

// void Station::changeBtnsVisibility(const bool state) {
//     // this->btnSaveClear->setVisible(state);
//     // this->btnSaveClear->setEnabled(state);
//     // this->btnConfig->setVisible(!state);
// }
