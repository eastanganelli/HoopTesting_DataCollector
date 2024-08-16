#ifndef DATAVISUALIZER_H
#define DATAVISUALIZER_H
#include <QMainWindow>
#include <QTimer>

#include "setstation.h"
#include "../services/database.h"
#include "../services/serialportmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DataVisualizerWindow; }
QT_END_NAMESPACE

class DataVisualizerWindow : public QMainWindow {
    Q_OBJECT

    void setStationsUI();
    void initStatusBar();
    void btnStationsDialog(const uint id_);

    Ui::DataVisualizerWindow *ui;
    QSharedPointer<SerialPortReader> myActivePort;
    QLabel* ConnectionDataBase, * ConnectionPort, * PortStatus;
    bool myBtnsStates;

    Q_SIGNAL void openDialog(const uint& ID_Station, const uint& ID_Test, const SetStation::Response& v_mode);

private slots:
    void doLater();

    void openDialogWindow(const uint& ID_Station, const uint& ID_Test, const SetStation::Response& v_mode);
    void Check_Status();
    void Station_StatusChanged(const Station::Status& myStatus);
    void Station_ErrorCode(const int codeError);
    void Station_LblsStates(const uint key, const double pressure, const double temperature);
    void SerialPort_Status(const SerialPortReader::Status& myStatus);
    void Plot_ChangeStyle(const double &yAxisDesviation, const QString &pressureColor, const QString &temperatureColor);
    void Database_Initialize(const Manager::Status& v_Status, const QString& v_Error);
    void Database_Connection(const Manager::Status& v_Status, const QString& v_Error);

    void on_btnEstConfig_1_clicked();
    void on_btnEstConfig_2_clicked();
    void on_btnEstConfig_3_clicked();
    void on_btnEstConfig_4_clicked();
    void on_btnEstConfig_5_clicked();
    void on_btnEstConfig_6_clicked();
    void on_serialConfig_triggered();
    void on_serialConnect_triggered();
    void on_close_triggered();
    void on_dbConfig_triggered();
    void on_dbConnect_triggered();
    void on_actionGr_fico_triggered();

public:
    DataVisualizerWindow(QWidget *parent = nullptr);
    ~DataVisualizerWindow();

public slots:
    void SetStation_FreeStation();
};
#endif // DATAVISUALIZER_H
