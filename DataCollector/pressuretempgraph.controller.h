#ifndef AGRAPH_H
#define AGRAPH_H
#include <QtMath>
#include "defines.h"
#include "qcustomplot.controller.h"

class PressureTempGraph : public QCustomPlot {
public:
    PressureTempGraph(QWidget* parent = nullptr);
    void maxRanges(const unsigned int pressure, const unsigned int temp);
    void maxPressure(const unsigned int pressure);
    void maxtemperature(const unsigned int temperature);
    void insertData(const double key, const float _pressure, const float _temp);
    void reset();
};
#endif // AGRAPH_H
