#include "shareddata.controller.h"

SharedData::SharedData(const uint stations) {
    this->myStations = StationList();
    for(uint i = 0; i < stations; i++) { this->myStations.append(QSharedPointer<Station>(new Station())); }
    this->cachePortSendings = QSharedPointer<QStack<QByteArray>>(new QStack<QByteArray>());
    this->statePort = false;
}

SharedData::~SharedData() {
    this->myStations.clear();
    this->cachePortSendings.clear();
}

StationList SharedData::getStations() { return this->myStations; }

QSharedPointer<Station> SharedData::getStation(const uint ID) { return this->myStations[ID]; }

void SharedData::pushMessageSendPort(const QByteArray msg) {
    this->cachePortSendings->push(msg);
}

const QByteArray SharedData::getMessageSendPort() {
    if(!this->cachePortSendings->isEmpty()) { return this->cachePortSendings->pop(); }
    return QByteArray("");
}
