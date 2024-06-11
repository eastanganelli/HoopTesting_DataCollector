#include "defines.h"
#include "utils/station.h"
#include "services/global.h"

unsigned int Station::stations = 0;

Station::Station() : ID(++Station::stations), MaxPressure(Station::autoMaxPressure()) {
    this->status       = StationStatus::READY;
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

void Station::ini(QSharedPointer<Data::NodeSample> inSample, QSharedPointer<Data::NodeSpecimen> inSpecimen, const uint time_) {
    this->status       = StationStatus::WAITING;
    this->mySample     = inSample;
    this->mySpecimen   = inSpecimen;
    this->testDuration = time_;
    this->myUI.graphMaxYAxis(this->mySample->getTargetPressure() + OFFSET_UPPER, this->mySample->getTargetTemperature() + OFFSET_UPPER);
    this->myUI.waiting();
}

void Station::ini(QSharedPointer<Data::NodeSample> inSample, QSharedPointer<Data::NodeSpecimen> inSpecimen, const QDateTime initTime, const QDateTime finisTime) {
    this->status     = StationStatus::WAITING;
    this->mySample   = inSample;
    this->mySpecimen = inSpecimen;
    this->initTest   = initTime;
    this->finishTest = finisTime;
    this->myUI.graphMaxYAxis(this->mySample->getTargetPressure() + OFFSET_UPPER, this->mySample->getTargetTemperature() + OFFSET_UPPER);
    this->myUI.waiting();
}

void Station::start() {
    #if MODE_ == Emulation
        uint condPeriod = this->ui->txtCondPeriod->text().split(" ")[0].toUInt() * 3600;
        QString myMessage = QString::number(this->ID) + "," + QString::number(this->getTargetPressure()) + "," + QString::number(this->getTargetTemperature()) + "," + QString::number(condPeriod) + "\n";
    #else
       QString myMessage = QString::number(this->ID) + "," + QString::number(this->getTargetPressure()) + "," + QString::number(this->getTargetTemperature()) + "\n";
    #endif

    this->status     = StationStatus::RUNNING;
    this->myUI.running();
    if(this->initTest == DEFAULT_DATETIME && this->finishTest == DEFAULT_DATETIME) {
        this->initTest   = QDateTime::currentDateTime();
        this->finishTest = QDateTime::currentDateTime().addSecs(this->testDuration);
    }
    this->saveCache();
    myData.pushMessageSendPort(myMessage.toUtf8());
}

void Station::stop() {
    QString msg = QString::number(this->ID) + ",1\n";
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

void Station::set(QSharedPointer<Schemas::Data> dataDB, QLabel* pressureLabel, QLabel* temperatureLabel, QLabel* timeLabel, QPushButton* configBtn, QPushButton* runBtn, QTabWidget* myTab, PressureTempGraph* myGraph) {
    this->myUI.startUI(pressureLabel, temperatureLabel, timeLabel, configBtn, runBtn, myTab, myGraph);
    this->myUI.graphMaxYAxis(this->MaxPressure, 80);
    this->dataDB = dataDB;
}

bool Station::updateStatus(const float pressureInput, const float temperatureInput) {
    QDateTime actualTime = QDateTime::currentDateTime();
    {
        #if CONSOLEDEBUGMODE == ConsoleDebugOn
            qDebug() << "Init   Time:" << this->initTest.toString(datetime_format) << this->initTest.toSecsSinceEpoch();
            qDebug() << "Actual Time:" << actualTime.toString(datetime_format) << actualTime.toSecsSinceEpoch();
            qDebug() << "Finish Time:" << this->finishTest.toString(datetime_format) << this->finishTest.toSecsSinceEpoch();
            qDebug() << (actualTime < this->finishTest);
        #endif
    }

    if(actualTime <= this->finishTest) {
        uint key = this->initTest.msecsTo(actualTime);
        QDateTime lblText = this->timer.addMSecs(key);
        {
            #if CONSOLEDEBUGMODE == ConsoleDebugOn
                qDebug() << "Key   value:" << key;
                qDebug() << "Label Time:" << lblText.toString(datetime_format) << lblText.toSecsSinceEpoch();
            #endif
        }

        QString time_ = QString::number((lblText.date().day() - 1) * 24 + lblText.time().hour()) + ":" + lblText.toString("mm:ss");
        this->myUI.updateUI(QString::number(pressureInput, 'f', 2) + " bar", QString::number(temperatureInput, 'f', 2) + " C", time_, key, pressureInput, temperatureInput);
        Data::NodeData myData(this->getIDSpecimen(), pressureInput, temperatureInput);
        Data::NodeData::insert(*this->dataDB.get(), myData);
        return false;
    }
    this->stop();
    return true;
}

void Station::setMax() { this->myUI.graphMaxYAxis(this->getTargetPressure() + OFFSET_UPPER, this->getTargetTemperature() + OFFSET_UPPER); }

void Station::setVisibleGraph(uint index) { this->myUI.selectGraph(index); }

uint Station::getID() { return this->ID; }

uint Station::getIDSample()   { return this->mySample->getID(); }

uint Station::getIDSpecimen() { return this->mySpecimen->getID(); }

void Station::setIDSample(const uint id)                        { this->mySample = QSharedPointer<Data::NodeSample>(new Data::NodeSample(*this->getSample().get(), id)); }

void Station::setIDSpecimen(const uint id, const uint idSample) { this->mySpecimen = QSharedPointer<Data::NodeSpecimen>(new Data::NodeSpecimen(*this->getSpecimen().get(), id, idSample));}

QSharedPointer<Data::NodeSample> Station::getSample()     { return this->mySample; }

QSharedPointer<Data::NodeSpecimen> Station::getSpecimen() { return this->mySpecimen; }

const QDateTime Station::getInitDateTime()   { return this->initTest; }

const QDateTime Station::getFinishDateTime() { return this->finishTest; }

uint Station::getTargetPressure()    { return this->mySample->getTargetPressure(); }

uint Station::getTargetTemperature() { return this->mySample->getTargetTemperature(); }

StationStatus Station::getStatus()   { return this->status; }

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

void Station::read(Schemas::Data& myDB, Station& myStation) {
    QSettings mySettings(QApplication::applicationDirPath() + "/cachedStations.ini", QSettings::IniFormat);
    mySettings.beginGroup("Station_" + QString::number(myStation.getID()));
    const uint idSpecimen = mySettings.value("idSpecimen").toUInt();
    const QDateTime finishDate = QDateTime::fromString(mySettings.value("finishTime").toString(), QString(datetime_format)),
                    initDate   = QDateTime::fromString(mySettings.value("initTime").toString(),   QString(datetime_format));
    {
        #if CONSOLEDEBUGMODE == ConsoleDebugOn
            qDebug() << "ID Specimen:"     << idSpecimen
                     << "Finish Datetime:" << finishDate.toString(datetime_format);
        #endif
    }

    if(idSpecimen > 0) {
        QSharedPointer<Data::NodeSpecimen> mySpecimen = Data::NodeSpecimen::get(myDB, idSpecimen);
        QSharedPointer<Data::NodeSample>   mySample   = Data::NodeSample::get(myDB, mySpecimen->getIDSample());
        myStation.ini(mySample, mySpecimen, initDate, finishDate);
        myStation.start();
    }
    mySettings.endGroup();
}

void Station::set(Schemas::Data &myDB, Station &myStation) {
    QSharedPointer<Data::NodeSample>   auxSample   = myStation.getSample();
    QSharedPointer<Data::NodeSpecimen> auxSpecimen = myStation.getSpecimen();

    const uint idSample = Data::NodeSample::insert(myDB, auxSample);
    if(idSample != myStation.getIDSample()) {
        myStation.setIDSample(idSample);
        auxSample = myStation.getSample();
    }

    const uint idSpecimen = Data::NodeSpecimen::insert(myDB, auxSpecimen, idSample);
    myStation.setIDSpecimen(idSpecimen, idSample);
}

void Station::set(QSharedPointer<Station> selectedStation, QSharedPointer<Data::NodeSample> mySample, QSharedPointer<Data::NodeSpecimen> mySpecimen, const uint time_) {
    selectedStation->ini(mySample, mySpecimen, time_);
    selectedStation->setMax();
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
    this->myGraph->maxPressure(maxPressure);
    this->myGraph->maxtemperature(maxTemperature);
}

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
