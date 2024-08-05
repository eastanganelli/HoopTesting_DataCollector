#ifndef SETSERIAL_H
#define SETSERIAL_H
#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QSettings>
#include <QMessageBox>

namespace Ui { class serialConfig; }

class serialConfig : public QDialog {
    Q_OBJECT

    Ui::serialConfig *ui;

    void loadSaveData();

private slots:
    void on_btnTest_clicked();
    void on_btnSave_clicked();
    void on_btnCancel_clicked();

public:
    explicit serialConfig(QWidget *parent = nullptr);
    ~serialConfig();
};
#endif // SETSERIAL_H
