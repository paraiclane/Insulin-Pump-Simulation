#include "qboluswindow.h"
#include "ui_boluswindow.h"
#include <QMessageBox>

QBolusWindow::QBolusWindow(Pump* pump, QWidget *parent)
  : QWidget(parent), ui(new Ui::BolusWindow),
    pump(pump), currentBolus(nullptr), patientProfile(nullptr)
{
  ui->setupUi(this);

  connect(ui->homeButton, &QPushButton::clicked, this, &QBolusWindow::onHomeButtonClicked);
  connect(ui->calculateButton, &QPushButton::clicked, this, &QBolusWindow::onCalculateBolusClicked);
}

QBolusWindow::~QBolusWindow() {
  delete ui;
  delete currentBolus;
  // Do not delete patientProfile or pump — they are managed externally
}

void QBolusWindow::setPatientProfile(Profile* profile) {
    this->patientProfile = profile;

    if (patientProfile) {
        float targetBG = patientProfile->getTargetGlucoseLevels();
        float carbRatio = patientProfile->getCarbohydratesRatio();
        float correctionFactor = patientProfile->getCorrectionFactor();

        // Optional: Show these in read-only labels for user reference (not required)
        ui->correctionFactor->setPlainText(QString::number(correctionFactor));
        ui->targetBGLabel->setText(QString::number(targetBG));
        ui->carbRatioLabel->setText(QString::number(carbRatio));

        // Leave user input fields blank so they can enter real-time values
        ui->glucoseLevel->setPlainText(""); // <-- clear this
        ui->carbIntake->setPlainText("");   // <-- clear this
    } else {
        QMessageBox::warning(this, "No Profile", "No active profile loaded into BolusWindow.");
    }
}



void QBolusWindow::onHomeButtonClicked() {
  emit navHomeRequested();
}

void QBolusWindow::onCalculateBolusClicked() {
   bool glucoseOK, carbOK, cfOK;
   float glucose = ui->glucoseLevel->toPlainText().toFloat(&glucoseOK);
   float carbs = ui->carbIntake->toPlainText().toFloat(&carbOK);
   float correctionFactor = ui->correctionFactor->toPlainText().toFloat(&cfOK);

   if (!glucoseOK || !carbOK || !cfOK || glucose <= 0 || carbs < 0 || correctionFactor <= 0) {
       QMessageBox::warning(this, "Input Error", "Please enter valid, positive values for glucose, carbs, and correction factor.");
       return;
   }

   if (currentBolus) delete currentBolus;

   Bolus* b = new Bolus("B001", glucose, carbs, patientProfile);
   currentBolus = pump->setBolus(b);
   pump->getHome()->setGlucoseLevel(glucose);

   currentBolus->setCorrectionFactorOverride(correctionFactor);
   currentBolus->setIOB(5.0f);
   currentBolus->calculateFinalBolus();

   float dose = currentBolus->getAppropriateDose();

   if (dose == 0.0f) {
       QMessageBox::critical(this, "Calculation Error", "Unable to calculate dose. Check input or profile values.");
       return;
   }

   if (ui->quickBolus->isChecked()) {
       float quickDose = 0.6f * dose;
       currentBolus->quickBolus();
       ui->resultLabel->setText(QString("Quick Bolus: %1 units delivered immediately.").arg(quickDose, 0, 'f', 2));
   }
   else if (ui->extendedBolus->isChecked()) {
       float extendedDose = 0.4f * dose;

       int hours = ui->extendedTimeInput->value();

       currentBolus->setExtendedDuration(hours);
       currentBolus->extendedBolus(hours);

       ui->resultLabel->setText(
           QString("Extended Bolus: %1 units over %2 hour(s) (%3 units/hour).")
           .arg(extendedDose, 0, 'f', 2)
           .arg(hours)
           .arg(extendedDose / hours, 0, 'f', 2)
       );
   }
   else if (ui->standardBolus->isChecked()) {
       ui->resultLabel->setText(QString("Standard Bolus: %1 units delivered.").arg(dose, 0, 'f', 2));
   }
   else {
       // Default fallback
       ui->resultLabel->setText(QString("Standard Bolus: %1 units delivered.").arg(dose, 0, 'f', 2));
   }
}

Bolus* QBolusWindow::getCurrentBolus() const {
    return currentBolus;
}




