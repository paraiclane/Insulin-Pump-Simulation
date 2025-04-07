#include "qhomewindow.h"
#include "pump.h"
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

QHomeWindow::QHomeWindow(Pump* pump, QWidget *parent) : QWidget(parent), pump(pump) {
    bolusButton = new QPushButton("Bolus", this);
    optionsButton = new QPushButton("Options", this);
    occlusionAlertButton = new QPushButton("Occlusion Alert", this);
    CGMAlertButton = new QPushButton("CGM Alert", this);
    startDeliveryButton = new QPushButton("Start Delivery",this);
    pauseDeliveryButton = new QPushButton("Pause Delivery", this);
    resumeDeliveryButton = new QPushButton("Resume Delivery",this);
    stopDeliveryButton = new QPushButton("Stop Delivery", this);
    batteryProgressBar = new QProgressBar(this);
    insulinProgressBar = new QProgressBar(this);

    series = new QtCharts::QScatterSeries();

    // Initialize progress bar
    batteryProgressBar->setRange(0, 100);
    batteryProgressBar->setValue(100);
    batteryProgressBar->setFormat("Battery: %p%");

    // Initialize insulin progress bar
    insulinProgressBar->setRange(0, 300);  // Assuming 300 is max from Home constructor
    insulinProgressBar->setValue(300);
    insulinProgressBar->setFormat("Insulin: %v units");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(bolusButton);
    layout->addWidget(optionsButton);
    layout->addWidget(occlusionAlertButton);
    layout->addWidget(CGMAlertButton);
    layout->addWidget(startDeliveryButton);
    layout->addWidget(pauseDeliveryButton);
    layout->addWidget(resumeDeliveryButton);
    layout->addWidget(stopDeliveryButton);
    layout->addWidget(batteryProgressBar);
    layout->addWidget(insulinProgressBar);

    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle("Bolus");

    QtCharts::QValueAxis *axisX = new QtCharts::QValueAxis();
    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
    axisY->setTitleText("mmol/L");

    axisX->setRange(0, 60);
    axisY->setRange(0, 22);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignRight);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(chartView);
    setLayout(layout);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &QHomeWindow::updateSim);
    connect(timer, &QTimer::timeout, this, &QHomeWindow::updateBatteryDisplay);
    connect(timer, &QTimer::timeout, this, &QHomeWindow::updateInsulinDisplay);
    timer->start(1000);

    // Connect buttons to slots (navigation)
    connect(bolusButton, &QPushButton::clicked, this, &QHomeWindow::navBolus);
    connect(optionsButton, &QPushButton::clicked, this, &QHomeWindow::navOptions);
    connect(occlusionAlertButton, &QPushButton::clicked, this, &QHomeWindow::showOcclusionAlert);
    connect(CGMAlertButton, &QPushButton::clicked, this, &QHomeWindow::showCGMAlert);
    connect(startDeliveryButton, &QPushButton::clicked,this, &QHomeWindow::startDelivery);
    connect(pauseDeliveryButton, &QPushButton::clicked,this, &QHomeWindow::pauseDelivery);
    connect(resumeDeliveryButton, &QPushButton::clicked,this, &QHomeWindow::resumeDelivery);
    connect(stopDeliveryButton, &QPushButton::clicked,this, &QHomeWindow::stopDelivery);

    updateBatteryDisplay();
    updateInsulinDisplay();
}

void QHomeWindow::navBolus() {
    emit navBolusRequested();
}

void QHomeWindow::startDelivery(){
    pump->startInsulinDelivery();
    pump->getBolus()->setActive();
}

void QHomeWindow::pauseDelivery(){
    //pump->pauseInsulinDelivery();
    pump->getBolus()->setPaused();

}

void QHomeWindow::resumeDelivery(){
    pump->resumeInsulinDelivery();
    pump->getBolus()->setActive();
}

void QHomeWindow::stopDelivery(){
    pump->stopInsulinDelivery();
    pump->getBolus()->setCanceled();
}


void QHomeWindow::navOptions() {
    emit navOptionsRequested();
}

void QHomeWindow::updateSim() {
    pump->getHome()->checkBatteryAlert();
    pump->getHome()->checkInsulinRemainingAlert();
    if(pump->getBolus() != nullptr)
     if(pump->getBolus()->isActive()){
         series->append(timeStep,pump->getHome()->getGlucoseLevel());
         pump->adjustGlucoseLevel();
         timeStep++;
      if (timeStep > 60) {
         series->remove(0);
      }
      QList<QPointF> points = series->points();
             series->clear();
             for (int i = 0; i < points.size(); ++i) {
                 series->append(points[i].x() - 1, points[i].y());
             }
     }


}

void QHomeWindow::showOcclusionAlert() {
    pump->occlusionAlert();

    // Create a new dialog
    QDialog *alertDialog = new QDialog(this);
    alertDialog->setWindowTitle("Occlusion Alert");

    // Create layout for dialog
    QVBoxLayout *dialogLayout = new QVBoxLayout(alertDialog);

    // Add warning text
    QLabel *alertText = new QLabel("Warning: Occlusion detected!\nPlease check the system.", alertDialog);
    alertText->setAlignment(Qt::AlignCenter);

    // Add OK button
    QPushButton *okButton = new QPushButton("OK", alertDialog);

    // Add widgets to layout
    dialogLayout->addWidget(alertText);
    dialogLayout->addWidget(okButton);

    // Connect OK button to close the dialog
    connect(okButton, &QPushButton::clicked, alertDialog, &QDialog::accept);

    // Show the dialog
    alertDialog->exec();

    // Clean up (dialog will be automatically deleted after closing due to parent)
}

void QHomeWindow::showCGMAlert() {
    pump->triggerCGMAlert();

    // Create a new dialog
    QDialog *alertDialog = new QDialog(this);
    alertDialog->setWindowTitle("CGM Alert");

    // Create layout for dialog
    QVBoxLayout *dialogLayout = new QVBoxLayout(alertDialog);

    // Add warning text
    QLabel *alertText = new QLabel("Warning: CGM disconnection detected!\nPlease check the system.", alertDialog);
    alertText->setAlignment(Qt::AlignCenter);

    // Add OK button
    QPushButton *okButton = new QPushButton("OK", alertDialog);

    // Add widgets to layout
    dialogLayout->addWidget(alertText);
    dialogLayout->addWidget(okButton);

    // Connect OK button to close the dialog
    connect(okButton, &QPushButton::clicked, alertDialog, &QDialog::accept);

    // Show the dialog
    alertDialog->exec();

    // Clean up (dialog will be automatically deleted after closing due to parent)
}

void QHomeWindow::handleBatteryLevelChanged(float level) {
    batteryProgressBar->setValue(static_cast<int>(level));
    QString styleSheet = (level > 20) ?
                             "QProgressBar::chunk { background-color: green; }" :
                             "QProgressBar::chunk { background-color: red; }";
    batteryProgressBar->setStyleSheet(styleSheet);
}

void QHomeWindow::updateBatteryDisplay() {
    if (pump) {
        float batteryLevel = pump->getBatteryLevel();
        handleBatteryLevelChanged(batteryLevel); // Reuse the same logic
    }
}

void QHomeWindow::handleInsulinLevelChanged(int remaining) {
    insulinProgressBar->setValue(remaining);
    QString styleSheet = (remaining > 50) ?
                             "QProgressBar::chunk { background-color: green; }" :
                             "QProgressBar::chunk { background-color: red; }";
    insulinProgressBar->setStyleSheet(styleSheet);
}

void QHomeWindow::updateInsulinDisplay() {
    if (pump) {
        int insulinRemaining = pump->getInsulinDoseRemaining();
        handleInsulinLevelChanged(insulinRemaining);
    }
}

