#include "qpersonalprofiles.h"
#include "ui_personalprofiles.h"

QPersonalProfiles::QPersonalProfiles(Pump* pump, QWidget *parent) : QWidget(parent), ui(new Ui::PersonalProfiles), pump(pump) {
   ui->setupUi(this);

   // Return to Home Page
   connect(ui->backButton, &QPushButton::clicked, this, &QPersonalProfiles::onHomeButtonClicked);

   // Create a profile
   connect(ui->createButton, &QPushButton::clicked, this, &QPersonalProfiles::createProfile);

   // Read a profile
   connect(ui->enterButton, &QPushButton::clicked, this, &QPersonalProfiles::readProfile);

   // Update a profile
   connect(ui->updateButton, &QPushButton::clicked, this, &QPersonalProfiles::updateProfile);

   // Delete a profile
   connect(ui->deleteButton, &QPushButton::clicked, this, &QPersonalProfiles::deleteProfile);

}

QPersonalProfiles::~QPersonalProfiles() {
   delete ui;
}

void QPersonalProfiles::onHomeButtonClicked() {
   emit navHomeRequested();
}


void QPersonalProfiles::createProfile(){
   QString Qname = ui->nameEdit->toPlainText();
   QString Qbasal = ui->basalEdit->toPlainText();
   QString Qcorrection = ui->correctionEdit->toPlainText();
   QString Qcarbs = ui->carbsEdit->toPlainText();
   QString Qtarget = ui->targetEdit->toPlainText();

   string nameStr = Qname.toStdString();
   float basalFlt = Qbasal.toFloat();
   float correctionFlt = Qcorrection.toFloat();
   float carbsFlt = Qcarbs.toFloat();
   float targetFlt = Qtarget.toFloat();

   if(targetFlt == 0 || carbsFlt == 0 || correctionFlt == 0 || basalFlt == 0 || pump->getProfileManager()->searchName(nameStr) != -1 || nameStr.length() == 0){
       cout << "No profile created. Invalid inputs" << endl;
   } else {
       std::string errorMsg;
       if (pump->getProfileManager()->createProfile(nameStr, basalFlt, correctionFlt, carbsFlt, targetFlt, errorMsg)) {
              listProfiles();
          } else {
              cout << "Failed to create profile: " << errorMsg << endl;
          }

   }
}

void QPersonalProfiles::readProfile(){
   QString Qselect = ui->selectEdit->toPlainText();

   string selectStr = Qselect.toStdString();

   Profile* p = pump->getProfileManager()->readProfile(selectStr);

   if (p) {
       ui->nameEdit->setText(QString::fromStdString(p->getMode()));
       ui->basalEdit->setText(QString::number(p->getBasalRate()));
       ui->correctionEdit->setText(QString::number(p->getCorrectionFactor()));
       ui->carbsEdit->setText(QString::number(p->getCarbohydratesRatio()));
       ui->targetEdit->setText(QString::number(p->getTargetGlucoseLevels()));

       pump->setCurrentProfile(p);

   } else {
       cout << "no profiles with name " << selectStr << endl;
   }
}

void QPersonalProfiles::updateProfile(){
   QString Qselect = ui->selectEdit->toPlainText();
   string selectStr = Qselect.toStdString();

   Profile* p = pump->getProfileManager()->readProfile(selectStr);

   if (p) {
       QString Qname = ui->nameEdit->toPlainText();
       QString Qbasal = ui->basalEdit->toPlainText();
       QString Qcorrection = ui->correctionEdit->toPlainText();
       QString Qcarbs = ui->carbsEdit->toPlainText();
       QString Qtarget = ui->targetEdit->toPlainText();

       string nameStr = Qname.toStdString();
       float basalFlt = Qbasal.toFloat();
       float correctionFlt = Qcorrection.toFloat();
       float carbsFlt = Qcarbs.toFloat();
       float targetFlt = Qtarget.toFloat();

       if(targetFlt == 0 || carbsFlt == 0 || correctionFlt == 0 || basalFlt == 0){
           cout << "No profile updated. Invalid inputs" << endl;
       } else {
              std::string errorMsg; // Create error message variable
              if (pump->getProfileManager()->updateProfile(nameStr, basalFlt, correctionFlt, carbsFlt, targetFlt, errorMsg)) {
                  listProfiles();
                  cout << "Profile " << selectStr << " updated" << endl;
              } else {
                  cout << "Failed to update profile: " << errorMsg << endl;
              }
          }
      }else {
      cout << "Profile not found" << endl;
   }
}

void QPersonalProfiles::deleteProfile(){

   QString Qselect = ui->selectEdit->toPlainText();
   string selectStr = Qselect.toStdString();

   std::string errorMsg; // Create error message variable
      if (pump->getProfileManager()->deleteProfile(selectStr, errorMsg)) {
          listProfiles();
      } else {
          cout << "Failed to delete profile: " << errorMsg << endl;
      }
}

void QPersonalProfiles::listProfiles() {
   QString profileListText;

   for (Profile* p : pump->getProfileManager()->getProfileList()) {
       profileListText += QString::fromStdString(p->getMode()) + ", ";
   }

   ui->profilesLabel->setText(profileListText);
}


