#include "../services/pressuretempgraph.h"
#include "../components/generalsettings.h"

class generalSettings;

PressureTempGraph::PressureTempGraph(QWidget* parent) : QCustomPlot(parent) {
    this->maxPressureVal   = 0.00;
    this->minPressureVal   = 0.00;
    this->maxTempVal       = 0.00;
    this->minTempVal       = 0.00;
    this->pressureColor    = QColor(Qt::green);
    this->temperatureColor = QColor(Qt::red);
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    this->xAxis->setTicker(timeTicker);
    this->addGraph(this->xAxis, this->yAxis);
    this->yAxis->setLabel("Presion");
    this->addGraph(this->xAxis, this->yAxis2);
    this->yAxis2->setLabel("Temperatura");
    this->yAxis2->setVisible(true);
    this->xAxis->setRange(0.00,  1.00);
    this->yAxis->setRange(0.00,  1.00);
    this->yAxis2->setRange(0.00, 1.00);
    this->replot();

    this->yAxis->setLabelColor(this->pressureColor);
    this->yAxis2->setLabelColor(this->temperatureColor);
    this->graph(0)->setPen(QPen(this->pressureColor));
    this->graph(0)->setAdaptiveSampling(true);
    this->graph(0)->rescaleValueAxis(true);
    this->graph(1)->setPen(QPen(this->temperatureColor));
    this->graph(1)->setAdaptiveSampling(true);
    this->graph(1)->rescaleValueAxis(true);
    PressureTempGraph::plotRangeConfigurations(this);

    connect(this->xAxis,  SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis,  SIGNAL(rangeChanged(QCPRange)), this->yAxis,  SLOT(setRange(QCPRange)));
    connect(this->yAxis2, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));
}

void PressureTempGraph::changeStyle(const double &yAxisDesviation, const QString &pressureColor, const QString &temperatureColor) {
    QColor pressure(pressureColor.isEmpty() ? QColor(Qt::green) : pressureColor), temperature(temperatureColor.isEmpty() ? QColor(Qt::red) : temperatureColor);
    this->yAxisDesviation = yAxisDesviation;
    this->graph(0)->setPen(QPen(pressure));
    this->yAxis->setLabelColor(pressure);
    this->graph(1)->setPen(QPen(temperature));
    this->yAxis2->setLabelColor(temperature);
    this->replot();
}

void PressureTempGraph::changeRanges(const double actualPressure, const double actualTemp) {
    if((this->maxPressureVal == 0.00 && this->minPressureVal == 0.00) && (this->maxTempVal == 0.00 && this->minTempVal == 0.00)) {
        this->maxPressureVal = actualPressure + this->yAxisDesviation;
        this->minPressureVal = actualPressure - this->yAxisDesviation;
        this->maxTempVal = actualTemp + this->yAxisDesviation;
        this->minTempVal = actualTemp - this->yAxisDesviation;

        this->yAxis->setRange(this->minPressureVal, this->maxPressureVal);
        this->yAxis2->setRange(this->minTempVal, this->maxTempVal);
    } else {
        if(actualPressure > this->maxPressureVal || actualPressure < this->minPressureVal) {
            if(actualPressure > this->maxPressureVal)      { this->maxPressureVal = actualPressure + this->yAxisDesviation; }
            else if(actualPressure < this->minPressureVal) { this->minPressureVal = actualPressure - this->yAxisDesviation; }
            this->yAxis->setRange(this->minPressureVal, this->maxPressureVal);
        }

        if(actualTemp > this->maxTempVal || actualTemp < this->minTempVal) {
            if(actualTemp > this->maxTempVal)      { this->maxTempVal = actualTemp + this->yAxisDesviation; }
            else if(actualTemp < this->minTempVal) { this->minTempVal = actualTemp - this->yAxisDesviation; }
            this->yAxis2->setRange(this->minTempVal, this->maxTempVal);
        }
    }
}

void PressureTempGraph::insert(const uint key, const double _pressure, const double _temp) {
    this->changeRanges(_pressure, _temp);
    this->graph(0)->addData(key, _pressure);
    this->graph(1)->addData(key, _temp);
    this->graph(0)->rescaleValueAxis(true);
    this->graph(1)->rescaleValueAxis(true);
    this->xAxis->setRange(key, key, Qt::AlignRight);
    this->replot();
}

void PressureTempGraph::clear() {
    for(int i = 0; i < this->graphCount(); i++) { this->graph(i)->data().data()->clear(); }
    this->maxPressureVal = 0.00;
    this->minPressureVal = 0.00;
    this->maxTempVal     = 0.00;
    this->minTempVal     = 0.00;
    this->xAxis->setRange(0.00,  1.00);
    this->yAxis->setRange(0.00,  1.00);
    this->yAxis2->setRange(0.00, 1.00);
    this->replot();
}

void PressureTempGraph::plotRangeConfigurations(PressureTempGraph *myPlot) {
    QString pressureColor, temperatureColor;
    generalSettings::loadSettingsPlot(myPlot->yAxisDesviation, pressureColor, temperatureColor);

    if(!pressureColor.isEmpty() || !temperatureColor.isEmpty()) {
        myPlot->pressureColor    = QColor(pressureColor);
        myPlot->temperatureColor = QColor(temperatureColor);
        myPlot->yAxis->setLabelColor(myPlot->pressureColor);
        myPlot->yAxis2->setLabelColor(myPlot->temperatureColor);
        myPlot->graph(0)->setPen(QPen(myPlot->pressureColor));
        myPlot->graph(1)->setPen(QPen(myPlot->temperatureColor));
    }
}
