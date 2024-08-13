#include <QMessageBox>
#include "datavisualizer.h"
#include "ui_datavisualizer.h"
#include "../defines.h"
#include "setdb.h"
#include "setserial.h"
#include "plotsettings.h"

QMap<uint, QSharedPointer<Station>> DataVisualizerWindow::myStations  = QMap<uint, QSharedPointer<Station>>();
QSharedPointer<Manager>             DataVisualizerWindow::myDatabases = QSharedPointer<Manager>(nullptr);

DataVisualizerWindow::DataVisualizerWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::DataVisualizerWindow) {
    ui->setupUi(this);
    this->initStatusBar();
    this->myActivePort = QSharedPointer<SerialPortReader>(new SerialPortReader());
    this->mStatusTimer = QSharedPointer<QTimer>(new QTimer(this));
    this->setStationsUI();
    this->ui->tabWidget->setEnabled(false);
    DataVisualizerWindow::myDatabases = QSharedPointer<Manager>(new Manager());
    QTimer::singleShot(0, this, SLOT(doLater()));
}

DataVisualizerWindow::~DataVisualizerWindow() {
    this->myActivePort.clear();
    for(auto myStation: DataVisualizerWindow::myStations) {
        disconnect(myStation.data(), &Station::statusChanged, this, &DataVisualizerWindow::Station_StatusChanged);
        disconnect(myStation.data(), &Station::labelsUpdate,  this, &DataVisualizerWindow::Station_LblsStates);
        disconnect(myStation.data(), &Station::plotNewPoint,  this, &DataVisualizerWindow::Plot_NewPoint);
        disconnect(myStation.data(), &Station::hoopErrorCode, this, &DataVisualizerWindow::Station_ErrorCode);
    }
    disconnect(DataVisualizerWindow::myDatabases.data(), &Manager::DatabaseInitialize, this, &DataVisualizerWindow::Database_Initialize);
    disconnect(DataVisualizerWindow::myDatabases.data(), &Manager::DatabaseConnection, this, &DataVisualizerWindow::Database_Connection);
    delete ui;
}

void DataVisualizerWindow::doLater() {
    connect(this->myActivePort.get(), &SerialPortReader::CheckSerialPort, this, &DataVisualizerWindow::Check_Status);
    connect(this->myActivePort.get(), &SerialPortReader::CheckSerialPort, this, &DataVisualizerWindow::SerialPort_Status);
    this->myActivePort->openPort();
    connect(this->mStatusTimer.get(), &QTimer::timeout, this, &DataVisualizerWindow::Check_Status);
    this->mStatusTimer->start(ms_);
    connect(DataVisualizerWindow::myDatabases.data(), &Manager::DatabaseInitialize, this, &DataVisualizerWindow::Database_Initialize);
    connect(DataVisualizerWindow::myDatabases.data(), &Manager::DatabaseConnection, this, &DataVisualizerWindow::Database_Connection);
    DataVisualizerWindow::myDatabases->initialize();
    DataVisualizerWindow::myDatabases->open();
}

void DataVisualizerWindow::Check_Status()  { this->ui->tabWidget->setEnabled(this->myActivePort->isActive() && DataVisualizerWindow::myDatabases->isOpen()); }

void DataVisualizerWindow::setStationsUI() {
    for(uint i = 1; i <= 6; i++) {
        double yAxisDesviationRead = 0.00;
        QString pressureColor, temperatureColor;
        plotSettings::loadSettings(yAxisDesviationRead, pressureColor, temperatureColor);
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
    try {
        DataVisualizerWindow::stationConfiguration(id_);
    }
    catch(...) {
        QMessageBox::warning(this, "Error", "Error al abrir la configuración de la estación");
    }
}

void DataVisualizerWindow::stationConfiguration(const uint ID_Station) {
    SetStation myHoopParameters(nullptr, &DataVisualizerWindow::myStations[ID_Station]);
    myHoopParameters.setModal(true);
    myHoopParameters.exec();
}

void DataVisualizerWindow::Station_StatusChanged(const Station::Status& myStatus) {
    Station* senderStation = qobject_cast<Station*>(sender());
    if(senderStation) {
        QPushButton* btnConfig = this->findChild<QPushButton*>("btnEstConfig_" + QString::number(senderStation->getID()));
        switch (myStatus) {
            case Station::Status::READY:
                if(btnConfig->isVisible())
                    btnConfig->setVisible(false);
                break;
            case Station::Status::RUNNING:
                if(btnConfig->isVisible())
                    btnConfig->setVisible(true);
                break;
            case Station::Status::WAITING:
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

    switch (myStatus) {
    case SerialPortReader::Status::OPEN:
        this->ui->serialConnect->setText("Cerrar Puerto");
        changeConnectionPort("Conectado", "Cerrar", StatusGreen);
        break;
    case SerialPortReader::Status::CLOSE:
        this->ui->serialConnect->setText("Abrir Puerto");
        changeConnectionPort("Desconectado", "Abrir", StatusRed);
        changeCommunicationPort("Cerrada",   StatusRed);
        break;
    case SerialPortReader::Status::ACTIVE:
        changeCommunicationPort("Abierta",   StatusGreen);
        break;
    case SerialPortReader::Status::INACTIVE:
        changeCommunicationPort("Cerrada",   StatusRed);
        break;
    }
}

void DataVisualizerWindow::Plot_ChangeStyle(const double &yAxisDesviation, const QString &pressureColor, const QString &temperatureColor) {
    for(auto myStation: DataVisualizerWindow::myStations) {
        PressureTempGraph* mygraph = this->findChild<PressureTempGraph*>("GraphE_" + QString::number(myStation->getID()));
        mygraph->changeStyle(yAxisDesviation, pressureColor, temperatureColor);
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

void DataVisualizerWindow::SetStation_Response(const SetStation::Response &response) {

}

void DataVisualizerWindow::Database_Initialize(const Manager::Status &v_Status, const QString &v_Error) {

}

void DataVisualizerWindow::Database_Connection(const Manager::Status &v_Status, const QString &v_Error) {
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
    case Manager::Status::ERROR:
        QMessageBox msgBox(QMessageBox::Warning, "Error", v_Error);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
        msgBox.exec();
        break;
    }
}

void DataVisualizerWindow::on_btnEstConfig_1_clicked() { this->btnStationsDialog(1); }

void DataVisualizerWindow::on_btnEstConfig_2_clicked() { this->btnStationsDialog(2); }

void DataVisualizerWindow::on_btnEstConfig_3_clicked() { this->btnStationsDialog(3); }

void DataVisualizerWindow::on_btnEstConfig_4_clicked() { this->btnStationsDialog(4); }

void DataVisualizerWindow::on_btnEstConfig_5_clicked() { this->btnStationsDialog(5); }

void DataVisualizerWindow::on_btnEstConfig_6_clicked() { this->btnStationsDialog(6); }

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

void DataVisualizerWindow::on_actionGr_fico_triggered() {
    plotSettings* myPlotSettings = new plotSettings();
    connect(myPlotSettings, &plotSettings::changeStyle, this, &DataVisualizerWindow::Plot_ChangeStyle);
    myPlotSettings->setModal(true);
    myPlotSettings->exec();
    disconnect(myPlotSettings, &plotSettings::changeStyle, this, &DataVisualizerWindow::Plot_ChangeStyle);
    delete myPlotSettings;
}

void DataVisualizerWindow::on_dbConnect_triggered() {
    if(DataVisualizerWindow::myDatabases->isOpen()) { DataVisualizerWindow::myDatabases->close(); return; }
    DataVisualizerWindow::myDatabases->open();
}

void DataVisualizerWindow::on_close_triggered() { this->close(); }
