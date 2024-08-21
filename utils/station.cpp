#include <QObject>
#include "station.h"

QMap<uint, QSharedPointer<Station>> Station::myStations  = QMap<uint, QSharedPointer<Station>>();
uint Station::activeStation = 1;

Station::Station(): ID(activeStation) {
    this->started        = DEFAULT_DATETIME;
    this->timer          = DEFAULT_DATETIME;
    this->idTest         = 0;
    activeStation++;
}

Station::~Station() { }

void Station::reloadTestParameters(const uint testID, const QDateTime started) {
    this->idTest   = testID;
    this->started  = started;
    this->timer    = started;
    this->clear();
}

uint Station::getID() { return this->ID; }

void Station::setTestID(const uint testID) { this->idTest = testID; }

uint Station::getTestID() const { return this->idTest; }

void Station::clear() {
    this->started = DEFAULT_DATETIME;
    this->timer   = DEFAULT_DATETIME;
    this->idTest  = 0;
    emit this->statusChanged(Status::READY);
}

void Station::refresh(double pressure, double temperature, double ambient) {
    QDateTime actualTime = QDateTime::currentDateTime();
    if(this->started == DEFAULT_DATETIME) { this->started = actualTime; }
    uint key  = this->started.secsTo(actualTime);
    QDateTime v_timerTxt = this->timer.addSecs(key);
    emit this->cacheNewData(this->ID, pressure, temperature, ambient);
    emit this->labelsUpdate(key, pressure, temperature);
    emit this->plotNewPoint(key, pressure, temperature);
}

void Station::hasStarted() { emit this->statusChanged(Status::RUNNING); }

void Station::hasStoped()  { emit this->statusChanged(Status::WAITING); }

void Station::hasStopError(const int& error_Code) {
    QString errorMsg = StationError::checkErrorCode(error_Code, this->ID);
    emit this->hoopErrorCode(errorMsg);
}

QDateTime Station::getTimer() { return this->timer; }

void Station::setTimer(const QDateTime timer) { this->timer = timer; }

QString StationError::checkErrorCode(const int& codeError, const uint& a_ID) {
    switch(codeError) {
        case (int)StationError::errorCodes::eInitPressureLoad: {
            // throw StationError::InitPressureLoad(a_ID);
            return QString("Se produjo una falla en la estación %1 al realizar la carga de presión inicial").arg(QString::number(a_ID));
        }
        case (int)StationError::errorCodes::ePressureLoose: {
            // throw StationError::PressureLoose(a_ID);
            return QString("Se produjo una falla en la estación %1 debido a una caída abrupta de presión").arg(QString::number(a_ID));
        }
        case (int)StationError::errorCodes::eRecurrentPressureLoad: {
            // throw StationError::RecurrentPressureLoad(a_ID);
            return QString("Se produjo una falla en la estación %1 por recargas recurrentes de presión").arg(QString::number(a_ID));
        }
    }
    return "";
}
