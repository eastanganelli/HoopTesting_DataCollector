#ifndef CACHEDSTATION_H
#define CACHEDSTATION_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>

class cachedStation {
    QSqlDatabase myCachedDB;

    void initialize();
    void stationIsPopulated(const uint ID_Station);
    void copyToDB(const uint ID_Test);

public:
    cachedStation();
    ~cachedStation();
    bool open();
    bool close();
    uint stationTestID(const uint ID_Station) const;
    uint stationOccupy(const uint ID_Station);
    void stationFree(const uint ID_Station);
    void insertData(const uint ID_Test, const float pressure, const float temperature, const float ambient);
};
#endif // CACHEDSTATION_H
