#ifndef QHOMEWINDOW_H
#define QHOMEWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QtCharts>
#include <QtCharts/QScatterSeries>
#include "qerrormessage.h"
#include "qpersonalprofiles.h"


class QHomeWindow : public QWidget {
    Q_OBJECT
public:
    explicit QHomeWindow(Pump* pump, QWidget *parent = nullptr);

signals:
    void navBolusRequested();
    void navOptionsRequested();

private slots:
    void navBolus();
    void navOptions();
    void updateSim();
    void showOcclusionAlert();
    void showCGMAlert();
    void startDelivery();
    void pauseDelivery();
    void resumeDelivery();
    void stopDelivery();
    void updateBatteryDisplay();
    void handleBatteryLevelChanged(float level);
    void updateInsulinDisplay();
    void handleInsulinLevelChanged(int remaining);

private:
    QPushButton *bolusButton;
    QPushButton *optionsButton;
    QScatterSeries *series;
    QPushButton *occlusionAlertButton;
    QPushButton  *CGMAlertButton;
    QTimer *timer;
    int timeStep = 0;
    QPushButton *startDeliveryButton;
    QPushButton *pauseDeliveryButton;
    QPushButton *resumeDeliveryButton;
    QPushButton *stopDeliveryButton;
    QProgressBar *batteryProgressBar;
    QProgressBar *insulinProgressBar;

    Pump* pump;
};

#endif // QHOMEWINDOW_H
