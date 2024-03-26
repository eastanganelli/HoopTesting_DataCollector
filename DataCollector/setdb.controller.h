#ifndef SETDB_H
#define SETDB_H
#include <QDialog>
#include <QMessageBox>
#include "defines.h"
#include "database.service.h"

namespace Ui { class DBConfig; }
class DBConfig : public QDialog {
    Q_OBJECT

    Ui::DBConfig *ui;
    Database myDB;
private slots:
    void on_btnDBTest_clicked();
    void on_btnSave_clicked();
    void on_btnCancel_clicked();
    void dataIsComplete();
public:
    explicit DBConfig(QWidget *parent = nullptr);
    ~DBConfig();
};
#endif // SETDB_H
