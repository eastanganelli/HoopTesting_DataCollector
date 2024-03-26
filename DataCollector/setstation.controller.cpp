#include "setstation.controller.h"
#include "ui_setstation.controller.h"

SetStation::SetStation(QWidget *parent) : QDialog(parent) , ui(new Ui::SetStation) {
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::Desktop);
    this->normsDB = QSharedPointer<Schemas::Static>(new Schemas::Static());
    if(this->normsDB->open()) {
        this->loadStandardCombobox();
    }
    this->setConnectionSignals();
    this->ui->lblSpecimen->setText("Nro Specimen: 0");
    {
#if MODE_ == Emulation
        this->ui->cbTemp->setCurrentText("20");
        this->ui->cbBoxTestTime->setCurrentText("90");
        this->ui->cbStandard->setCurrentIndex(0);
        this->ui->cbMaterial->setCurrentIndex(0);
        this->ui->cbSpecification->setCurrentIndex(0);
        this->ui->inputDiamNormal->setValue(50);
        this->ui->inputDiamReal->setValue(50);
        this->ui->inputWallThickness->setValue(5);
        this->ui->inputLenTotal->setValue(560);
        this->ui->inputLenFree->setValue(340);
        this->ui->inputPressure->setValue(9);
#endif
    }
}

SetStation::~SetStation() {
    this->normsDB->close();
    delete ui;
}

void SetStation::sharePointer(StationResponse* response, QSharedPointer<Schemas::Data> dataDB, QSharedPointer<SerialPortReader> myPort, QSharedPointer<Station> selectedStation) {
    this->dataDB          = dataDB;
    this->portWriting     = myPort;
    this->selectedStation = selectedStation;
    this->ui->gpParameters->setTitle("Estación: " + QString::number(selectedStation->getID()));
    this->response        = response;
    if(selectedStation->getID() < 4) { this->ui->inputPressure->setMaximum(30); }
}

void SetStation::on_inputWallThickness_valueChanged(int wallthickness) {
    QString conditionalPeriod = "0 h ± 0 min";
    if(this->listCondPeriods.count() > 0) {
        for(auto myCP : this->listCondPeriods) {
            if((uint)(wallthickness) > myCP->getMinWall() && (uint)(wallthickness) <= myCP->getMaxWall()) {
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

void SetStation::loadStandardCombobox() {
    this->listStandards = NodeStandard::get(*this->normsDB.get());
    for(auto myStandard : this->listStandards) { this->ui->cbStandard->addItem(myStandard->getStandard()); }
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
    this->idSample = Data::NodeSample::exists(*this->dataDB.get(), this->ui->cbStandard->currentText(), this->ui->cbMaterial->currentText(), this->ui->cbSpecification->currentText(), this->ui->inputDiamNormal->value(), this->ui->inputDiamReal->value(), this->ui->inputWallThickness->value(),  this->ui->inputLenFree->value(), this->ui->inputLenTotal->value(), this->ui->inputPressure->value(), this->ui->cbTemp->currentText().toUInt());
    this->ui->lblSpecimen->setText("Nro Specimen: " + QString::number(Data::NodeSpecimen::count(*this->dataDB.get(), this->idSample) + 1));
}

void SetStation::configureStation() {
    const QString standard = this->ui->cbStandard->currentText(),
                  material = this->ui->cbMaterial->currentText(),
                  specification     = this->ui->cbSpecification->currentText(),
                  conditionalPeriod = this->ui->txtCondPeriod->text();
    const uint time_ = this->ui->radHours->isChecked() ? this->ui->cbBoxTestTime->currentText().toUInt() * 3600 : this->ui->cbBoxTestTime->currentText().toUInt(),
               lengthFree   = this->ui->inputLenFree->value(),
               lengthTotal  = this->ui->inputLenTotal->value(),
               diameterReal = this->ui->inputDiamReal->value(),
               diameterNormal = this->ui->inputDiamNormal->value(),
               wallthickness  = this->ui->inputWallThickness->value(),
               targetTemperature = this->ui->cbTemp->currentText().toUInt(),
               targetPressure    = this->ui->inputPressure->value();
    QSharedPointer<Data::NodeSample>   SampleData   = Data::NodeSample::add(*this->dataDB.get(), standard, material, specification,diameterNormal, diameterReal, wallthickness, lengthFree, lengthTotal, targetPressure, targetTemperature, conditionalPeriod);
    const uint idSample = SampleData->getID();
    QSharedPointer<Data::NodeSpecimen> SpecimenData = Data::NodeSpecimen::add(*this->dataDB.get(), idSample, 0, "water");
    Station::configure(this->selectedStation, SampleData, SpecimenData, time_);
    #if MODE_ == Emulation
        uint condPeriod = this->ui->txtCondPeriod->text().split(" ")[0].toUInt() * 3600;
        QString myMessage = QString::number(this->selectedStation->getID()) + "," + QString::number(targetPressure) + "," + QString::number(targetTemperature) + "," + QString::number(condPeriod) + "\n";
    #else
        QString myMessage = QString::number(this->selectedStation->getID()) + "," + QString::number(targetPressure) + "," + QString::number(targetTemperature) + "\n";
    #endif
    myData.pushMessageSendPort(myMessage.toUtf8());
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

    if(index > -1) {
        this->selectedStandard = this->listStandards[index];
        const uint idStandard = this->selectedStandard->getID();

        this->listCondPeriods = NodeConditionalPeriod::get(*this->normsDB.get(), idStandard);
        this->listMaterials = NodeMaterial::get(*this->normsDB.get(), idStandard);

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

        this->listSpecifications = NodeSpecification::get(*this->normsDB.get(), this->selectedMaterial->getID());
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

        this->listSettings = NodeSetting::get(*this->normsDB.get(), this->selectedSpecification->getID());
        if(this->listSettings.length() > 0) {
            QStringList tempList, timeList;
            for(auto mySetting : this->listSettings) {
                tempList << QString::number(mySetting->getTemperature());
                timeList << QString::number(mySetting->getTimes());
            }
            this->ui->cbTemp->addItems(tempList);
            this->ui->cbBoxTestTime->addItems(timeList);
        }
    }
}

void SetStation::on_btnSave_clicked() {
    int ret = QMessageBox::information(this, "Ajuste Estación "  + QString::number(this->selectedStation->getID()), "Está seguro que desea guardar?", QMessageBox::Yes | QMessageBox::No);
    switch(ret) {
        case QMessageBox::Yes: {
            this->configureStation();
            *this->response = StationResponse::Save;
            this->close();
            break;
        }
        default: break;
    }
}

void SetStation::on_btnCancel_clicked() {
    int ret = QMessageBox::information(this, "Ajuste Estación "  + QString::number(this->selectedStation->getID()), "Está seguro que desea cancelar?", QMessageBox::Yes | QMessageBox::No);
    switch(ret) {
        case QMessageBox::Yes: {
            *this->response = StationResponse::Cancel;
            this->close();
            break;
        }
        default: break;
    }
}
