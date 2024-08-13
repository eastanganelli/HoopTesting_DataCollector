#ifndef SETSTATION_H
#define SETSTATION_H
#include <QList>
#include <QDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QStringList>
#include <QSqlDatabase>
#include <QSharedPointer>

#include "../utils/station.h"

typedef enum { Save, Cancel } StationResponse;
namespace Ui { class SetStation; }

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
    QSharedPointer<Station>* myStation;
    QSqlDatabase staticDatabase, cacheDatabase;
    QJsonObject StaticData;
    // QSharedPointer<NodeStandard>      selectedStandard;
    // QSharedPointer<NodeMaterial>      selectedMaterial;
    // QSharedPointer<NodeSpecification> selectedSpecification;
    // QSharedPointer<NodeSetting>       selectedSetting;
    // QSharedPointer<NodeEndCap>        selectedEndCap;
    // QSharedPointer<NodeTestType>      selectedTestType;
    // QSharedPointer<NodeOperator>      selectedOperator;
    // QSharedPointer<NodeEnviroment>    selectedEnviroment;

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
    void on_cbBoxTestTime_currentIndexChanged(int index);
    void on_btnSave_clicked();
    void on_btnCancel_clicked();

public:
    enum class Response { SaveExport, EscExport };

    explicit SetStation(QWidget *parent = nullptr, QSharedPointer<Station>* selectedStation = nullptr);
    ~SetStation();

    Q_SIGNAL void setHoopParameters(const SetStation::Response& response);
};
#endif // SETSTATION_H
