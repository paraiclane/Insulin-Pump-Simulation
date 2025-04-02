#include "QHomeWindow.h"
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

QHomeWindow::QHomeWindow(QWidget *parent) : QWidget(parent) {
    bolusButton = new QPushButton("Bolus", this);
    optionsButton = new QPushButton("Options", this);
    time = new QTextEdit(this);
    errorMessage = new QErrorMessage(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(bolusButton);
    layout->addWidget(optionsButton);
    layout->addWidget(time);
    layout->addWidget(errorMessage);
    
    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle("Bolus");

    QtCharts::QValueAxis *axisX = new QtCharts::QValueAxis();
    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
    axisY->setTitleText("mmol/L");

    axisX->setRange(0, 100);
    axisY->setRange(0, 40);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignRight);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    setCentralWidget(chartView);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateGraph);
    timer->start(1000);

    // Connect buttons to slots (navigation)
    connect(bolusButton, &QPushButton::clicked, this, &QHomeWindow::navBolus);
    connect(optionsButton, &QPushButton::clicked, this, &QHomeWindow::navOptions);
}

void QHomeWindow::navBolus() {
    emit navBolusRequested();
}

void QHomeWindow::navOptions() {
    emit navOptionsRequested();
}



void MainWindow::updateGraph() {
    int newValue = qrand() % 100;
    series->append(timeStep, newValue);
    timeStep++;

    if (timeStep > 100) {
        series->remove(0);
    }
}
