#ifndef STATIONLIST_H
#define STATIONLIST_H
#include <QList>
#include "defines.h"
#include "station.controller.h"

class StationList : public QList<QSharedPointer<Station>> {
public:
    QSharedPointer<Station> operator[](const uint ID);
};
#endif // STATIONLIST_H
