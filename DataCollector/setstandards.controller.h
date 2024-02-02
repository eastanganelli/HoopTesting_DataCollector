#ifndef SETSTANDARDS_H
#define SETSTANDARDS_H
#include <QDialog>
#include "defines.h"

namespace Ui { class configDBNorms; }

class configDBNorms : public QDialog {
    Q_OBJECT
public:
    explicit configDBNorms(QWidget *parent = nullptr);
    ~configDBNorms();
    void testingQuery();
private:
    Ui::configDBNorms *ui;
    //DBTreeNorm* myData;
};
#endif // SETSTANDARDS_H
