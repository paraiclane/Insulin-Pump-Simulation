#ifndef QBOLUSWINDOW_H
#define QBOLUSWINDOW_H

#include <QWidget>
#include "bolus.h"
#include "pump.h"

namespace Ui {
class BolusWindow;
}

class QBolusWindow : public QWidget {
   Q_OBJECT

public:
   explicit QBolusWindow(Pump* pump, QWidget *parent = nullptr);
   ~QBolusWindow();

   void setPatientProfile(Profile* profile);
   Bolus* getCurrentBolus() const;

signals:
   void navHomeRequested();

private slots:
   void onHomeButtonClicked();
   void onCalculateBolusClicked();

private:
   Ui::BolusWindow *ui;
   Pump* pump;
   Bolus* currentBolus;
   Profile* patientProfile;
};

#endif // QBOLUSWINDOW_H



