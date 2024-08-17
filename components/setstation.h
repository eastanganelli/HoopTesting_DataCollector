#ifndef SETSTATION_H
#define SETSTATION_H
#include <QList>
#include <QDialog>
#include <QThread>
#include <QSpinBox>
#include <QComboBox>
#include <QJsonArray>
#include <QMessageBox>
#include <QStringList>
#include <QSqlDatabase>
#include <QSharedPointer>
#include "../services/database.h"

class DataVisualizerWindow;

namespace Ui { class SetStation; }


class SetStation : public QDialog {
    Q_OBJECT

    uint hooppressure();
    void SetSignals();
    void configureStation();

    static void inputPressureFixed(float pressure_, QSpinBox* input_);
    static void clearComboBox(QComboBox* myWidget, QString text, bool state);

    Ui::SetStation *ui;
    uint ID_Station, ID_Test;
    int indexStandard, indexMaterial, indexSpecification;
    QSharedPointer<Manager> myManager;
    QJsonArray standards, operators;

private slots:
    void checkSpecimen();
    void checkFieldsCompletetion();
    void on_cbStandard_currentIndexChanged(int index);
    void on_cbMaterial_currentIndexChanged(int index);
    void on_cbSpecification_currentIndexChanged(int index);
    void on_inputWallThickness_valueChanged(int wallthickness);
    void on_inputPressure_valueChanged(int Pressure);
    void on_inputLenFree_valueChanged(int value_);
    void on_btnSave_clicked();
    void on_btnCancel_clicked();
    void on_btnSaveExport_clicked();
    void on_btnEscExport_clicked();

public:
    enum class Response { Save, Cancel, Export };

    explicit SetStation(QWidget *parent = nullptr);
    void setParameters(const uint &ID_Station_, const uint &ID_Test_, const SetStation::Response& mode);
    ~SetStation();

    static void stationConfiguration(const uint& ID_Station, const uint& ID_Test, const SetStation::Response& v_mode);

    Q_SIGNAL void updateTest(const uint testID, const QString& standard, const QString& material, const QString& specification, const uint lenTotal, const uint lenFree, const uint diamNom, const uint diamReal, const uint thickness, const QString& testType, const QString& operatorName, const QString& endCap, const QString& enviroment, const QString& conditionalPeriod, const uint& pressureTarget, const uint& temperatureTarget);
    Q_SIGNAL void exportTest(const uint& testID);

private:
    void preLoadUI();
    void preLoadData();
};
#endif // SETSTATION_H
