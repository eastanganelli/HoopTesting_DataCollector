#ifndef DATAVISUALIZER_H
#define DATAVISUALIZER_H
#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include <iomanip>
#include "defines.h"
#include "setstation.controller.h"
#include "setserial.controller.h"
#include "setdb.controller.h"
#include "setstandards.controller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DataVisualizerWindow; }
QT_END_NAMESPACE

class DataVisualizerWindow : public QMainWindow {
    Q_OBJECT

    void setStationsUI();
    void initStatusBar();
    void btnsStates(const bool state_);
    bool btnsStates();
    void btnStationsDialog(const unsigned int id_, QPushButton* btnState);

    Ui::DataVisualizerWindow *ui;
    QSharedPointer<QTimer> mStatusTimer;
    QSharedPointer<Database> myDataDB;
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
    void on_serialConnect_triggered();
    void on_close_triggered();
    void on_dbConfig_triggered();

    void on_paramConfig_triggered();
    void statusConnections();
    void on_dbConnect_triggered();

public:
    DataVisualizerWindow(QWidget *parent = nullptr);
    ~DataVisualizerWindow();
};
#endif // DATAVISUALIZER_H
