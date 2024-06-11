#include "stationlist.h"

QSharedPointer<Station> StationList::operator[](const uint ID) {
   QSharedPointer<Station> selectedStation = nullptr;

    for(uint i = 0; i < this->length(); i++) {
        selectedStation = this->at(i);
        if(selectedStation->getID() == ID) { break; }
    }
    return selectedStation;
}
