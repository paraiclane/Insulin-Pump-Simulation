#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), pump(nullptr), ui(new Ui::MainWindow) {
   ui->setupUi(this);

   ProfileManager* pm = new ProfileManager();
   Home* h = new Home();
   Log* l = new Log();

   pump = new Pump(pm, h, l);

   homeWindow = new QHomeWindow(pump, this);
   bolusWindow = new QBolusWindow(pump, this);
   logWindow = new QLogWindow(pump, this);
   optionsMenu = new QOptionsMenu(pump, this);
   personalProfiles = new QPersonalProfiles(pump, this);


   stackedWidget = new QStackedWidget(this);
   stackedWidget->addWidget(homeWindow);
   stackedWidget->addWidget(bolusWindow);
   stackedWidget->addWidget(logWindow);
   stackedWidget->addWidget(optionsMenu);
   stackedWidget->addWidget(personalProfiles);


   setCentralWidget(stackedWidget);
   stackedWidget->setCurrentWidget(homeWindow);


   connect(homeWindow, &QHomeWindow::navBolusRequested, this, &MainWindow::showBolusWindow);
   connect(homeWindow, &QHomeWindow::navOptionsRequested, this, &MainWindow::showOptionsMenu);
   connect(bolusWindow, &QBolusWindow::navHomeRequested, this, &MainWindow::showHomeWindow);
   connect(optionsMenu, &QOptionsMenu::navHomeRequested, this, &MainWindow::showHomeWindow);
   connect(optionsMenu, &QOptionsMenu::navPersonalRequested, this, &MainWindow::showPersonalProfiles);
   connect(personalProfiles, &QPersonalProfiles::navHomeRequested, this, &MainWindow::showHomeWindow);
}

MainWindow::~MainWindow() {
   delete ui;
   delete pump;
}

void MainWindow::showHomeWindow() {
   stackedWidget->setCurrentWidget(homeWindow);
}

void MainWindow::showBolusWindow() {
   // Always refresh profile before showing the window
   bolusWindow->setPatientProfile(pump->getCurrentProfile());
   stackedWidget->setCurrentWidget(bolusWindow);
}
void MainWindow::showOptionsMenu() {
   stackedWidget->setCurrentWidget(optionsMenu);
}

void MainWindow::showPersonalProfiles() {
   stackedWidget->setCurrentWidget(personalProfiles);
}


