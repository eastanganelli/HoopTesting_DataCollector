#ifndef SETSTATION_H
#define SETSTATION_H
#include <QDialog>
#include <QScopedPointer>
#include <QMessageBox>
#include <QStringList>
#include "defines.h"
#include "serialportmanager.controller.h"

typedef enum { Save, Cancel } StationResponse;
namespace Ui { class SetStation; }

using namespace FrontClases;
class SetStation : public QDialog {
    Q_OBJECT

    unsigned int hooppressure();
    static void inputPressureFixed(float pressure_, QSpinBox* input_);
    static void clearComboBox(QComboBox* myWidget, QString text, bool state);
    void loadStandardCombobox();
    void setConnectionSignals();
    void checkSpecimen();
    void configureStation();

    Ui::SetStation *ui;
    QSharedPointer<Schemas::Static> normsDB;
    QSharedPointer<Schemas::Data>   dataDB;
    QSharedPointer<Station> selectedStation;
    uint idSample;
    StationResponse* response;
    QSharedPointer<SerialPortReader> portWriting;
    QSharedPointer<NodeStandard>      selectedStandard;
    QSharedPointer<NodeMaterial>      selectedMaterial;
    QSharedPointer<NodeSpecification> selectedSpecification;
    QList<QSharedPointer<NodeStandard>>          listStandards;
    QList<QSharedPointer<NodeMaterial>>          listMaterials;
    QList<QSharedPointer<NodeSpecification>>     listSpecifications;
    QList<QSharedPointer<NodeConditionalPeriod>> listCondPeriods;
    QList<QSharedPointer<NodeSetting>>           listSettings;

private slots:
    void checkFieldsCompletetion();
    void on_inputWallThickness_valueChanged(int wallthickness);
    void on_inputPressure_valueChanged(int Pressure);
    void on_inputLenFree_valueChanged(int value_);
    void on_cbStandard_currentIndexChanged(int index);
    void on_cbMaterial_currentIndexChanged(int index);
    void on_cbSpecification_currentIndexChanged(int index);
    void on_btnSave_clicked();
    void on_btnCancel_clicked();

public:
    explicit SetStation(QWidget *parent = nullptr);
    ~SetStation();
    void sharePointer(StationResponse* response, QSharedPointer<Schemas::Data> dataDB = nullptr, QSharedPointer<SerialPortReader> myPort = nullptr, QSharedPointer<Station> selectedStation = nullptr);
};
#endif // SETSTATION_H
