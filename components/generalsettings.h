#ifndef GENERALSETTINGS_H
#define GENERALSETTINGS_H
#include <QDialog>

class Station;

namespace Ui { class generalSettings; }

class generalSettings : public QDialog {
    Q_OBJECT

public:
    static void loadSettingsPlot(double &yaxisDesviation, QString &pressureColor, QString &temperatureColor);
    static void loadSettingsStation(uint& timeoutTest);

    explicit generalSettings(QWidget *parent = nullptr);
    ~generalSettings();

    Q_SIGNAL void changeStyle(const double& yaxisDesviation, const QString& pressureColor, const QString& temperatureColor);
    Q_SIGNAL void changeTimeout(const uint& timeoutTest);

private slots:
    void on_btnPressureColor_clicked();
    void on_btnTemperatureColor_clicked();
    void on_btnSave_clicked();
    void on_btnCancel_clicked();
    void on_inputPressureColor_textChanged(const QString &arg1);
    void on_inputTemperatureColor_textChanged(const QString &arg1);
    void on_btnDefaultValues_clicked();

private:
    static void saveSettingsPlot(const double yaxisDesviation, const QString &pressureColor, const QString &temperatureColor);
    static void saveSettingsStation(const uint& timeoutTest);

    void isComplete();
    void setDefault();

    Ui::generalSettings *ui;
};

#endif // GENERALSETTINGS_H
