#include <QApplication>
#include <QColorDialog>
#include <QSettings>
#include <QmessageBox>
#include "plotsettings.h"
#include "ui_plotsettings.h"

plotSettings::plotSettings(QWidget *parent) : QDialog(parent), ui(new Ui::plotSettings) {
    ui->setupUi(this);
    bool activeDesviation = false;
    uint minValuesDesviation = 0;
    double pressureDesviation = 0.00, yaxisDesviation = 0.00;
    QString pressureColor, temperatureColor;
    this->loadSettings(yaxisDesviation, pressureColor, temperatureColor);
    this->ui->sboxDesviationYAxis->setValue(yaxisDesviation);
    this->ui->inputPressureColor->setText(pressureColor.isEmpty() ? QColor(Qt::green).name() : pressureColor);
    this->ui->colorBoxPressure->setStyleSheet("background-color: " + (pressureColor.isEmpty() ? QColor(Qt::green).name() : pressureColor) + ";");
    this->ui->inputTemperatureColor->setText(temperatureColor.isEmpty() ? QColor(Qt::red).name() : temperatureColor);
    this->ui->colorBoxTemperature->setStyleSheet("background-color: " + (temperatureColor.isEmpty() ? QColor(Qt::red).name() : temperatureColor) + ";");
}

plotSettings::~plotSettings() { delete ui; }

void plotSettings::saveSettings(const double yaxisDesviation, const QString &pressureColor, const QString &temperatureColor) {
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("PlotSettings");
    mySettings.setValue("YAxisDesviations",    yaxisDesviation);
    mySettings.setValue("PressureColor",       pressureColor);
    mySettings.setValue("TemperatureColor",    temperatureColor);
    mySettings.endGroup();
}

void plotSettings::isComplete() {
    if(!this->ui->inputPressureColor->text().isEmpty() && !this->ui->inputTemperatureColor->text().isEmpty()) { this->ui->btnSave->setEnabled(true); }
    else { this->ui->btnSave->setEnabled(false); }
}

void plotSettings::setDefault() {
    this->ui->sboxDesviationYAxis->setValue(0);
    this->ui->inputPressureColor->setText(QColor(Qt::green).name());
    this->ui->colorBoxPressure->setStyleSheet("background-color: " + QColor(Qt::green).name() + ";");
    this->ui->inputTemperatureColor->setText(QColor(Qt::red).name());
    this->ui->colorBoxTemperature->setStyleSheet("background-color: " + QColor(Qt::red).name() + ";");
}

void plotSettings::loadSettings(double& yaxisDesviation, QString& pressureColor, QString& temperatureColor) {
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("PlotSettings"); {
        yaxisDesviation    = mySettings.value("YAxisDesviations",    QString()).toDouble();
        pressureColor      = mySettings.value("PressureColor",       QString()).toString();
        temperatureColor   = mySettings.value("TemperatureColor",    QString()).toString();
    }
    mySettings.endGroup();
}

void plotSettings::on_btnPressureColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        ui->colorBoxPressure->setStyleSheet("background-color: " + color.name());
        ui->inputPressureColor->setText(color.name());
    }
}

void plotSettings::on_btnTemperatureColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        ui->colorBoxTemperature->setStyleSheet("background-color: " + color.name());
        ui->inputTemperatureColor->setText(color.name());
    }
}

void plotSettings::on_btnSave_clicked() {
    QMessageBox msgBox(QMessageBox::Warning, "Configuración", "Desea guardar cambios?");
    msgBox.addButton(QMessageBox::Yes)->setText(tr("Si"));
    msgBox.addButton(QMessageBox::No)->setText(tr("No"));
    switch(msgBox.exec()) {
    case QMessageBox::Yes : {
        plotSettings::saveSettings(this->ui->sboxDesviationYAxis->value(), this->ui->inputPressureColor->text(), this->ui->inputTemperatureColor->text());
        // for(QSharedPointer<Station> myStation : DataVisualizerWindow::myStations) {
        //     myStation->reloadPlotSettings();
        // }
        this->close();
        break;
    }
    default: break;
    }
}

void plotSettings::on_btnCancel_clicked() { this->close(); }

void plotSettings::on_inputPressureColor_textChanged(const QString &arg1) { this->isComplete(); }

void plotSettings::on_inputTemperatureColor_textChanged(const QString &arg1) { this->isComplete(); }

void plotSettings::on_btnDefaultValues_clicked() { this->setDefault(); }
