#include "datavisualizer.controller.h"
#include "ui_datavisualizer.controller.h"

DataVisualizerWindow::DataVisualizerWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::DataVisualizerWindow) {
    ui->setupUi(this);

    this->dialogStation = nullptr;
    this->initStatusBar();
    this->myActivePort = QSharedPointer<SerialPortReader>(new SerialPortReader(this->PortStatus, this->ConnectionPort, this->ui->serialConnect));
    this->mStatusTimer = QSharedPointer<QTimer>(new QTimer(this));
    this->myDataDB       = QSharedPointer<Database>(new Database("localhost", 33060, "root", "root", "STEL_DB_DATA", this->ConnectionDataBase, this->ui->dbConnect));
    this->myDataDB->openDatabase();
    this->myActivePort->openPort();
    this->setStationsUI();
    connect(this->mStatusTimer.get(), &QTimer::timeout, this, &DataVisualizerWindow::statusConnections);
    this->mStatusTimer->start(250);
    this->ui->tabWidget->setEnabled(false);
}

DataVisualizerWindow::~DataVisualizerWindow() {
    this->myDataDB->close();
    this->myDataDB.clear();
    this->myActivePort.clear();
    delete ui;
}

void DataVisualizerWindow::setStationsUI() {
    for(auto myStation : myData.getStations()) {
        QLabel* pressurelbl    = this->findChild<QLabel*>("lblPress_" + QString::number(myStation->getID())),
              * temperaturelbl = this->findChild<QLabel*>("lblTemp_" + QString::number(myStation->getID())),
              * timelbl        = this->findChild<QLabel*>("lblTime_" + QString::number(myStation->getID()));
        QPushButton* btnConfig = this->findChild<QPushButton*>("btnEstConfig_" + QString::number(myStation->getID())),
                   * btnRun    = this->findChild<QPushButton*>("btnEstRun_" + QString::number(myStation->getID()));
        PressureTempGraph* mygraph = this->findChild<PressureTempGraph*>("GraphE_" + QString::number(myStation->getID()));
        QTabWidget* myTabs     = this->ui->tabWidget;
        myStation->start(this->myDataDB, pressurelbl, temperaturelbl, timelbl, btnConfig, btnRun, myTabs, mygraph);
        Station::read(*this->myDataDB.get(), *myStation.get());
        if(!myStation->isFree()) {
            myData.pushMessageSendPort(QString(QString::number(myStation->getID()) + "," + QString::number(myStation->getTargetPressure()) + "," + QString::number(myStation->getTargetTemperature()) + "\n").toUtf8());
            /**
             * Use timer to resend msg with station data to restart.
             * If time is greater than the finish time, station must stop
             */
        }
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

void DataVisualizerWindow::btnsStates(const bool state_) {
    this->ui->tabWidget->setEnabled(state_);
}

bool DataVisualizerWindow::btnsStates() {
    for(auto myStation : myData.getStations()) { return this->findChild<QGroupBox*>("gbBtns_" + QString::number(myStation->getID()))->isEnabled(); }
    return false;
}

void DataVisualizerWindow::btnStationsDialog(const unsigned int id_, QPushButton* btnState) {
    QSharedPointer<Station> myStation = myData.getStation(id_);
    if(myStation->isFree()) {
        this->dialogStation = new SetStation();
        StationResponse response = StationResponse::Cancel;
        this->dialogStation->sharePointer(&response, this->myDataDB, this->myActivePort, myStation);
        this->dialogStation->setModal(true);
        this->dialogStation->exec();
        delete this->dialogStation;

        if(response == StationResponse::Save) { btnState->setText("Detener"); }
        this->dialogStation = nullptr;
    } else {
        this->myActivePort->stationStop(id_);
        btnState->setText("Iniciar");
    }
}

void DataVisualizerWindow::on_btnEstConfig_1_clicked() { this->btnStationsDialog(1, this->ui->btnEstConfig_1); }

void DataVisualizerWindow::on_btnEstConfig_2_clicked() { this->btnStationsDialog(2, this->ui->btnEstConfig_2); }

void DataVisualizerWindow::on_btnEstConfig_3_clicked() { this->btnStationsDialog(3, this->ui->btnEstConfig_3); }

void DataVisualizerWindow::on_btnEstConfig_4_clicked() { this->btnStationsDialog(4, this->ui->btnEstConfig_4); }

void DataVisualizerWindow::on_btnEstConfig_5_clicked() { this->btnStationsDialog(5, this->ui->btnEstConfig_5); }

void DataVisualizerWindow::on_btnEstConfig_6_clicked() { this->btnStationsDialog(6, this->ui->btnEstConfig_6); }

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
    if(this->myDataDB->isOpen()) { this->myDataDB->closeDatabase(); }
    else { this->myDataDB->openDatabase(); }
}

void DataVisualizerWindow::on_paramConfig_triggered() {
    QPointer<configDBNorms> myNormsConfiguration = QPointer<configDBNorms>(new configDBNorms);
    myNormsConfiguration->setModal(false);
    myNormsConfiguration->setWindowFlag(Qt::WindowStaysOnTopHint);
    myNormsConfiguration->show();
}

void DataVisualizerWindow::statusConnections() {
    bool portState = this->myActivePort->statusPort(),
         DbState   = this->myDataDB->status();
    if(portState && DbState) {
        this->myBtnsStates = true;
        this->btnsStates(this->myBtnsStates);
    } else if(!portState || !DbState) {
        this->myBtnsStates = false;
        this->btnsStates(this->myBtnsStates);
    }
}

void DataVisualizerWindow::on_close_triggered() { this->close(); }
