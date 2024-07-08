#include "../services/global.h"
#include "station.h"

unsigned int Station::stations = 0;

Station::Station() : ID(++Station::stations), MaxPressure(Station::autoMaxPressure()) {
    this->status              = StationStatus::READY;
    this->mySample            = nullptr;
    this->mySpecimen          = nullptr;
    this->initTest            = QDateTime(QDate(1970, 1, 1), QTime(0,0,0,0));
    this->finishTest          = QDateTime(QDate(1970, 1, 1), QTime(0,0,0,0));
    this->timer               = QDateTime(QDate(1970, 1, 1), QTime(0,0,0,0));
    this->testDuration        = 0;
    this->countPressure       = 0;
    this->sumPressure         = 0;
    this->pressureDesviation  = 0.2;
    this->autoStopDesviation  = false;
    this->desviationMinValues = 4;
}

Station::~Station() {
    this->mySample.clear();
    this->mySpecimen.clear();
}

void Station::ini(QSharedPointer<Data::NodeSample> inSample, QSharedPointer<Data::NodeSpecimen> inSpecimen, const uint time_) {
    this->status       = StationStatus::WAITING;
    this->mySample     = inSample;
    this->mySpecimen   = inSpecimen;
    this->testDuration = time_;
    this->myUI.waiting();
}

void Station::ini(QSharedPointer<Data::NodeSample> inSample, QSharedPointer<Data::NodeSpecimen> inSpecimen, const QDateTime initTime, const QDateTime finisTime) {
    this->status     = StationStatus::WAITING;
    this->mySample   = inSample;
    this->mySpecimen = inSpecimen;
    this->initTest   = initTime;
    this->finishTest = finisTime;
    this->myUI.waiting();
}

void Station::start() {
    QString myMessage = "start," + QString::number(this->ID) + "," + QString::number(this->getTargetPressure()) + "," + QString::number(this->getTargetTemperature()) + "\n";
    this->status         = StationStatus::RUNNING;
    this->myUI.running();
    myData.pushMessageSendPort(myMessage.toUtf8());
}

void Station::stop() {
    QString msg = "stop," + QString::number(this->ID) + "\n";
    myData.pushMessageSendPort(msg.toUtf8());

    this->myUI.resetUI();
    this->mySample.clear();
    this->mySpecimen.clear();
    this->testDuration = 0;
    this->initTest     = DEFAULT_DATETIME;
    this->finishTest   = DEFAULT_DATETIME;
    this->status       = StationStatus::READY;
    this->myUI.stop();
    this->clearCache();
}

void Station::set(QLabel* pressureLabel, QLabel* temperatureLabel, QLabel* timeLabel, QPushButton* configBtn, QPushButton* runBtn, QTabWidget* myTab, PressureTempGraph* myGraph) { this->myUI.startUI(pressureLabel, temperatureLabel, timeLabel, configBtn, runBtn, myTab, myGraph); }

void Station::updateStatus(const float pressureInput, const float temperatureInput) {
    QDateTime actualTime = QDateTime::currentDateTime();

    if(this->initTest == DEFAULT_DATETIME && this->finishTest == DEFAULT_DATETIME) {
        this->initTest   = actualTime;
        this->finishTest = QDateTime::currentDateTime().addSecs(this->testDuration);
        this->saveCache();
    }
    if(actualTime <= this->finishTest.addSecs(1)) {
        uint key          = this->initTest.msecsTo(actualTime);
        QDateTime lblText = this->timer.addMSecs(key);

        // qDebug() << "State of Auto Desviation -> " << this->autoStopDesviation;
        if(this->autoStopDesviation) {
            // qDebug() << "Check if Hoop did break";
            // try {
                this->sumPressure  += pressureInput;
                this->countPressure++;
                // qDebug() << "Pressure under avg -> " << pressureInput << " < " << this->getPressureMinimal();
                if(pressureInput < this->getPressureMinimal()) { this->pressurePoints.push_back({ key, pressureInput }); }
                else if(pressureInput > this->getPressureMinimal()) { this->pressurePoints.clear(); }
                this->slope();
            // } catch(StationError::HoopPressureLoose& ex) { qDebug() << "Hoop is broken"; throw ex; }
        }
        QString time_ = QString::number((lblText.date().day() - 1) * 24 + lblText.time().hour()) + ":" + lblText.toString("mm:ss");
        this->myUI.updateUI(QString::number(pressureInput, 'f', 2) + " bar", QString::number(temperatureInput, 'f', 2) + " C", time_, key, pressureInput, temperatureInput);
        Data::NodeData myData(this->getIDSpecimen(), pressureInput, temperatureInput);
        Data::NodeData::insert(myData);
    } else { throw StationError::TestOverTime(); }
}

void Station::refresh(const bool& activeDesviation, const float& pressureDesviation, const uint& minValuesDeviation, const double &yAxisDesviation, const QString &pressureColor, const QString &temperatureColor)  {
    this->autoStopDesviation  = activeDesviation;
    this->pressureDesviation  = pressureDesviation;
    this->desviationMinValues = minValuesDeviation;
    this->myUI.refreshPlot(yAxisDesviation, pressureColor, temperatureColor);
}

void Station::setVisibleGraph(uint index) { this->myUI.selectGraph(index); }

uint Station::getID()          { return this->ID; }

uint Station::getIDSample()   { return this->mySample->getID(); }

uint Station::getIDSpecimen() { return this->mySpecimen->getID(); }

void Station::setIDSample(const uint id)                         { this->mySample   = QSharedPointer<Data::NodeSample>(new Data::NodeSample(*this->getSample().get(), id)); }

void Station::setIDSpecimen(const uint id, const uint idSample) { this->mySpecimen = QSharedPointer<Data::NodeSpecimen>(new Data::NodeSpecimen(*this->getSpecimen().get(), id, idSample));}

QSharedPointer<Data::NodeSample> Station::getSample()     { return this->mySample; }

QSharedPointer<Data::NodeSpecimen> Station::getSpecimen() { return this->mySpecimen; }

const QDateTime Station::getInitDateTime()   { return this->initTest; }

const QDateTime Station::getFinishDateTime() { return this->finishTest; }

uint Station::getTargetPressure()    { return this->mySpecimen->getTargetPressure(); }

uint Station::getTargetTemperature() { return this->mySpecimen->getTargetTemperature(); }

StationStatus Station::getStatus()    { return this->status; }

void Station::saveCache() {
    QSettings mySettings(QApplication::applicationDirPath() + "/cachedStations.ini", QSettings::IniFormat);
    mySettings.beginGroup("Station_" + QString::number(this->ID));
    mySettings.setValue("idSpecimen",  QString::number(this->mySpecimen->getID()));
    mySettings.setValue("initTime",    this->initTest.toString(datetime_format));
    mySettings.setValue("finishTime",  this->finishTest.toString(datetime_format));
    mySettings.endGroup();
    mySettings.sync();
}

void Station::clearCache() {
    QSettings mySettings(QApplication::applicationDirPath() + "/cachedStations.ini", QSettings::IniFormat);
    mySettings.beginGroup("Station_" + QString::number(this->ID));
    mySettings.remove("");
    mySettings.endGroup();
    mySettings.sync();
}

float Station::getPressureMinimal() { return (float)((this->sumPressure/this->countPressure) - this->pressureDesviation); }

void Station::slope() {
    if(this->pressurePoints.length() > this->desviationMinValues) {
        pressurePoint init = this->pressurePoints.first(), end = this->pressurePoints.last();
        double slope = ((end.pressure - init.pressure)/(end.key - init.key));
        // qDebug() << "Slope value: " << slope;
        if(slope < 0) {
            qDebug() << "Hoop is broken";
            throw StationError::HoopPressureLoose();
        }
    }
}

void Station::read(Station& myStation) {
    QSettings mySettings(QApplication::applicationDirPath() + "/cachedStations.ini", QSettings::IniFormat);
    mySettings.beginGroup("Station_" + QString::number(myStation.getID()));
    const uint idSpecimen = mySettings.value("idSpecimen").toUInt();
    const QDateTime finishDate = QDateTime::fromString(mySettings.value("finishTime").toString(), QString(datetime_format)),
                    initDate   = QDateTime::fromString(mySettings.value("initTime").toString(),   QString(datetime_format));
    if(idSpecimen > 0) {
        QSharedPointer<Data::NodeSpecimen> mySpecimen = Data::NodeSpecimen::get(idSpecimen);
        QSharedPointer<Data::NodeSample>   mySample   = Data::NodeSample::get(mySpecimen->getIDSample());
        myStation.ini(mySample, mySpecimen, initDate, finishDate);
        myStation.start();
    }
    mySettings.endGroup();
}

void Station::set(Station &myStation) {
    QSharedPointer<Data::NodeSample>   auxSample   = myStation.getSample();
    QSharedPointer<Data::NodeSpecimen> auxSpecimen = myStation.getSpecimen();
    const uint idSample = Data::NodeSample::insert(auxSample);
    if(idSample != myStation.getIDSample()) {
        myStation.setIDSample(idSample);
        auxSample = myStation.getSample();
    }
    const uint idSpecimen = Data::NodeSpecimen::insert(auxSpecimen, idSample);
    myStation.setIDSpecimen(idSpecimen, idSample);
}

void Station::set(QSharedPointer<Station> selectedStation, QSharedPointer<Data::NodeSample> mySample, QSharedPointer<Data::NodeSpecimen> mySpecimen, const uint time_) {
    selectedStation->ini(mySample, mySpecimen, time_);
    selectedStation->setVisibleGraph(selectedStation->getID());
}

uint Station::autoMaxPressure() { return Station::stations < 4 ? 30.00 : 60.00; }

bool Station::IsFinished(const QDateTime actual, const QDateTime end) { return actual <= end; }

StationUI::StationUI() {
    this->pressureLabel    = nullptr;
    this->temperatureLabel = nullptr;
    this->timeLabel  = nullptr;
    this->myGraph    = nullptr;
    this->configBtn  = nullptr;
    this->runBtn     = nullptr;
    this->myTab      = nullptr;
}

StationUI::~StationUI(){
    this->pressureLabel    = nullptr;
    this->temperatureLabel = nullptr;
    this->timeLabel  = nullptr;
    this->myGraph    = nullptr;
    this->configBtn  = nullptr;
    this->runBtn     = nullptr;
    this->myTab      = nullptr;
}

void StationUI::startUI(QLabel* pressureLabel, QLabel* temperatureLabel, QLabel* timeLabel, QPushButton* configBtn, QPushButton* runBtn, QTabWidget* myTab, PressureTempGraph* myGraph) {
    this->pressureLabel    = pressureLabel;
    this->temperatureLabel = temperatureLabel;
    this->timeLabel  = timeLabel;
    this->configBtn  = configBtn;
    this->runBtn     = runBtn;
    this->myTab      = myTab;
    this->myGraph    = myGraph;
}

void StationUI::updateUI(QString txtPre, QString txtTemp, QString txtTime, int key, float Pressure, float Temperature) {
    this->pressureLabel->setText((txtPre != "" ? txtPre : this->pressureLabel->text()));
    this->temperatureLabel->setText((txtTemp != "" ? txtTemp : this->temperatureLabel->text()));
    this->timeLabel->setText((txtTime != "" ? txtTime : this->timeLabel->text()));
    if(key != -1) { this->myGraph->insertData((key/1000.0), Pressure, Temperature); }
}

void StationUI::resetUI() {
    this->pressureLabel->setText("00.00 bar");
    this->temperatureLabel->setText("00.00 C");
    this->timeLabel->setText("00:00:00");
    this->runBtn->setText("Iniciar");
    this->myGraph->reset();
}

void StationUI::refreshPlot(const double &yAxisDesviation, const QString &pressureColor, const QString &temperatureColor) { this->myGraph->refresh(yAxisDesviation, pressureColor, temperatureColor); }

void StationUI::selectGraph(uint index) { this->myTab->setCurrentIndex(index - 1); }

void StationUI::waiting() {
    this->runBtn->setVisible(true);
    this->runBtn->setEnabled(true);
}

void StationUI::running() {
    this->configBtn->setEnabled(false);
    this->runBtn->setText("Detener");
}

void StationUI::stop() {
    this->configBtn->setEnabled(true);
    this->runBtn->setVisible(false);
    this->runBtn->setText("Inciar");
}
