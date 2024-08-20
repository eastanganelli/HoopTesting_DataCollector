#include <QApplication>
#include <QColorDialog>
#include <QSettings>
#include <QmessageBox>
#include "generalsettings.h"
#include "ui_generalsettings.h"

generalSettings::generalSettings(QWidget *parent) : QDialog(parent), ui(new Ui::generalSettings) {
    ui->setupUi(this);
    uint timeoutTest = 2;
    double yaxisDesviation = 0.00;
    QString pressureColor, temperatureColor;
    this->loadSettingsPlot(yaxisDesviation, pressureColor, temperatureColor);
    this->loadSettingsStation(timeoutTest);
    this->ui->sboxDesviationYAxis->setValue(yaxisDesviation);
    this->ui->inputPressureColor->setText(pressureColor.isEmpty() ? QColor(Qt::green).name() : pressureColor);
    this->ui->colorBoxPressure->setStyleSheet("background-color: " + (pressureColor.isEmpty() ? QColor(Qt::green).name() : pressureColor) + ";");
    this->ui->inputTemperatureColor->setText(temperatureColor.isEmpty() ? QColor(Qt::red).name() : temperatureColor);
    this->ui->colorBoxTemperature->setStyleSheet("background-color: " + (temperatureColor.isEmpty() ? QColor(Qt::red).name() : temperatureColor) + ";");
    this->ui->inputTimeoutTest->setValue(timeoutTest);
}

generalSettings::~generalSettings() { delete ui; }

void generalSettings::saveSettingsPlot(const double yaxisDesviation, const QString &pressureColor, const QString &temperatureColor) {
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("PlotSettings");
    mySettings.setValue("YAxisDesviations", yaxisDesviation);
    mySettings.setValue("PressureColor",    pressureColor);
    mySettings.setValue("TemperatureColor", temperatureColor);
    mySettings.endGroup();
}

void generalSettings::saveSettingsStation(const uint &timeoutTest) {
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("StationSettings");
    mySettings.setValue("TimeoutTest", timeoutTest);
    mySettings.endGroup();
}

void generalSettings::isComplete() {
    if(!this->ui->inputPressureColor->text().isEmpty() && !this->ui->inputTemperatureColor->text().isEmpty()) { this->ui->btnSave->setEnabled(true); }
    else { this->ui->btnSave->setEnabled(false); }
}

void generalSettings::setDefault() {
    this->ui->sboxDesviationYAxis->setValue(0);
    this->ui->inputPressureColor->setText(QColor(Qt::green).name());
    this->ui->colorBoxPressure->setStyleSheet("background-color: " + QColor(Qt::green).name() + ";");
    this->ui->inputTemperatureColor->setText(QColor(Qt::red).name());
    this->ui->colorBoxTemperature->setStyleSheet("background-color: " + QColor(Qt::red).name() + ";");
}

void generalSettings::loadSettingsPlot(double& yaxisDesviation, QString& pressureColor, QString& temperatureColor) {
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("PlotSettings"); {
        yaxisDesviation  = mySettings.value("YAxisDesviations", QString()).toDouble();
        pressureColor    = mySettings.value("PressureColor",    QString()).toString();
        temperatureColor = mySettings.value("TemperatureColor", QString()).toString();
    }
    mySettings.endGroup();
}

void generalSettings::loadSettingsStation(uint &timeoutTest) {
    QSettings mySettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    mySettings.beginGroup("StationSettings");
    timeoutTest = mySettings.value("TimeoutTest", QString()).toUInt();
    mySettings.endGroup();
    if(timeoutTest == 0) { timeoutTest = 2; }
}

void generalSettings::on_btnPressureColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        ui->colorBoxPressure->setStyleSheet("background-color: " + color.name());
        ui->inputPressureColor->setText(color.name());
    }
}

void generalSettings::on_btnTemperatureColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        ui->colorBoxTemperature->setStyleSheet("background-color: " + color.name());
        ui->inputTemperatureColor->setText(color.name());
    }
}

void generalSettings::on_btnSave_clicked() {
    QMessageBox msgBox(QMessageBox::Warning, "Configuración", "Desea guardar cambios?");
    msgBox.addButton(QMessageBox::Yes)->setText(tr("Si"));
    msgBox.addButton(QMessageBox::No)->setText(tr("No"));
    switch(msgBox.exec()) {
    case QMessageBox::Yes : {
        emit this->changeStyle(this->ui->sboxDesviationYAxis->value(), this->ui->inputPressureColor->text(), this->ui->inputTemperatureColor->text());
        generalSettings::saveSettingsPlot(this->ui->sboxDesviationYAxis->value(), this->ui->inputPressureColor->text(), this->ui->inputTemperatureColor->text());
        generalSettings::saveSettingsStation(this->ui->inputTimeoutTest->value());
        this->close();
        break;
    }
    default: break;
    }
}

void generalSettings::on_btnCancel_clicked() { this->close(); }

void generalSettings::on_inputPressureColor_textChanged(const QString &arg1) { this->isComplete(); }

void generalSettings::on_inputTemperatureColor_textChanged(const QString &arg1) { this->isComplete(); }

void generalSettings::on_btnDefaultValues_clicked() { this->setDefault(); }
