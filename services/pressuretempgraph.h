#ifndef AGRAPH_H
#define AGRAPH_H
#include <QtMath>
#include "../utils/qcustomplot.h"

class PressureTempGraph : public QCustomPlot {
    Q_OBJECT
    double yAxisDesviation, minPressureVal, maxPressureVal, minTempVal, maxTempVal;
    void changeRanges(const float actualPressure, const float actualTemp);
    void loadSettings();

public:
    PressureTempGraph(QWidget* parent = nullptr);
    void refresh(const double &yAxisDesviation, const QString &pressureColor, const QString &temperatureColor);
    void insertData(const double key, const float _pressure, const float _temp);
    void reset();
};
#endif // AGRAPH_H
