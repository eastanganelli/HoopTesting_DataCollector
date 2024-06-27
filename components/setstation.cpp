#include "setstation.h"
#include "components/ui_setstation.h"
#include "../defines.h"

SetStation::SetStation(QWidget *parent) : QDialog(parent) , ui(new Ui::SetStation) {
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::Desktop);
    this->normsDB = QSharedPointer<Schemas::Static>(new Schemas::Static());
    if(this->normsDB->open()) { this->preLoadData(); }
    this->setConnectionSignals();
    this->ui->lblSpecimen->setText("Prueba Nro.: 0");
}

SetStation::~SetStation() {
    this->normsDB->close();
    delete ui;
}

void SetStation::sharePointer(QSharedPointer<Station> selectedStation) {
    this->selectedStation = selectedStation;
    this->ui->gpParameters->setTitle("Estación: " + QString::number(selectedStation->getID()));
    if(selectedStation->getID() < 4) { this->ui->inputPressure->setMaximum(30); }
    this->isPopulated();
}

void SetStation::on_inputWallThickness_valueChanged(int wallthickness) {
    QString conditionalPeriod = "0 h ± 0 min";
    if(this->listCondPeriods.count() > 0) {
        for(auto myCP : this->listCondPeriods) {
            if((uint)(wallthickness) >= myCP->getMinWall() && (uint)(wallthickness) < myCP->getMaxWall()) {
                conditionalPeriod = myCP->getConditionalPeriod();
                break;
            }
        }
    }
    this->ui->txtCondPeriod->setText(conditionalPeriod);
}

void SetStation::on_inputPressure_valueChanged(int Pressure) { if(Pressure > 0 && this->ui->inputDiamReal->value() > 0) { this->ui->inputHoopStress->setValue(this->hooppressure()); } }

unsigned int SetStation::hooppressure() { return (this->ui->inputPressure->value() * (this->ui->inputDiamReal->value() - this->ui->inputWallThickness->value()) / (20 * this->ui->inputWallThickness->value())); }

void SetStation::inputPressureFixed(float pressure_, QSpinBox* input_) { if(input_->value() > pressure_) { input_->setValue(pressure_); } }

void SetStation::clearComboBox(QComboBox* myWidget, QString text, bool state) {
    if(myWidget->isEnabled()) { myWidget->setPlaceholderText(text); }
    if(myWidget->count() > 0) { myWidget->clear(); }
    myWidget->setEnabled(state);
}

void SetStation::preLoadData() {
    this->listStandards = NodeStandard::get();
    for(auto myStandard : this->listStandards) { this->ui->cbStandard->addItem(myStandard->getStandard()); }

    this->listOperators = NodeOperator::get();
    for(auto myOperator : this->listOperators) { this->ui->cbBoxOperator->addItem(myOperator->getFullName()); }
}

void SetStation::setConnectionSignals() {
    connect(this->ui->cbStandard,      SIGNAL(currentIndexChanged(int)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->cbMaterial,      SIGNAL(currentIndexChanged(int)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->cbSpecification, SIGNAL(currentIndexChanged(int)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->cbBoxTestTime,   SIGNAL(currentTextChanged(QString)), this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->cbTemp,          SIGNAL(currentTextChanged(QString)), this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->radSeconds,      SIGNAL(clicked(bool)),               this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->radHours,        SIGNAL(clicked(bool)),               this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputDiamNormal,     SIGNAL(textChanged(QString)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputDiamReal,       SIGNAL(textChanged(QString)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputHoopStress,     SIGNAL(textChanged(QString)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputLenFree,        SIGNAL(textChanged(QString)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputLenTotal,       SIGNAL(textChanged(QString)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputPressure,       SIGNAL(textChanged(QString)),    this, SLOT(checkFieldsCompletetion()));
    connect(this->ui->inputWallThickness,  SIGNAL(textChanged(QString)),    this, SLOT(checkFieldsCompletetion()));
}

void SetStation::checkSpecimen() {
    this->idSample = Data::NodeSample::exists(this->ui->cbStandard->currentText(), this->ui->cbMaterial->currentText(), this->ui->cbSpecification->currentText(), this->ui->inputDiamNormal->value(), this->ui->inputDiamReal->value(), this->ui->inputWallThickness->value(),  this->ui->inputLenFree->value(), this->ui->inputLenTotal->value(), this->ui->inputPressure->value(), this->ui->cbTemp->currentText().toUInt());
    this->ui->lblSpecimen->setText("Prueba Nro.: " + QString::number(Data::NodeSpecimen::count(this->idSample) + 1));
}

void SetStation::isPopulated() {
    if(this->selectedStation->getStatus() == StationStatus::WAITING) {
        this->ui->cbStandard->setCurrentText(this->selectedStation->getSample()->getStandard());
        this->ui->cbMaterial->setEnabled(true);
        this->ui->cbMaterial->setCurrentText(this->selectedStation->getSample()->getMaterial());
        this->ui->cbSpecification->setEnabled(true);
        this->ui->cbSpecification->setCurrentText(this->selectedStation->getSample()->getSpecification());
        this->ui->cbBoxTestType->setEnabled(true);
        this->ui->cbBoxTestType->setCurrentText(this->selectedStation->getSpecimen()->getTestName());
        this->ui->cbBoxEnviroment->setEnabled(true);
        this->ui->cbBoxEnviroment->setCurrentText(this->selectedStation->getSpecimen()->getEnviroment());
        this->ui->cbBoxEndCap->setEnabled(true);
        this->ui->cbBoxEndCap->setCurrentText(this->selectedStation->getSpecimen()->getEndCap());
        this->ui->cbBoxOperator->setEnabled(true);
        this->ui->cbBoxOperator->setCurrentText(this->selectedStation->getSpecimen()->getOperatorName());
        this->ui->inputDiamNormal->setValue(this->selectedStation->getSample()->getDiamNom());
        this->ui->inputDiamReal->setValue(this->selectedStation->getSample()->getDiamReal());
        this->ui->inputWallThickness->setValue(this->selectedStation->getSample()->getWallThick());
        this->ui->inputLenFree->setValue(this->selectedStation->getSample()->getLenFree());
        this->ui->inputLenTotal->setValue(this->selectedStation->getSample()->getLenTotal());
        this->ui->inputPressure->setValue(this->selectedStation->getSample()->getTargetPressure());
        this->ui->cbTemp->setCurrentText(QString::number(this->selectedStation->getSample()->getTargetTemperature()));
    }
}

void SetStation::configureStation() {
    const QString standard          = this->ui->cbStandard->currentText(),
                  material          = this->ui->cbMaterial->currentText(),
                  specification     = this->ui->cbSpecification->currentText(),
                  conditionalPeriod = this->ui->txtCondPeriod->text(),
                  operatorName      = this->ui->cbBoxOperator->currentText(),
                  enviroment        = this->ui->cbBoxEnviroment->currentText(),
                  typeTest          = this->ui->cbBoxTestType->currentText(),
                  endCap            = this->ui->cbBoxEndCap->currentText();

    const uint time_ = this->ui->radHours->isChecked() ? this->ui->cbBoxTestTime->currentText().toUInt() * 3600 : this->ui->cbBoxTestTime->currentText().toUInt(),
               lengthFree        = this->ui->inputLenFree->value(),
               lengthTotal       = this->ui->inputLenTotal->value(),
               diameterReal      = this->ui->inputDiamReal->value(),
               diameterNormal    = this->ui->inputDiamNormal->value(),
               wallthickness     = this->ui->inputWallThickness->value(),
               targetTemperature = this->ui->cbTemp->currentText().toUInt(),
               targetPressure    = this->ui->inputPressure->value();

    QSharedPointer<Data::NodeSample>   SampleData   = QSharedPointer<Data::NodeSample>(new Data::NodeSample(this->idSample, standard, material, specification, diameterNormal, diameterReal, wallthickness, lengthFree, lengthTotal, targetPressure, targetTemperature, conditionalPeriod));
    QSharedPointer<Data::NodeSpecimen> SpecimenData = QSharedPointer<Data::NodeSpecimen>(new Data::NodeSpecimen(0, this->idSample, operatorName, enviroment, typeTest, endCap));

    Station::set(this->selectedStation, SampleData, SpecimenData, time_);
}

void SetStation::checkFieldsCompletetion() {
    if((this->ui->inputDiamReal->value() != 0 && this->ui->inputDiamNormal->value() != 0 && this->ui->inputWallThickness->value() != 0) &&
        (this->ui->inputLenFree->value() != 0 && this->ui->inputLenTotal->value() != 0) && (!this->ui->cbTemp->currentText().isEmpty() && this->ui->inputPressure->value() != 0) &&
            (!this->ui->cbBoxTestTime->currentText().isEmpty() && (this->ui->radSeconds->isChecked() || this->ui->radHours->isChecked()))) {
        this->ui->btnSave->setEnabled(true);
        this->checkSpecimen();
    } else { this->ui->btnSave->setEnabled(false); }
}

void SetStation::on_inputLenFree_valueChanged(int value_) { if(this->ui->inputLenTotal->value() <= value_) { this->ui->inputLenFree->setValue(this->ui->inputLenTotal->value() - 1); } }

void SetStation::on_cbStandard_currentIndexChanged(int index) {
    SetStation::clearComboBox(this->ui->cbMaterial, "Material", true);
    SetStation::clearComboBox(this->ui->cbSpecification, "Especificación", false);
    SetStation::clearComboBox(this->ui->cbTemp, " ", true);
    SetStation::clearComboBox(this->ui->cbBoxTestTime, " ", true);
    SetStation::clearComboBox(this->ui->cbBoxEndCap, " ", true);
    SetStation::clearComboBox(this->ui->cbBoxTestType, " ", true);
    SetStation::clearComboBox(this->ui->cbBoxEnviroment, " ", true);

    if(index > -1) {
        this->selectedStandard = this->listStandards[index];
        const uint idStandard = this->selectedStandard->getID();

        this->listCondPeriods = NodeConditionalPeriod::get(idStandard);
        this->listEnviroments = NodeEnviroment::get(       idStandard);
        this->listEndCap      = NodeEndCap::get(           idStandard);
        this->listTestTypes   = NodeTestType::get(         idStandard);
        this->listMaterials   = NodeMaterial::get(         idStandard);

        QStringList testEndCapsList;
        for(auto myEndCap : this->listEndCap) { testEndCapsList << myEndCap->getEndCap(); }
        this->ui->cbBoxEndCap->addItems(testEndCapsList);
        this->ui->cbBoxEndCap->setEnabled(true);

        QStringList testTypesList;
        for(auto myTestType : this->listTestTypes) { testTypesList << myTestType->getTestType(); }
        this->ui->cbBoxTestType->addItems(testTypesList);
        this->ui->cbBoxTestType->setEnabled(true);

        QStringList enviromentsList;
        for(auto myEnviroment : this->listEnviroments) { enviromentsList << myEnviroment->getEnviroment(); }
        this->ui->cbBoxEnviroment->addItems(enviromentsList);
        this->ui->cbBoxEnviroment->setEnabled(true);

        QStringList materialList;
        for(auto myMaterial : this->listMaterials) { materialList << myMaterial->getMaterial(); }
        this->ui->cbMaterial->addItems(materialList);
    }
}

void SetStation::on_cbMaterial_currentIndexChanged(int index) {
    SetStation::clearComboBox(this->ui->cbSpecification, "Especificación", true);
    SetStation::clearComboBox(this->ui->cbTemp, " ", true);
    SetStation::clearComboBox(this->ui->cbBoxTestTime, " ", true);

    if(index > -1) {
        this->selectedMaterial = this->listMaterials[index];

        this->listSpecifications = NodeSpecification::get(this->selectedMaterial->getID());
        if(this->listSpecifications.length() > 0) {
            QStringList specificationsList;
            for(auto mySpec : this->listSpecifications) { specificationsList << mySpec->getSpecification(); }
            this->ui->cbSpecification->addItems(specificationsList);
        }
    }
}

void SetStation::on_cbSpecification_currentIndexChanged(int index) {
    SetStation::clearComboBox(this->ui->cbTemp, " ", true);
    SetStation::clearComboBox(this->ui->cbBoxTestTime, " ", true);

    if(index > -1) {
        this->selectedSpecification = this->listSpecifications[index];

        this->listSettings = NodeSetting::get(this->selectedSpecification->getID());
        if(this->listSettings.length() > 0) {
            QStringList tempList, timeList;
            for(auto mySetting : this->listSettings) {
                tempList << QString::number(mySetting->getTemperature());
                timeList << QString::number(mySetting->getTime());
            }
            this->ui->cbTemp->addItems(tempList);
            this->ui->cbBoxTestTime->addItems(timeList);
        }
    }
}

void SetStation::on_btnSave_clicked() {
    QMessageBox msgBox(QMessageBox::Warning, "Configuración: Estación "  + QString::number(this->selectedStation->getID()), "Desea guardar cambios?");
    msgBox.setWindowIcon(QIcon(":/icon/logo"));
    msgBox.addButton(QMessageBox::Yes)->setText("Si");
    msgBox.addButton(QMessageBox::No)->setText("No");

    switch(msgBox.exec()) {
        case QMessageBox::Yes: {
            this->configureStation();
            this->close();
            break;
        }
        default: break;
    }
}

void SetStation::on_btnCancel_clicked() {
    QMessageBox msgBox(QMessageBox::Warning, "Configuración: Estación "  + QString::number(this->selectedStation->getID()), "Desea cancelar cambios?");
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

void SetStation::on_cbBoxOperator_currentIndexChanged(int index) { if(index > -1) { this->selectedOperator = this->listOperators[index]; } }

void SetStation::on_cbBoxTestType_currentIndexChanged(int index) { if(index > -1) { this->selectedTestType = this->listTestTypes[index]; } }

void SetStation::on_cbBoxEnviroment_currentIndexChanged(int index) {
    if(index > -1) {
        this->selectedEnviroment = this->listEnviroments[index];
    }
}

void SetStation::on_cbBoxTestTime_currentIndexChanged(int index) {
    if(index > -1) {
        this->selectedSetting = this->listSettings[index];
        if(this->selectedSetting->getTimeType() == "h")      { this->ui->radHours->setChecked(true); }
        else if(this->selectedSetting->getTimeType() == "s") { this->ui->radSeconds->setChecked(true); }
    }
}

