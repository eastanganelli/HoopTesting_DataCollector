#include "../services/pressuretempgraph.h"
#include <QDebug>

PressureTempGraph::PressureTempGraph(QWidget* parent) : QCustomPlot(parent) {
    this->maxPressureVal = 0.00;
    this->minPressureVal = 0.00;
    this->maxTempVal = 0.00;
    this->minTempVal = 0.00;
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    this->xAxis->setTicker(timeTicker);
    this->addGraph(this->xAxis, this->yAxis);
    this->graph(0)->setPen(QPen(QColor(Qt::green)));
    this->yAxis->setLabel("Presion");
    this->yAxis->setLabelColor(QColor(Qt::green));
    this->addGraph(this->xAxis, this->yAxis2);
    this->graph(1)->setPen(QPen(QColor(Qt::red)));
    this->yAxis2->setLabel("Temperatura");
    this->yAxis2->setLabelColor(QColor(Qt::red));
    this->yAxis2->setVisible(true);
    this->xAxis->setRange(0.00, 0.00);
    this->yAxis->setRange(this->minPressureVal, this->maxPressureVal);
    this->yAxis2->setRange(this->minTempVal,    this->maxTempVal);
    this->replot();

    connect(this->xAxis,  SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis,  SIGNAL(rangeChanged(QCPRange)), this->yAxis,  SLOT(setRange(QCPRange)));
    connect(this->yAxis2, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));
}

void PressureTempGraph::changeRanges(const float actualPressure, const float actualTemp) {
    const double offset  = 2.00;
    if(actualPressure > this->maxPressureVal || actualPressure < this->minPressureVal) {
        if(actualPressure > this->maxPressureVal) {
            this->maxPressureVal = actualPressure + offset;
        }
        else if(actualPressure < this->minPressureVal) {
            this->minPressureVal = actualPressure - offset;
        }
        this->yAxis->setRange(this->minPressureVal, this->maxPressureVal);
    }

    if(actualTemp > this->maxTempVal || actualTemp < this->minTempVal) {
        if(actualTemp > this->maxTempVal) {
            this->maxTempVal = actualTemp + offset;
        }
        else if(actualTemp < this->minTempVal) {
            this->minTempVal = actualTemp - offset;
        }
        this->yAxis2->setRange(this->minTempVal, this->maxTempVal);
    }
}

// void PressureTempGraph::maxRanges(const unsigned int pressure, const unsigned int temp) {
//     this->yAxis->setRangeUpper(pressure);
//     this->yAxis2->setRangeUpper(temp);
// }

// void PressureTempGraph::maxPressure(const unsigned int pressure) { this->yAxis->setRange(QCPRange(00,(double)pressure + 2.00)); }

// void PressureTempGraph::maxtemperature(const unsigned int temperature) { this->yAxis2->setRange(QCPRange(00, (double)temperature + 5.00)); }

void PressureTempGraph::insertData(const double key, const float _pressure, const float _temp) {
    if((this->maxPressureVal == 0.00 && this->minPressureVal == 0.00) && (this->maxTempVal == 0.00 && this->minTempVal == 0.00)) {
        this->maxPressureVal = static_cast<double>(_pressure) + 2.00;
        this->minPressureVal = static_cast<double>(_pressure) - 2.00;
        this->maxTempVal = static_cast<double>(_temp) + 2.00;
        this->minTempVal = static_cast<double>(_temp) - 2.00;

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
    this->yAxis->setRange(this->minPressureVal, this->maxPressureVal);
    this->yAxis2->setRange(this->minTempVal,    this->maxTempVal);
    this->replot();
}
