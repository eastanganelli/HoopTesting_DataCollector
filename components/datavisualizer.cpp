#include "datavisualizer.h"
#include "ui_datavisualizer.h"
#include "../components/plotsettings.h"
#include "setserial.h"
#include "setdb.h"

QMap<uint, QSharedPointer<Station>> DataVisualizerWindow::myStations = QMap<uint, QSharedPointer<Station>>();
QSharedPointer<Manager> DataVisualizerWindow::myDatabases = QSharedPointer<Manager>(nullptr);

DataVisualizerWindow::DataVisualizerWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::DataVisualizerWindow) {
    ui->setupUi(this); {
        this->dialogStation = nullptr;
        this->initStatusBar();
        this->myActivePort = QSharedPointer<SerialPortReader>(new SerialPortReader(this->PortStatus, this->ConnectionPort, this->ui->serialConnect));
        this->mStatusTimer = QSharedPointer<QTimer>(new QTimer(this));
        // this->myDataDB     = QSharedPointer<Schemas::Data>(new Schemas::Data(this->ConnectionDataBase, this->ui->dbConnect));
    } {
        // this->myDataDB->open();
        this->myActivePort->openPort();
        this->setStationsUI();
        connect(this->mStatusTimer.get(), &QTimer::timeout, this, &DataVisualizerWindow::statusConnections);
        this->mStatusTimer->start(ms_);
        this->ui->tabWidget->setEnabled(false);
    }
    DataVisualizerWindow::myDatabases = QSharedPointer<Manager>(new Manager());
    DataVisualizerWindow::myDatabases->initialize();
    DataVisualizerWindow::myDatabases->open();
}

DataVisualizerWindow::~DataVisualizerWindow() {
    // this->myDataDB->close();
    // this->myDataDB.clear();
    this->myActivePort.clear();
    delete ui;
}

void DataVisualizerWindow::setStationsUI() {
    for(uint i = 1; i <= 6; i++) {
        QLabel* pressurelbl  = this->findChild<QLabel*>("lblPress_" + QString::number(i)),
              * temperaturelbl = this->findChild<QLabel*>("lblTemp_" + QString::number(i)),
              * timelbl = this->findChild<QLabel*>("lblTime_" + QString::number(i)),
              * statuslbl = this->findChild<QLabel*>("lblState_" + QString::number(i));
        QPushButton* btnConfig = this->findChild<QPushButton*>("btnEstConfig_" + QString::number(i)),
                   * btnSaveClear = this->findChild<QPushButton*>("btnSvClr_" + QString::number(i));
        PressureTempGraph* mygraph = this->findChild<PressureTempGraph*>("GraphE_" + QString::number(i));
        QTabWidget* myTabs = this->ui->tabWidget;
        btnSaveClear->setVisible(false); {
            double yAxisDesviationRead = 0.00;
            QString pressureColor, temperatureColor;
            plotSettings::loadSettings(yAxisDesviationRead, pressureColor, temperatureColor);
        }
        QSharedPointer<Station> auxStation = QSharedPointer<Station>(new Station(pressurelbl, temperaturelbl, timelbl, statuslbl, btnConfig, btnSaveClear, myTabs, mygraph));
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
