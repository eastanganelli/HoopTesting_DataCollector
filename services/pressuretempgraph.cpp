#include "../components/plotsettings.h"
#include "../services/pressuretempgraph.h"

PressureTempGraph::PressureTempGraph(QWidget* parent) : QCustomPlot(parent) {
    this->maxPressureVal = 0.00;
    this->minPressureVal = 0.00;
    this->maxTempVal = 0.00;
    this->minTempVal = 0.00;
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    this->xAxis->setTicker(timeTicker);
    this->addGraph(this->xAxis, this->yAxis);
    this->yAxis->setLabel("Presion");
    this->addGraph(this->xAxis, this->yAxis2);
    this->yAxis2->setLabel("Temperatura");
    this->yAxis2->setVisible(true);
    this->xAxis->setRange(0.00, 0.00);
    this->yAxis->setRange(this->minPressureVal, this->maxPressureVal);
    this->yAxis2->setRange(this->minTempVal,    this->maxTempVal);
    this->loadSettings();
    this->replot();

    connect(this->xAxis,  SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis,  SIGNAL(rangeChanged(QCPRange)), this->yAxis,  SLOT(setRange(QCPRange)));
    connect(this->yAxis2, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));
}

void PressureTempGraph::refresh(const double &yAxisDesviation, const QString &pressureColor, const QString &temperatureColor) {
    QColor pressure(pressureColor.isEmpty() ? QColor(Qt::green) : pressureColor), temperature(temperatureColor.isEmpty() ? QColor(Qt::red) : temperatureColor);
    this->yAxisDesviation = yAxisDesviation;
    this->graph(0)->setPen(QPen(pressure));
    this->yAxis->setLabelColor(pressure);
    this->graph(1)->setPen(QPen(temperature));
    this->yAxis2->setLabelColor(temperature);
    this->replot();
}

void PressureTempGraph::loadSettings() {
    double dummy, yaxisDesviation = 0.00;
    QString pressureColor, temperatureColor;
    plotSettings::loadSettings(dummy, yaxisDesviation, pressureColor, temperatureColor);
    QColor pressure(pressureColor.isEmpty() ? QColor(Qt::green) : pressureColor), temperature(temperatureColor.isEmpty() ? QColor(Qt::red) : temperatureColor);
    this->yAxisDesviation = yaxisDesviation;
    this->graph(0)->setPen(QPen(pressure));
    this->yAxis->setLabelColor(pressure);
    this->graph(1)->setPen(QPen(temperature));
    this->yAxis2->setLabelColor(temperature);
}

void PressureTempGraph::changeRanges(const float actualPressure, const float actualTemp) {
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

void PressureTempGraph::insertData(const double key, const float _pressure, const float _temp) {
    if((this->maxPressureVal == 0.00 && this->minPressureVal == 0.00) && (this->maxTempVal == 0.00 && this->minTempVal == 0.00)) {
        this->maxPressureVal = static_cast<double>(_pressure) + this->yAxisDesviation;
        this->minPressureVal = static_cast<double>(_pressure) - this->yAxisDesviation;
        this->maxTempVal = static_cast<double>(_temp) + this->yAxisDesviation;
        this->minTempVal = static_cast<double>(_temp) - this->yAxisDesviation;

        this->yAxis->setRange(this->minPressureVal, this->maxPressureVal);
        this->yAxis2->setRange(this->minTempVal, this->maxTempVal);
    } else { this->changeRanges(static_cast<double>(_pressure), static_cast<double>(_temp)); }
    this->graph(0)->addData(key, _pressure);
    this->graph(1)->addData(key, _temp);
    this->graph(0)->rescaleValueAxis(true);
    this->graph(1)->rescaleValueAxis(true);

    this->xAxis->setRange(key, key, Qt::AlignRight);
    this->replot();
}

void PressureTempGraph::reset() {
    for(int i = 0; i < this->graphCount(); i++) { this->graph(i)->data().data()->clear(); }
    this->maxPressureVal = 0.00;
    this->minPressureVal = 0.00;
    this->maxTempVal = 0.00;
    this->minTempVal = 0.00;
    this->xAxis->setRange(0.00, 0.00);
    this->yAxis->setRange(0.00, 0.00);
    this->yAxis2->setRange(0.00, 0.00);
    this->loadSettings();
    this->replot();
}
