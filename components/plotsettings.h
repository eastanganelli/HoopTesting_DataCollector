#ifndef PLOTSETTINGS_H
#define PLOTSETTINGS_H
#include <QDialog>
#include "datavisualizer.h"

class DataVisualizerWindow;
class Station;

namespace Ui { class plotSettings; }

class plotSettings : public QDialog {
    Q_OBJECT

public:
    // static void loadSettings(bool &activeDesviation, double &pressureDesviation, uint &minValuesDesviation, double &yaxisDesviation, QString &pressureColor, QString &temperatureColor);
    static void loadSettings(double &yaxisDesviation, QString &pressureColor, QString &temperatureColor);

    explicit plotSettings(QWidget *parent = nullptr);
    ~plotSettings();

private slots:
    void on_btnPressureColor_clicked();
    void on_btnTemperatureColor_clicked();
    void on_btnSave_clicked();
    void on_btnCancel_clicked();
    void on_inputPressureColor_textChanged(const QString &arg1);
    void on_inputTemperatureColor_textChanged(const QString &arg1);
    void on_btnDefaultValues_clicked();

private:
    // static void saveSettings(const bool activeDesviation, const double pressureDesviation, const uint minValuesDesviation, const double yaxisDesviation, const QString &pressureColor, const QString &temperatureColor);
    static void saveSettings(const double yaxisDesviation, const QString &pressureColor, const QString &temperatureColor);

    void isComplete();
    void setDefault();

    static void StationsPlotReload(const double yaxisDesviation, const QString &pressureColor, const QString &temperatureColor);

    Ui::plotSettings *ui;
};

#endif // PLOTSETTINGS_H
