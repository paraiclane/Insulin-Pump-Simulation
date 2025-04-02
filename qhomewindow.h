#ifndef QHOMEWINDOW_H
#define QHOMEWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include "QErrorMessage.h"

class QHomeWindow : public QWidget {
    Q_OBJECT
public:
    explicit QHomeWindow(QWidget *parent = nullptr);

signals:
    void navBolusRequested();
    void navOptionsRequested();

private slots:
    void navBolus();
    void navOptions();
    void updateGraph();

private:
    QPushButton *bolusButton;
    QPushButton *optionsButton;
    QtCharts::QScatterSeries *series
    QTextEdit *time;
    QTimer *timer; 
    QErrorMessage *errorMessage;
    int timeStep;
};

#endif // QHOMEWINDOW_H
