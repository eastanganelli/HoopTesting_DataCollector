#include <QMessageBox>
#include "datavisualizer.h"
#include "ui_datavisualizer.h"
#include "setdb.h"
#include "setserial.h"
#include "plotsettings.h"

QMap<uint, QSharedPointer<Station>> DataVisualizerWindow::myStations = QMap<uint, QSharedPointer<Station>>();
QSharedPointer<Manager> DataVisualizerWindow::myDatabases = QSharedPointer<Manager>(nullptr);

enum class Status;

DataVisualizerWindow::DataVisualizerWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::DataVisualizerWindow) {
    ui->setupUi(this); {
        this->dialogStation = nullptr;
        this->initStatusBar();
        this->myActivePort = QSharedPointer<SerialPortReader>(new SerialPortReader(this->PortStatus, this->ConnectionPort, this->ui->serialConnect));
        this->mStatusTimer = QSharedPointer<QTimer>(new QTimer(this));
    } {
        this->myActivePort->openPort();
        this->setStationsUI();
        connect(this->mStatusTimer.get(), &QTimer::timeout, this, &DataVisualizerWindow::statusConnections);
        // connect(this->mStatusTimer.get(), &QTimer::timeout, this, myStationsAux);
        this->mStatusTimer->start(ms_);
        this->ui->tabWidget->setEnabled(false);
    }
    DataVisualizerWindow::myDatabases = QSharedPointer<Manager>(new Manager());
    DataVisualizerWindow::myDatabases->initialize();
    DataVisualizerWindow::myDatabases->open();
}

DataVisualizerWindow::~DataVisualizerWindow() {
    this->myActivePort.clear();
    delete ui;
}

void DataVisualizerWindow::setStationsUI() {
    for(uint i = 1; i <= 6; i++) {
        QPushButton* btnConfig = this->findChild<QPushButton*>("btnEstConfig_" + QString::number(i));
        PressureTempGraph* mygraph = this->findChild<PressureTempGraph*>("GraphE_" + QString::number(i));
        btnConfig->setVisible(false); {
            double yAxisDesviationRead = 0.00;
            QString pressureColor, temperatureColor;
            plotSettings::loadSettings(yAxisDesviationRead, pressureColor, temperatureColor);
        }
        QSharedPointer<Station> auxStation = QSharedPointer<Station>(new Station());
        connect(auxStation.data(), &Station::statusChanged, this, &DataVisualizerWindow::Station_StatusChanged);
        connect(auxStation.data(), &Station::labelsUpdate,  this, &DataVisualizerWindow::Station_LblsStates);
        connect(auxStation.data(), &Station::plotNewPoint,  this, &DataVisualizerWindow::Plot_NewPoint);
        connect(auxStation.data(), &Station::hoopErrorCode, this, &DataVisualizerWindow::Station_ErrorCode);
        DataVisualizerWindow::myStations.insert(i, auxStation);
    }
}

void DataVisualizerWindow::initStatusBar() {
    this->ConnectionDataBase = new QLabel("Base de Datos: No Conectado");
    this->ConnectionPort     = new QLabel("Puerto: No Conectado");
    this->PortStatus         = new QLabel("Comunicación: Cerrado");
    this->ConnectionDataBase->setObjectName("lblConnectDB");
    this->ConnectionDataBase->setStyleSheet("color: red;");
    this->ConnectionPort->setObjectName("lblConnectionPort");
    this->ConnectionPort->setStyleSheet("color: red;");
    this->PortStatus->setObjectName("lblPortStatus");
    this->PortStatus->setStyleSheet("color: red;");
    ui->statusBar->addPermanentWidget(this->ConnectionDataBase);
    ui->statusBar->addPermanentWidget(this->ConnectionPort);
    ui->statusBar->addPermanentWidget(this->PortStatus);
    this->ui->serialConnect->setText("Abrir Puerto");
}

void DataVisualizerWindow::btnsStates(const bool state_) { this->ui->tabWidget->setEnabled(state_); }

bool DataVisualizerWindow::btnsStates() {
    for(auto myStation: DataVisualizerWindow::myStations) { return this->findChild<QGroupBox*>("gbBtns_" + QString::number(myStation->getID()))->isEnabled(); }
    return false;
}

void DataVisualizerWindow::btnStationsDialog(const uint id_) {
    try {
        DataVisualizerWindow::stationConfiguration(id_);
    }
    catch(...) {
        QMessageBox::warning(this, "Error", "Error al abrir la configuración de la estación");
    }
}

void DataVisualizerWindow::btnStationSaveClear(const uint id_) {
    QSharedPointer<Station> myStation = DataVisualizerWindow::myStations[id_];
    myStation->clear();
}

void DataVisualizerWindow::stationConfiguration(const uint ID_Station) {
    SetStation* dialogStation = new SetStation();
    dialogStation->setSelectStation(DataVisualizerWindow::myStations[ID_Station]);
    dialogStation->setModal(true);
    dialogStation->exec();
    delete dialogStation;
}

void DataVisualizerWindow::Station_StatusChanged() {
    Station* senderStation = qobject_cast<Station*>(sender());
    if(senderStation) {
        QPushButton* btnConfig = this->findChild<QPushButton*>("btnEstConfig_" + QString::number(senderStation->getID()));
        switch (senderStation->getStatus()) {
            case Status::READY:
                // senderStation->setPort(this->myActivePort);
                // senderStation->start();
                break;
            case Status::RUNNING:
                if(btnConfig->isVisible())
                    btnConfig->setVisible(true);
                break;
            case Status::WAITING:
                // senderStation->stop();
                break;
        }
    }
}

void DataVisualizerWindow::Station_ErrorCode(const int codeError) {
    Station* senderStation = qobject_cast<Station*>(sender());
    if(senderStation) {
        try {
            try {
                Station::checkErrorCode(codeError, senderStation->getID());
            } catch(StationError::InitPressureLoad& ex)    { throw ex.what(); }
            catch(StationError::PressureLoose& ex)         { throw ex.what(); }
            catch(StationError::RecurrentPressureLoad& ex) { throw ex.what(); }
        } catch(QString& ex) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
            msgBox.setText(ex);
            msgBox.setWindowTitle("Error");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
    }
}

void DataVisualizerWindow::Station_LblsStates(const uint key, const double pressure, const double temperature) {
    Station* senderStation = qobject_cast<Station*>(sender());
    if(senderStation) {
        const uint v_ID = senderStation->getID();
        QDateTime v_timerTxt = senderStation->getTimer().addSecs(key);
        QLabel* pressurelbl  = this->findChild<QLabel*>("lblPress_" + QString::number(v_ID)),
              * temperaturelbl = this->findChild<QLabel*>("lblTemp_" + QString::number(v_ID)),
              * timelbl = this->findChild<QLabel*>("lblTime_" + QString::number(v_ID));
        pressurelbl->setText(QString("%1 Bar").arg(QString::number(pressure, 'f', 2)));
        temperaturelbl->setText(QString("%1 °C").arg(QString::number(temperature, 'f', 2)));
        timelbl->setText(QString::number((v_timerTxt.date().day() - 1) * 24 + v_timerTxt.time().hour()) + ":" + v_timerTxt.toString("mm:ss"));
    }
}

void DataVisualizerWindow::Plot_NewPoint(const uint key, const double pressure, const double temperature) {
    Station* senderStation = qobject_cast<Station*>(sender());
    if(senderStation) {
        const uint v_ID = senderStation->getID();
        PressureTempGraph* mygraph = this->findChild<PressureTempGraph*>("GraphE_" + QString::number(v_ID));
        mygraph->insert(key, pressure, temperature);
    }
}

void DataVisualizerWindow::on_btnEstConfig_1_clicked() { this->btnStationsDialog(1); }

void DataVisualizerWindow::on_btnEstConfig_2_clicked() { this->btnStationsDialog(2); }

void DataVisualizerWindow::on_btnEstConfig_3_clicked() { this->btnStationsDialog(3); }

void DataVisualizerWindow::on_btnEstConfig_4_clicked() { this->btnStationsDialog(4); }

void DataVisualizerWindow::on_btnEstConfig_5_clicked() { this->btnStationsDialog(5); }

void DataVisualizerWindow::on_btnEstConfig_6_clicked() { this->btnStationsDialog(6); }

void DataVisualizerWindow::on_btnSvClr_1_clicked() { this->btnStationSaveClear(1); }

void DataVisualizerWindow::on_btnSvClr_2_clicked() { this->btnStationSaveClear(2); }

void DataVisualizerWindow::on_btnSvClr_3_clicked() { this->btnStationSaveClear(3); }

void DataVisualizerWindow::on_btnSvClr_4_clicked() { this->btnStationSaveClear(4); }

void DataVisualizerWindow::on_btnSvClr_5_clicked() { this->btnStationSaveClear(5); }

void DataVisualizerWindow::on_btnSvClr_6_clicked() { this->btnStationSaveClear(6); }

void DataVisualizerWindow::on_serialConnect_triggered() {
    if(this->myActivePort->isOpen()) { this->myActivePort->closePort(); }
    else { this->myActivePort->openPort(); }
}

void DataVisualizerWindow::on_serialConfig_triggered() {
    QPointer<serialConfig> mySerialConfiguration = QPointer<serialConfig>(new serialConfig());
    mySerialConfiguration->setModal(true);
    mySerialConfiguration->exec();
}

void DataVisualizerWindow::on_dbConfig_triggered() {
    QPointer<DBConfig> myDBConfiguration = QPointer<DBConfig>(new DBConfig());
    myDBConfiguration->setModal(true);
    myDBConfiguration->exec();
}

void DataVisualizerWindow::on_dbConnect_triggered() {
    // if(this->myDataDB->isOpen()) { this->myDataDB->close(); return; }
    // this->myDataDB->open();
}

void DataVisualizerWindow::statusConnections() {
    bool portState = this->myActivePort->statusPort()/*,
         DbState   = this->myDataDB->status()*/;
    if(portState /*&& DbState*/) {
        this->myBtnsStates = true;
        this->btnsStates(this->myBtnsStates);
    } else if(!portState /*|| !DbState*/) {
        this->myBtnsStates = false;
        this->btnsStates(this->myBtnsStates);
    }
}

void DataVisualizerWindow::on_close_triggered() { this->close(); }

void DataVisualizerWindow::on_actionGr_fico_triggered() {
    plotSettings* myPlotSettings = new plotSettings();
    myPlotSettings->setModal(true);
    myPlotSettings->exec();
    delete myPlotSettings;
}
