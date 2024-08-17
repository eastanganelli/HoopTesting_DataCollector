#include "setstation.h"
#include "components/ui_setstation.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QJsonDocument>
#include <QJsonObject>

SetStation::SetStation(QWidget *parent): QDialog(parent), ui(new Ui::SetStation) {
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::Desktop);
    this->myManager = Manager::myDatabases;
    this->indexStandard = this->indexMaterial = this->indexSpecification = -1;
}

SetStation::~SetStation() {
    disconnect(this, &SetStation::updateTest, Manager::myDatabases.data(), &Manager::updateTest);
    disconnect(this, &SetStation::exportTest, Manager::myDatabases.data(), &Manager::exportTestData);
    delete ui;
}

void SetStation::stationConfiguration(const uint& ID_Station, const uint& ID_Test, const SetStation::Response& v_mode) {
    SetStation myHoopParameters;
    myHoopParameters.setParameters(ID_Station, ID_Test, v_mode);
    // myHoopParameters.setWindowModality(Qt::ApplicationModal);
    myHoopParameters.setModal(true);
    myHoopParameters.exec();
}

void SetStation::setParameters(const uint& ID_Station_, const uint& ID_Test_, const Response &mode) {
    this->ID_Station = ID_Station_;
    this->ID_Test    = ID_Test_;
    if(mode == SetStation::Response::Export) {
        this->ui->btnSave->setVisible(false);
        this->ui->btnCancel->setVisible(false);
    } else if(mode == SetStation::Response::Cancel || mode == SetStation::Response::Save) {
        this->ui->btnSaveExport->setVisible(false);
        this->ui->btnEscExport->setVisible(false);
    }
    this->preLoadUI();
    this->preLoadData();
    this->SetSignals();
    connect(this->ui->cbStandard,         SIGNAL(currentIndexChanged(int)), this, SLOT(checkSpecimen()));
    connect(this->ui->cbMaterial,         SIGNAL(currentIndexChanged(int)), this, SLOT(checkSpecimen()));
    connect(this->ui->cbSpecification,    SIGNAL(currentIndexChanged(int)), this, SLOT(checkSpecimen()));
    connect(this->ui->inputDiamNormal,    SIGNAL(textChanged(QString)),     this, SLOT(checkSpecimen()));
    connect(this->ui->inputDiamReal,      SIGNAL(textChanged(QString)),     this, SLOT(checkSpecimen()));
    connect(this->ui->inputLenFree,       SIGNAL(textChanged(QString)),     this, SLOT(checkSpecimen()));
    connect(this->ui->inputLenTotal,      SIGNAL(textChanged(QString)),     this, SLOT(checkSpecimen()));
    connect(this->ui->inputWallThickness, SIGNAL(textChanged(QString)),     this, SLOT(checkSpecimen()));
    connect(this, &SetStation::updateTest, Manager::myDatabases.data(), &Manager::updateTest);
    connect(this, &SetStation::exportTest, Manager::myDatabases.data(), &Manager::exportTestData);
}

void SetStation::preLoadData() {
    try {
        QSqlQuery myResult = this->myManager->selectTest(QString("SELECT * FROM test t WHERE t.id = %1;").arg(QString::number(this->ID_Test)), "cachedDatabase");
        QString standard = myResult.value("standard").toString(), material = myResult.value("material").toString(), specification = myResult.value("specification").toString(),
                operatorName = myResult.value("operator").toString(), endCap = myResult.value("endCap").toString(), testType = myResult.value("testType").toString(),
                enviroment = myResult.value("enviroment").toString();

        if(!(standard.isEmpty())) {
            for(uint i = 0; i < (uint)this->ui->cbStandard->count(); i++) {
                if(this->ui->cbStandard->itemText(i) == standard) {
                    this->ui->cbStandard->setCurrentIndex(i);
                    break;
                }
            }
        }
        if(!(material.isEmpty())) {
            for(uint i = 0; i < (uint)this->ui->cbMaterial->count(); i++) {
                if(this->ui->cbMaterial->itemText(i) == material) {
                    this->ui->cbMaterial->setCurrentIndex(i);
                    break;
                }
            }
        }
        if(!(specification.isEmpty())) {
            for(uint i = 0; i < (uint)this->ui->cbSpecification->count(); i++) {
                if(this->ui->cbSpecification->itemText(i) == specification) {
                    this->ui->cbSpecification->setCurrentIndex(i);
                    break;
                }
            }
        }
        if(!(enviroment.isNull())) {
            for(uint i = 0; i < (uint)this->ui->cbBoxEnviroment->count(); i++) {
                if(this->ui->cbBoxEnviroment->itemText(i) == enviroment) {
                    this->ui->cbBoxEnviroment->setCurrentIndex(i);
                    break;
                }
            }
        }
        if(!(endCap.isNull())) {
            for(uint i = 0; i < (uint)this->ui->cbBoxEndCap->count(); i++) {
                if(this->ui->cbBoxEndCap->itemText(i) == endCap) {
                    this->ui->cbBoxEndCap->setCurrentIndex(i);
                    break;
                }
            }
        }
        if(!(testType.isNull())) {
            for(uint i = 0; i < (uint)this->ui->cbBoxTestType->count(); i++) {
                if(this->ui->cbBoxTestType->itemText(i) == testType) {
                    this->ui->cbBoxTestType->setCurrentIndex(i);
                    break;
                }
            }
        }
        if(!(operatorName.isNull())) {
            for(uint i = 0; i < (uint)this->ui->cbBoxOperator->count(); i++) {
                if(this->ui->cbBoxOperator->itemText(i) == operatorName) {
                    this->ui->cbBoxOperator->setCurrentIndex(i);
                    break;
                }
            }
        }
        this->ui->inputLenTotal->setValue(myResult.value("lenTotal").toInt());
        this->ui->inputLenFree->setValue(myResult.value("lenFree").toInt());
        this->ui->inputDiamNormal->setValue(myResult.value("diameterNormal").toInt());
        this->ui->inputDiamReal->setValue(myResult.value("diameterReal").toInt());
        this->ui->inputWallThickness->setValue(myResult.value("wallThickness").toInt());
        this->ui->cbTemp->setCurrentText(myResult.value("temperatureTarget").toString());
        this->ui->inputPressure->setValue(myResult.value("pressureTarget").toInt());
        this->checkFieldsCompletetion();
    } catch(ManagerErrors::QuerySelectError& ex) {
        qDebug() << ex.what();
    }
}

void SetStation::preLoadUI() {
    try {
        QSqlQuery myResult = this->myManager->selectTest(QString("SELECT selectOperators_Into_JSON() AS operators, selectStandard_Into_JSON() AS standards;"), "RemoteStatic");
        this->standards = QJsonDocument::fromJson(myResult.record().value("standards").toByteArray()).array();
        this->operators = QJsonDocument::fromJson(myResult.record().value("operators").toByteArray()).array();
    } catch(ManagerErrors::QuerySelectError& ex) {
        qDebug() << ex.what();
    }
    this->ui->gpParameters->setTitle(QString("Estación Nro.: %1").arg(QString::number(this->ID_Station)));
    this->ui->lblSpecimen->setText("Prueba Nro.: 0");
    this->ui->gpTestTime->setVisible(false);
    if(this->ID_Station < 4)
        this->ui->inputPressure->setMaximum(30);
    else
        this->ui->inputPressure->setMaximum(60);
    for(auto element: this->standards)
        this->ui->cbStandard->addItem(element.toObject()["standard"].toString());
    for(auto element: this->operators)
        this->ui->cbBoxOperator->addItem(QString("%1, %2").arg(element.toObject()["familyName"].toString()).arg(element.toObject()["name"].toString()));
}

void SetStation::SetSignals() {
    connect(this->ui->cbStandard,         SIGNAL(currentIndexChanged(int)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->cbMaterial,         SIGNAL(currentIndexChanged(int)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->cbSpecification,    SIGNAL(currentIndexChanged(int)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->cbTemp,             SIGNAL(currentTextChanged(QString)), this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputDiamNormal,    SIGNAL(textChanged(QString)),        this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputDiamReal,      SIGNAL(textChanged(QString)),        this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputHoopStress,    SIGNAL(textChanged(QString)),        this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputLenFree,       SIGNAL(textChanged(QString)),        this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputLenTotal,      SIGNAL(textChanged(QString)),        this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputPressure,      SIGNAL(textChanged(QString)),        this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputWallThickness, SIGNAL(textChanged(QString)),        this, SLOT(checkFieldsCompletetion()));
}

void SetStation::on_inputWallThickness_valueChanged(int wallthickness) {
    QString conditionalPeriod = "0 h ± 0 min";
    if(this->indexStandard > -1) {
        QJsonObject myStandard = this->standards[this->indexStandard].toObject();
        if(myStandard.count() > 0) {
            for(auto myConditionalPeriod: myStandard["conditionalperiods"].toArray()) {
                if(wallthickness >= myConditionalPeriod.toObject()["minwall"].toInt() && wallthickness < myConditionalPeriod.toObject()["maxwall"].toInt()) {
                    conditionalPeriod = myConditionalPeriod.toObject()["condPeriod"].toString();
                    break;
                }
            }
        }
    }
    this->ui->txtCondPeriod->setText(conditionalPeriod);
}

void SetStation::on_inputPressure_valueChanged(int Pressure) { if(Pressure > 0 && this->ui->inputDiamReal->value() > 0) { this->ui->inputHoopStress->setValue(this->hooppressure()); } }

uint SetStation::hooppressure() { return (this->ui->inputPressure->value() * (this->ui->inputDiamReal->value() - this->ui->inputWallThickness->value()) / (20 * this->ui->inputWallThickness->value())); }

void SetStation::inputPressureFixed(float pressure_, QSpinBox* input_) { if(input_->value() > pressure_) { input_->setValue(pressure_); } }

void SetStation::clearComboBox(QComboBox* myWidget, QString text, bool state) {
    if(myWidget->isEnabled()) { myWidget->setPlaceholderText(text); }
    if(myWidget->count() > 0) { myWidget->clear(); }
    myWidget->setEnabled(state);
}

void SetStation::checkSpecimen() {
    try {
        const QString standard = this->ui->cbStandard->currentText(), material = this->ui->cbMaterial->currentText(), specification = this->ui->cbSpecification->currentText();
        const uint diamNom = this->ui->inputDiamNormal->value(), diamReal = this->ui->inputDiamReal->value(), wallThick = this->ui->inputWallThickness->value(), lenFree = this->ui->inputLenFree->value(), lenTotal = this->ui->inputLenTotal->value();
        QSqlQuery myResult = this->myManager->selectTest(QString("SELECT countSpecimens('%1', '%2', '%3', %4, %5, %6, %7, %8) AS countSpecimen").arg(standard).arg(material).arg(specification).arg(QString::number(diamNom)).arg(QString::number(diamReal)).arg(QString::number(wallThick)).arg(QString::number(lenFree)).arg(QString::number(lenTotal)), "RemoteData");
        this->ui->lblSpecimen->setText(QString("Prueba Nro.: %1").arg(QString::number(myResult.value("countSpecimen").toUInt() + 1)));
    } catch(ManagerErrors::QuerySelectError& ex) {
        qDebug() << ex.what();
    }
}

void SetStation::checkFieldsCompletetion() {
    if((this->ui->inputDiamReal->value() != 0 && this->ui->inputDiamNormal->value() != 0 && this->ui->inputWallThickness->value() != 0) &&
        (this->ui->inputLenFree->value() != 0 && this->ui->inputLenTotal->value() != 0)) {
        if(!this->ui->cbTemp->currentText().isEmpty() && this->ui->inputPressure->value() != 0) {
            this->ui->btnSave->setEnabled(true);
            this->ui->btnSaveExport->setEnabled(false);
        }
    } else {
        this->ui->btnSave->setEnabled(false);
        this->ui->btnSaveExport->setEnabled(false);
    }
}

void SetStation::on_inputLenFree_valueChanged(int value_) { if(this->ui->inputLenTotal->value() <= value_) { this->ui->inputLenFree->setValue(this->ui->inputLenTotal->value() - 1); } }

void SetStation::on_cbStandard_currentIndexChanged(int index) {
    SetStation::clearComboBox(this->ui->cbMaterial,      "Material",       true);
    SetStation::clearComboBox(this->ui->cbSpecification, "Especificación", false);
    SetStation::clearComboBox(this->ui->cbBoxTestType,   "Tipo de Prueba", true);
    SetStation::clearComboBox(this->ui->cbBoxEndCap,     "Tapa",           true);
    SetStation::clearComboBox(this->ui->cbBoxEnviroment, "Entorno",        true);
    this->indexMaterial = this->indexStandard = this->indexSpecification = -1;

    if(index > -1) {
        this->indexStandard = index;
        QJsonObject myStandard = this->standards[index].toObject();
        for(auto myMaterial:  myStandard["materials"].toArray())
            this->ui->cbMaterial->addItem(myMaterial.toObject()["material"].toString());
        for(auto myEndCap:  myStandard["endcaps"].toArray())
            this->ui->cbBoxEndCap->addItem(myEndCap.toObject()["endcap"].toString());
        for(auto myEnviroment:  myStandard["enviroments"].toArray())
            this->ui->cbBoxEnviroment->addItem(myEnviroment.toObject()["insideFluid"].toString());
        for(auto myTestType:  myStandard["testtypes"].toArray())
            this->ui->cbBoxTestType->addItem(myTestType.toObject()["testtype"].toString());
    }
}

void SetStation::on_cbMaterial_currentIndexChanged(int index) {
    SetStation::clearComboBox(this->ui->cbSpecification, "Especificación", true);
    this->indexMaterial = this->indexSpecification = -1;

    if(index > -1) {
        this->indexMaterial = index;
        QJsonObject myMaterials = this->standards[this->ui->cbMaterial->currentIndex()].toObject();
        QJsonObject mySpecifications = myMaterials["materials"].toArray()[index].toObject();
        for(auto mySpecification:  mySpecifications["specifications"].toArray())
            this->ui->cbSpecification->addItem(mySpecification.toObject()["specification"].toString());
    }
}

void SetStation::on_cbSpecification_currentIndexChanged(int index) {
    this->indexSpecification = -1;
    if(index > -1) { this->indexSpecification = index; }
}

void SetStation::on_btnSave_clicked() {
    QMessageBox msgBox(QMessageBox::Warning, "Parametros: Estación "  + QString::number(this->ID_Station), "Desea guardar los cambios?");
    msgBox.setWindowIcon(QIcon(":/icon/logo"));
    msgBox.addButton(QMessageBox::Yes)->setText("Si");
    msgBox.addButton(QMessageBox::No)->setText("No");
    switch(msgBox.exec()) {
    case QMessageBox::Yes: {
        const QString standard = this->ui->cbStandard->currentText(), material = this->ui->cbMaterial->currentText(), specification = this->ui->cbSpecification->currentText(), testType = this->ui->cbBoxTestType->currentText(),
                      enviroment = this->ui->cbBoxEnviroment->currentText(), endCap = this->ui->cbBoxEndCap->currentText(), operatorName = this->ui->cbBoxOperator->currentText(), conditionalPeriod = this->ui->txtCondPeriod->text();
        const uint diamNom = this->ui->inputDiamNormal->value(), diamReal = this->ui->inputDiamReal->value(), wallThick = this->ui->inputWallThickness->value(), lenFree = this->ui->inputLenFree->value(), lenTotal = this->ui->inputLenTotal->value(),
                   pressure = this->ui->inputPressure->value(), temp = this->ui->cbTemp->currentText().toUInt();
        emit this->updateTest(this->ID_Test, standard, material, specification, lenTotal, lenFree, diamNom, diamReal, wallThick, testType, operatorName, endCap, enviroment, conditionalPeriod, pressure, temp);
        this->close();
        break;
    }
    default: break;
    }
}

void SetStation::on_btnCancel_clicked() {
    QMessageBox msgBox(QMessageBox::Warning, "Parametros: Estación "  + QString::number(this->ID_Station), "Desea cancelar los cambios?");
    msgBox.setWindowIcon(QIcon(":/icon/logo"));
    msgBox.addButton(QMessageBox::Yes)->setText("Si");
    msgBox.addButton(QMessageBox::No)->setText("No");
    switch(msgBox.exec()) {
        case QMessageBox::Yes: {
            this->close();
            break;
        }
        default: break;
    }
}

void SetStation::on_btnSaveExport_clicked() {
    QMessageBox msgBox(QMessageBox::Warning, "Parametros: Estación "  + QString::number(this->ID_Station), "Desea guardar y exportar los cambios?");
    msgBox.setWindowIcon(QIcon(":/icon/logo"));
    msgBox.addButton(QMessageBox::Yes)->setText("Si");
    msgBox.addButton(QMessageBox::No)->setText("No");
    switch(msgBox.exec()) {
    case QMessageBox::Yes: {
        const QString standard = this->ui->cbStandard->currentText(), material = this->ui->cbMaterial->currentText(), specification = this->ui->cbSpecification->currentText(), testType = this->ui->cbBoxTestType->currentText(),
            enviroment = this->ui->cbBoxEnviroment->currentText(), endCap = this->ui->cbBoxEndCap->currentText(), operatorName = this->ui->cbBoxOperator->currentText(), conditionalPeriod = this->ui->txtCondPeriod->text();
        const uint diamNom = this->ui->inputDiamNormal->value(), diamReal = this->ui->inputDiamReal->value(), wallThick = this->ui->inputWallThickness->value(), lenFree = this->ui->inputLenFree->value(), lenTotal = this->ui->inputLenTotal->value(),
            pressure = this->ui->inputPressure->value(), temp = this->ui->cbTemp->currentText().toUInt();
        emit this->updateTest(this->ID_Test, standard, material, specification, lenTotal, lenFree, diamNom, diamReal, wallThick, testType, operatorName, endCap, enviroment, conditionalPeriod, pressure, temp);
        emit this->exportTest(this->ID_Test);
        this->close();
        break;
    }
    default: break;
    }
}

void SetStation::on_btnEscExport_clicked() {
    QMessageBox msgBox(QMessageBox::Warning, "Parametros: Estación "  + QString::number(this->ID_Station), "Desea salir y exportar sin cambios?");
    msgBox.setWindowIcon(QIcon(":/icon/logo"));
    msgBox.addButton(QMessageBox::Yes)->setText("Si");
    msgBox.addButton(QMessageBox::No)->setText("No");
    switch(msgBox.exec()) {
    case QMessageBox::Yes: {
        emit this->exportTest(this->ID_Test);
        this->close();
        break;
    }
    default: break;
    }
}
