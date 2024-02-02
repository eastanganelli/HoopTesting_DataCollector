#include "setstandards.controller.h"
#include "ui_setstandards.controller.h"

configDBNorms::configDBNorms(QWidget *parent) : QDialog(parent), ui(new Ui::configDBNorms) {
    ui->setupUi(this);
}

configDBNorms::~configDBNorms() {
    delete ui;
}

void configDBNorms::testingQuery() {
}
