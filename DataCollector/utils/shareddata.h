#ifndef SHAREDDATA_H
#define SHAREDDATA_H
#include <QList>
#include <QSharedPointer>
#include <QStack>
#include "../defines.h"
#include "utils/stationlist.h"

class SharedData {
    StationList myStations;
    QSharedPointer<QStack<QByteArray>> cachePortSendings;
    bool statePort;
public:
    SharedData(const uint stations = 6);
    ~SharedData();
    StationList getStations();
    QSharedPointer<Station> getStation(const uint ID);
    void pushMessageSendPort(const QByteArray msg);
    const QByteArray getMessageSendPort();
    void changePortState();
};
#endif // SHAREDDATA_H
