#ifndef DATAVISUALIZER_H
#define DATAVISUALIZER_H
#include <QMainWindow>
#include <QTimer>

#include "../services/database.h"
#include "../services/serialportmanager.h"

#include "setstation.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DataVisualizerWindow; }
QT_END_NAMESPACE

class DataVisualizerWindow : public QMainWindow {
    Q_OBJECT

    void setStationsUI();
    void initStatusBar();
    void btnsStates(const bool state_);
    bool btnsStates();
    void btnStationsDialog(const uint id_);
    void btnStationSaveClear(const uint id_);

    static void stationConfiguration(const uint ID_Station);

    Ui::DataVisualizerWindow *ui;
    QSharedPointer<QTimer> mStatusTimer;
    // QSharedPointer<Schemas::Data> myDataDB;
    QSharedPointer<SerialPortReader> myActivePort;
    SetStation* dialogStation;
    QLabel* ConnectionDataBase,
          * ConnectionPort,
          * PortStatus;
    bool myBtnsStates;

private slots:
    void on_btnEstConfig_1_clicked();
    void on_btnEstConfig_2_clicked();
    void on_btnEstConfig_3_clicked();
    void on_btnEstConfig_4_clicked();
    void on_btnEstConfig_5_clicked();
    void on_btnEstConfig_6_clicked();
    void on_serialConfig_triggered();
    void on_btnSvClr_1_clicked();
    void on_btnSvClr_2_clicked();
    void on_btnSvClr_3_clicked();
    void on_btnSvClr_4_clicked();
    void on_btnSvClr_5_clicked();
    void on_btnSvClr_6_clicked();
    void on_serialConnect_triggered();
    void on_close_triggered();
    void on_dbConfig_triggered();
    void statusConnections();
    void on_dbConnect_triggered();
    void on_actionGr_fico_triggered();

public:
    DataVisualizerWindow(QWidget *parent = nullptr);
    ~DataVisualizerWindow();

private:
    friend class SerialPortReader;
    friend class SetStation;
    friend class plotSettings;
    static QMap<uint, QSharedPointer<Station>> myStations;
    static QSharedPointer<Manager> myDatabases;
};
#endif // DATAVISUALIZER_H
