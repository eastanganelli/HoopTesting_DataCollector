#ifndef AGRAPH_H
#define AGRAPH_H
#include <QtMath>
#include "../utils/qcustomplot.h"

class PressureTempGraph : public QCustomPlot {
    Q_OBJECT

    QColor pressureColor, temperatureColor;
    double yAxisDesviation, minPressureVal, maxPressureVal, minTempVal, maxTempVal;

    void changeRanges(const double actualPressure, const double actualTemp);

public:
    PressureTempGraph(QWidget* parent = nullptr);
    void refresh(const double &yAxisDesviation, const QString &pressureColor, const QString &temperatureColor);
    void insert(const uint key, const double _pressure, const double _temp);
    void clear();

    static void plotRangeConfigurations(PressureTempGraph* myPlot);
};
#endif // AGRAPH_H
