#ifndef DATAVISUALIZER_H
#define DATAVISUALIZER_H
#include <QMainWindow>
#include <QTimer>

#include "services/database.h"
#include "services/serialportmanager.h"

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
    void btnStationStartStop(const uint id_);

    Ui::DataVisualizerWindow *ui;
    QSharedPointer<QTimer> mStatusTimer;
    QSharedPointer<Schemas::Data> myDataDB;
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
    void on_btnEstRun_1_clicked();
    void on_btnEstRun_2_clicked();
    void on_btnEstRun_3_clicked();
    void on_btnEstRun_4_clicked();
    void on_btnEstRun_5_clicked();
    void on_btnEstRun_6_clicked();
    void on_serialConfig_triggered();
    void on_serialConnect_triggered();
    void on_close_triggered();
    void on_dbConfig_triggered();
    void statusConnections();
    void on_dbConnect_triggered();

public:
    DataVisualizerWindow(QWidget *parent = nullptr);
    ~DataVisualizerWindow();
};
#endif // DATAVISUALIZER_H
