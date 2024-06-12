#include "../services/pressuretempgraph.h"
#include <QDebug>

PressureTempGraph::PressureTempGraph(QWidget* parent) : QCustomPlot(parent) {
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
    this->yAxis->setRange(0.00, 0.00);
    this->yAxis2->setRange(0.00, 0.00);

    connect(this->xAxis,  SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis,  SIGNAL(rangeChanged(QCPRange)), this->yAxis, SLOT(setRange(QCPRange)));
    connect(this->yAxis2, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));
}

void PressureTempGraph::maxRanges(const unsigned int pressure, const unsigned int temp) {
    this->yAxis->setRangeUpper(pressure);
    this->yAxis2->setRangeUpper(temp);
}

void PressureTempGraph::maxPressure(const unsigned int pressure) { this->yAxis->setRange(QCPRange(00,(double)pressure + 2.00)); }

void PressureTempGraph::maxtemperature(const unsigned int temperature) { this->yAxis2->setRange(QCPRange(00, (double)temperature + 5.00)); }

void PressureTempGraph::insertData(const double key, const float _pressure, const float _temp) {
    this->graph(0)->addData(key, _pressure);
    this->graph(1)->addData(key, _temp);
    this->graph(0)->rescaleValueAxis(true);
    this->graph(1)->rescaleValueAxis(true);

    this->xAxis->setRange(key, key, Qt::AlignRight);
    this->replot();
}

void PressureTempGraph::reset() {
    for(int i = 0; i < this->graphCount(); i++) { this->graph(i)->data().data()->clear(); }
    this->xAxis->setRange(0.00, 0.00);
    this->yAxis->setRange(0.00, 30.00);
    this->yAxis2->setRange(0.00, 50.00);
    this->replot();
}
