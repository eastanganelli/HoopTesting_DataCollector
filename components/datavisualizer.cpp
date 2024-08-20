#include <QMessageBox>
#include "datavisualizer.h"
#include "ui_datavisualizer.h"
#include "../defines.h"
#include "setdb.h"
#include "setserial.h"
#include "generalsettings.h"

DataVisualizerWindow::DataVisualizerWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::DataVisualizerWindow) {
    ui->setupUi(this);
    this->ui->tabWidget->setEnabled(true);
    this->myActivePort   = QSharedPointer<SerialPortReader>(new SerialPortReader());
    Manager::myDatabases = QSharedPointer<Manager>(new Manager());
    QTimer::singleShot(0, this, SLOT(doLater()));
}

DataVisualizerWindow::~DataVisualizerWindow() {
    this->myActivePort.clear();
    for(auto myStation: Station::myStations) {
        disconnect(myStation.data(), &Station::statusChanged, this, &DataVisualizerWindow::Station_StatusChanged);
        disconnect(myStation.data(), &Station::labelsUpdate,  this, &DataVisualizerWindow::Station_LblsStates);
        disconnect(myStation.data(), &Station::hoopErrorCode, this, &DataVisualizerWindow::Station_ErrorCode);
    }
    disconnect(Manager::myDatabases.data(), &Manager::DatabaseConnection, this, &DataVisualizerWindow::Database_Connection);
    delete ui;
}

void DataVisualizerWindow::doLater() {
    this->initStatusBar();
    this->setStationsUI();
    connect(this->myActivePort.data(),   &SerialPortReader::CheckSerialPort, this, &DataVisualizerWindow::SerialPort_Status);
    connect(Manager::myDatabases.data(), &Manager::DatabaseConnection,       this, &DataVisualizerWindow::Database_Connection);
    connect(this,                        &DataVisualizerWindow::openDialog,  this, &DataVisualizerWindow::openDialogWindow);
    try {
        Manager::myDatabases->initialize();
        Manager::myDatabases->open();
        Manager::myDatabases->stationsFreedom();
    }
    catch(ManagerErrors::ConfigurationError& ex) { QMessageBox::warning(nullptr, "Base de Datos", ex.what(), QMessageBox::Ok); }
    catch(ManagerErrors::ConnectionError& ex)    { QMessageBox::warning(nullptr, "Base de Datos", ex.what(), QMessageBox::Ok); }
    try {
        this->myActivePort->initialize();
        if(Manager::myDatabases->isOpen()) {
            this->myActivePort->openPort();
        }
    }
    catch(SerialError::Parameter& ex) { QMessageBox::warning(nullptr, "Puerto Serial", ex.what(), QMessageBox::Ok); }
    catch(SerialError::OpenPort& ex)  { QMessageBox::warning(nullptr, "Puerto Serial", ex.what(), QMessageBox::Ok); }
}

void DataVisualizerWindow::openDialogWindow(const uint &ID_Station, const uint &ID_Test, const SetStation::Response &v_mode) { SetStation::stationConfiguration(ID_Station, ID_Test, v_mode); }

void DataVisualizerWindow::setStationsUI() {
    double yAxisDesviationRead = 0.00;
    QString pressureColor, temperatureColor;
    generalSettings::loadSettingsPlot(yAxisDesviationRead, pressureColor, temperatureColor);
    for(uint i = 1; i <= 6; i++) {
        QSharedPointer<Station> auxStation = QSharedPointer<Station>(new Station());
        PressureTempGraph* mygraph = this->findChild<PressureTempGraph*>("GraphE_" + QString::number(auxStation->getID()));
        mygraph->changeStyle(yAxisDesviationRead, pressureColor, temperatureColor);
        connect(auxStation.data(), &Station::cacheNewData,   Manager::myDatabases.data(), &Manager::insertData);
        connect(auxStation.data(), &Station::cacheErrorTest, Manager::myDatabases.data(), &Manager::failureTest);
        connect(auxStation.data(), &Station::plotNewPoint,   mygraph, &PressureTempGraph::insert);
        connect(auxStation.data(), &Station::statusChanged,  this, &DataVisualizerWindow::Station_StatusChanged);
        connect(auxStation.data(), &Station::labelsUpdate,   this, &DataVisualizerWindow::Station_LblsStates);
        connect(auxStation.data(), &Station::hoopErrorCode,  this, &DataVisualizerWindow::Station_ErrorCode);
        Station::myStations.insert(i, auxStation);
    }
}

void DataVisualizerWindow::initStatusBar() {
    this->ConnectionDataBase = new QLabel("Base de Datos: No Conectado");
    this->ConnectionPort     = new QLabel("Puerto: No Conectado");
    this->PortStatus         = new QLabel("Comunicación: Cerrada");
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

void DataVisualizerWindow::btnStationsDialog(const uint id_) {
    QSharedPointer<Station> myStation = Station::myStations[id_];
    SetStation::stationConfiguration(myStation->getID(), myStation->getTestID(), SetStation::Response::Save);
}

void DataVisualizerWindow::Station_StatusChanged(const Station::Status& myStatus) {
    Station* senderStation = qobject_cast<Station*>(sender());
    if(senderStation) {
        QPushButton* btnConfig = this->findChild<QPushButton*>("btnEstConfig_" + QString::number(senderStation->getID()));
        if(myStatus == Station::Status::READY) {
            if(btnConfig->isEnabled())
                btnConfig->setEnabled(false);
        } else if(myStatus == Station::Status::RUNNING) {
            if(!btnConfig->isEnabled())
                btnConfig->setEnabled(true);
        } else if(myStatus == Station::Status::WAITING) {
            SetStation::stationConfiguration(senderStation->getID(), senderStation->getTestID(), SetStation::Response::Export);
            PressureTempGraph* mygraph = this->findChild<PressureTempGraph*>("GraphE_" + QString::number(senderStation->getID()));
            btnConfig->setEnabled(false);
            mygraph->clear();
            senderStation->clear();
        }
    }
}

void DataVisualizerWindow::Station_ErrorCode(const QString& errMsg) {
    Station* senderStation = qobject_cast<Station*>(sender());
    QMessageBox msgBox(QMessageBox::Warning, "Error", errMsg);
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint);
    // msgBox.setWindowModality(Qt::WindowModal);
    // msgBox.setModal(true);
    msgBox.setStandardButtons(QMessageBox::Ok);
    int result = msgBox.exec();
    if(result == QMessageBox::Ok)
        emit senderStation->hasStoped();
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

void DataVisualizerWindow::SerialPort_Status(const SerialPortReader::Status &myStatus) {
    auto changeConnectionPort = [&](const QString &lbl_text, const QString& v_Status, const QString &v_color) {
        this->ui->serialConnect->setText(v_Status);
        this->ConnectionPort->setText("Puerto: " + lbl_text);
        this->ConnectionPort->setStyleSheet("color:" + v_color + ";");
    };
    auto changeCommunicationPort = [&](const QString &lbl_text, const QString &v_color) {
        this->PortStatus->setText("Comunicación: " + lbl_text);
        this->PortStatus->setStyleSheet("color:" + v_color + ";");
    };

    if(myStatus == SerialPortReader::Status::OPEN) {
        this->ui->serialConnect->setText("Cerrar Puerto");
        changeConnectionPort("Conectado", "Cerrar", StatusGreen);
    } else if(myStatus == SerialPortReader::Status::CLOSE) {
        this->ui->serialConnect->setText("Abrir Puerto");
        changeConnectionPort("Desconectado", "Abrir", StatusRed);
        changeCommunicationPort("Cerrada",   StatusRed);
    } else if(myStatus == SerialPortReader::Status::ACTIVE) {
        changeCommunicationPort("Abierta",   StatusGreen);
        changeConnectionPort("Conectado", "Cerrar", StatusGreen);
    } else if(myStatus == SerialPortReader::Status::INACTIVE) {
        changeCommunicationPort("Cerrada",   StatusRed);
    }
}

void DataVisualizerWindow::Plot_ChangeStyle(const double &yAxisDesviation, const QString &pressureColor, const QString &temperatureColor) {
    for(auto myStation: Station::myStations) {
        PressureTempGraph* mygraph = this->findChild<PressureTempGraph*>("GraphE_" + QString::number(myStation->getID()));
        mygraph->changeStyle(yAxisDesviation, pressureColor, temperatureColor);
    }
}

void DataVisualizerWindow::Database_Connection(const Manager::Status &v_Status) {
    auto changeConnectionDB = [&](const QString &lbl_Status, const QString& v_Status, const QString &v_color) {
        this->ui->dbConnect->setText(v_Status);
        this->ConnectionDataBase->setText("Base de Datos: " + lbl_Status);
        this->ConnectionDataBase->setStyleSheet("color:" + v_color + ";");
    };

    switch (v_Status) {
    case Manager::Status::OPEN:
        changeConnectionDB("Conectada", "Desconectar", StatusGreen);
        break;
    case Manager::Status::CLOSE:
        changeConnectionDB("Desconectada", "Conectar", StatusRed);
        break;
    }
}

void DataVisualizerWindow::on_serialConnect_triggered() {
    try {
        if(this->myActivePort->isOpen()) { this->myActivePort->closePort(); }
        else { this->myActivePort->openPort(); }
    }
    catch(SerialError::Parameter& ex) {
        QMessageBox::warning(nullptr, "Puerto Serial", ex.what(), QMessageBox::Ok);
    }
    catch(SerialError::OpenPort& ex) {
        QMessageBox::warning(nullptr, "Puerto Serial", ex.what(), QMessageBox::Ok);
    }
}

void DataVisualizerWindow::on_serialConfig_triggered() {
    QPointer<serialConfig> mySerialConfiguration = QPointer<serialConfig>(new serialConfig());
    mySerialConfiguration->setModal(true);
    mySerialConfiguration->exec();
}

void DataVisualizerWindow::on_dbConnect_triggered() {
    try {
        if(Manager::myDatabases->isOpen()) { Manager::myDatabases->close(); return; }
        Manager::myDatabases->open();
    }
    catch(ManagerErrors::ConfigurationError& ex) {
        QMessageBox::warning(nullptr, "Base de Datos", ex.what(), QMessageBox::Ok);
    }
    catch(ManagerErrors::ConnectionError& ex) {
        QMessageBox::warning(nullptr, "Base de Datos", ex.what(), QMessageBox::Ok);
    }
}

void DataVisualizerWindow::on_dbConfig_triggered() {
    QPointer<DBConfig> myDBConfiguration = QPointer<DBConfig>(new DBConfig());
    myDBConfiguration->setModal(true);
    myDBConfiguration->exec();
}

void DataVisualizerWindow::on_actionGr_fico_triggered() {
    generalSettings* myPlotSettings = new generalSettings();
    connect(myPlotSettings, &generalSettings::changeStyle,   this, &DataVisualizerWindow::Plot_ChangeStyle);
    connect(myPlotSettings, &generalSettings::changeTimeout, Manager::myDatabases.data(), &Manager::testTimeoutTime);
    myPlotSettings->setModal(true);
    myPlotSettings->exec();
    disconnect(myPlotSettings, &generalSettings::changeStyle,   this, &DataVisualizerWindow::Plot_ChangeStyle);
    disconnect(myPlotSettings, &generalSettings::changeTimeout, Manager::myDatabases.data(), &Manager::testTimeoutTime);
    delete myPlotSettings;
}

void DataVisualizerWindow::on_close_triggered() { this->close(); }

void DataVisualizerWindow::on_btnEstConfig_1_clicked() { this->btnStationsDialog(1); }

void DataVisualizerWindow::on_btnEstConfig_2_clicked() { this->btnStationsDialog(2); }

void DataVisualizerWindow::on_btnEstConfig_3_clicked() { this->btnStationsDialog(3); }

void DataVisualizerWindow::on_btnEstConfig_4_clicked() { this->btnStationsDialog(4); }

void DataVisualizerWindow::on_btnEstConfig_5_clicked() { this->btnStationsDialog(5); }

void DataVisualizerWindow::on_btnEstConfig_6_clicked() { this->btnStationsDialog(6); }
