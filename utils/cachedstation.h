#ifndef CACHEDSTATION_H
#define CACHEDSTATION_H
#include <QSqlDatabase>
#include <QSqlQuery>

class cachedStation: public QSqlDatabase {
protected:
    static void initialize();
    static void insert();
    static void update();
    static void remove();
    static void select();

};
#endif // CACHEDSTATION_H
