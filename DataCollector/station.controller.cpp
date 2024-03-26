#include "station.controller.h"

unsigned int Station::stations = 0;

Station::Station() : ID(++Station::stations),MaxPressure(Station::autoMaxPressure()) {
    this->mySample     = nullptr;
    this->mySpecimen   = nullptr;
    this->initTest     = QDateTime(QDate(1970, 1, 1), QTime(0,0,0,0));
    this->finishTest   = QDateTime(QDate(1970, 1, 1), QTime(0,0,0,0));
    this->timer        = QDateTime(QDate(1970, 1, 1), QTime(0,0,0,0));
    this->testDuration = 0;
}

Station::~Station() {
    this->mySample.clear();
    this->mySpecimen.clear();
}

void Station::start(QSharedPointer<Data::NodeSample> inSample, QSharedPointer<Data::NodeSpecimen> inSpecimen, const uint time_) {
    if(!inSample.isNull()) {
        this->mySample = inSample;
        this->myUI.graphMaxYAxis(this->mySample->getTargetPressure() + 5, this->mySample->getTargetTemperature() * 5);
        this->testDuration = time_;
    }
    if(!inSpecimen.isNull()) { this->mySpecimen = inSpecimen; }
}

void Station::start(QSharedPointer<Data::NodeSample> inSample, QSharedPointer<Data::NodeSpecimen> inSpecimen, const QDateTime initTime, const QDateTime finisTime) {
    if(!inSample.isNull()) {
        this->mySample   = inSample;
        this->myUI.graphMaxYAxis(this->mySample->getTargetPressure() + 5, this->mySample->getTargetTemperature() * 5);
        this->initTest   = initTime;
        this->finishTest = finisTime;
    }
    if(!inSpecimen.isNull()) { this->mySpecimen = inSpecimen; }
}

void Station::start(QSharedPointer<Schemas::Data> dataDB, QLabel* pressureLabel, QLabel* temperatureLabel, QLabel* timeLabel, QPushButton* configBtn, QPushButton* runBtn, QTabWidget* myTab, PressureTempGraph* myGraph) {
    this->myUI.startUI(pressureLabel, temperatureLabel, timeLabel, configBtn, runBtn, myTab, myGraph);
    this->dataDB = dataDB;
}

void Station::stop() {
    this->myUI.resetUI();
    this->mySample.clear();
    this->mySpecimen.clear();
    this->testDuration = 0;
    this->initTest     = DEFAULT_DATETIME;
    this->finishTest   = DEFAULT_DATETIME;
    Station::clear(this->ID);
}

bool Station::updateStatus(const float pressureInput, const float temperatureInput) {
    if(this->initTest == DEFAULT_DATETIME && this->finishTest == DEFAULT_DATETIME) {
        this->initTest   = QDateTime::currentDateTime();
        this->finishTest = QDateTime::currentDateTime().addSecs(this->testDuration);
        Station::save(*this);
    }
    QDateTime actualTime = QDateTime::currentDateTime();
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "Init   Time:" << this->initTest.toString(datetime_format) << this->initTest.toSecsSinceEpoch();
        qDebug() << "Actual Time:" << actualTime.toString(datetime_format) << actualTime.toSecsSinceEpoch();
        qDebug() << "Finish Time:" << this->finishTest.toString(datetime_format) << this->finishTest.toSecsSinceEpoch();
        qDebug() << (actualTime < this->finishTest);
    #endif
    if(!this->mySpecimen.isNull() && (actualTime <= this->finishTest)) {
        uint key = this->initTest.msecsTo(actualTime);
        QDateTime lblText = this->timer.addMSecs(key);
        #if CONSOLEDEBUGMODE == ConsoleDebugOn
            qDebug() << "Key   value:" << key;
            qDebug() << "Label Time:" << lblText.toString(datetime_format) << lblText.toSecsSinceEpoch();
        #endif
        QString time_ = QString::number((lblText.date().day() - 1) * 24 + lblText.time().hour()) + ":" + lblText.toString("mm:ss");
        this->myUI.updateUI(QString::number(pressureInput, 'f', 2) + " bar", QString::number(temperatureInput, 'f', 2) + " C", time_, key, pressureInput, temperatureInput);
        Data::NodeData myData(this->getIDSpecimen(), pressureInput, temperatureInput);
        Data::NodeData::insert(*this->dataDB.get(), myData);
        return false;
    }
    this->stop();
    return true;
}

void Station::setMaxTemperature(const unsigned int temperature) { this->myUI.graphMaxYAxis(-1,temperature); }

void Station::setVisibleGraph(uint index) { this->myUI.selectGraph(index); }

uint Station::getID() { return this->ID; }

uint Station::getIDSample() { return this->mySample->getID(); }

uint Station::getIDSpecimen() { return this->mySpecimen->getID(); }

const QDateTime Station::getInitDateTime() { return this->initTest; }

const QDateTime Station::getFinishDateTime() { return this->finishTest; }

uint Station::getTargetPressure() { return this->mySample->getTargetPressure(); }

uint Station::getTargetTemperature() { return this->mySample->getTargetTemperature(); }

bool Station::isFree() { return this->mySample.isNull() || this->mySpecimen.isNull(); }

void Station::save(Station& myStation) {
    QSettings mySettings(QApplication::applicationDirPath() + "/cachedStations.ini", QSettings::IniFormat);
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "Station ID:" << myStation.getID() << " - Sample ID:" << myStation.getIDSample()
                 << " - Specimen ID:" << myStation.getIDSpecimen() << " - Finish Datetime:" << myStation.getDateTime().toString(datetime_format);
    #endif
    mySettings.beginGroup("Station_" + QString::number(myStation.getID()));
    mySettings.setValue("idSpecimen",  QString::number(myStation.getIDSpecimen()));
    mySettings.setValue("initTime",    myStation.getInitDateTime().toString(datetime_format));
    mySettings.setValue("finishTime",  myStation.getFinishDateTime().toString(datetime_format));
    mySettings.endGroup();
}

void Station::clear(const uint idStation) {
    QSettings mySettings(QApplication::applicationDirPath() + "/cachedStations.ini", QSettings::IniFormat);
    mySettings.beginGroup("Station_" + QString::number(idStation));
    mySettings.setValue("idSpecimen",  QString::number(0));
    mySettings.endGroup();
}

void Station::read(Schemas::Data& myDB, Station& myStation) {
    QSettings mySettings(QApplication::applicationDirPath() + "/cachedStations.ini", QSettings::IniFormat);
    mySettings.beginGroup("Station_" + QString::number(myStation.getID()));
    const uint idSpecimen = mySettings.value("idSpecimen").toUInt();
    const QDateTime finishDate = QDateTime::fromString(mySettings.value("finishTime").toString(), QString(datetime_format)),
                    initDate   = QDateTime::fromString(mySettings.value("initTime").toString(),   QString(datetime_format));
    #if CONSOLEDEBUGMODE == ConsoleDebugOn
        qDebug() << "ID Specimen:"     << idSpecimen
                 << "Finish Datetime:" << finishDate.toString(datetime_format);
    #endif
    if(idSpecimen > 0) {
        QSharedPointer<Data::NodeSpecimen> mySpecimen = Data::NodeSpecimen::get(myDB, idSpecimen);
        QSharedPointer<Data::NodeSample>   mySample   = Data::NodeSample::get(myDB, mySpecimen->getIDSample());
        myStation.start(mySample, mySpecimen, initDate, finishDate);
    }
    mySettings.endGroup();
}

void Station::configure(QSharedPointer<Station> selectedStation, QSharedPointer<Data::NodeSample> mySample, QSharedPointer<Data::NodeSpecimen> mySpecimen, const uint time_) {
    selectedStation->start(mySample, mySpecimen, time_);
    selectedStation->setMaxTemperature(mySample->getTargetTemperature());
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

void StationUI::graphMaxYAxis(const uint maxPressure, const uint maxTemperature) {
    if(maxPressure != -1) { this->myGraph->maxPressure(maxPressure); }
    if(maxTemperature != -1) { this->myGraph->maxtemperature(maxTemperature); }
}

void StationUI::selectGraph(uint index) { this->myTab->setCurrentIndex(index - 1); }
