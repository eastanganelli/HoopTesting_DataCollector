#ifndef SETSTATION_H
#define SETSTATION_H
#include <QDialog>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QMessageBox>
#include <QList>
#include <QStringList>

#include "utils/station.h"

typedef enum { Save, Cancel } StationResponse;
namespace Ui { class SetStation; }

using namespace FrontClases;
class SetStation : public QDialog {
    Q_OBJECT

    unsigned int hooppressure();
    void preLoadData();
    void setConnectionSignals();
    void checkSpecimen();
    void isPopulated();
    void configureStation();

    static void inputPressureFixed(float pressure_, QSpinBox* input_);
    static void clearComboBox(QComboBox* myWidget, QString text, bool state);

    Ui::SetStation *ui;
    QSharedPointer<Schemas::Static> normsDB;
    QSharedPointer<Schemas::Data>   dataDB;
    QSharedPointer<Station> selectedStation;
    uint idSample;
    QSharedPointer<NodeStandard>      selectedStandard;
    QSharedPointer<NodeMaterial>      selectedMaterial;
    QSharedPointer<NodeSpecification> selectedSpecification;
    QSharedPointer<NodeEndCap>        selectedEndCap;
    QSharedPointer<NodeTestType>      selectedTestType;
    QSharedPointer<NodeOperator>      selectedOperator;
    QSharedPointer<NodeEnviroment>    selectedEnviroment;
    QList<QSharedPointer<NodeStandard>>          listStandards;
    QList<QSharedPointer<NodeMaterial>>          listMaterials;
    QList<QSharedPointer<NodeSpecification>>     listSpecifications;
    QList<QSharedPointer<NodeConditionalPeriod>> listCondPeriods;
    QList<QSharedPointer<NodeSetting>>           listSettings;
    QList<QSharedPointer<NodeEndCap>>            listEndCap;
    QList<QSharedPointer<NodeTestType>>          listTestTypes;
    QList<QSharedPointer<NodeOperator>>          listOperators;
    QList<QSharedPointer<NodeEnviroment>>        listEnviroments;

private slots:
    void checkFieldsCompletetion();
    void on_inputWallThickness_valueChanged(int wallthickness);
    void on_inputPressure_valueChanged(int Pressure);
    void on_inputLenFree_valueChanged(int value_);
    void on_cbStandard_currentIndexChanged(int index);
    void on_cbMaterial_currentIndexChanged(int index);
    void on_cbSpecification_currentIndexChanged(int index);
    void on_cbBoxOperator_currentIndexChanged(int index);
    void on_cbBoxTestType_currentIndexChanged(int index);
    void on_cbBoxEnviroment_currentIndexChanged(int index);
    void on_btnSave_clicked();
    void on_btnCancel_clicked();

public:
    explicit SetStation(QWidget *parent = nullptr);
    ~SetStation();
    void sharePointer(QSharedPointer<Schemas::Data> dataDB = nullptr, QSharedPointer<Station> selectedStation = nullptr);
};
#endif // SETSTATION_H
