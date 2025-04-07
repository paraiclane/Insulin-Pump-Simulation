#include "qoptionsmenu.h"
#include "ui_optionsmenu.h"

QOptionsMenu::QOptionsMenu(Pump* pump, QWidget *parent) : QWidget(parent), ui(new Ui::OptionsMenu), pump(pump) {
    ui->setupUi(this);


    connect(ui->homeButton, &QPushButton::clicked, this, &QOptionsMenu::onHomeButtonClicked);
    connect(ui->powerOffButton, &QPushButton::clicked, this, &QOptionsMenu::onPowerOffClicked);
    connect(ui->createButton, &QPushButton::clicked, this, &QOptionsMenu::onCreateClicked);
}

QOptionsMenu::~QOptionsMenu() {
    delete ui;
}

void QOptionsMenu::onHomeButtonClicked() {
    emit navHomeRequested();
}

void QOptionsMenu::onPowerOffClicked() {

    qApp->quit();
}

void QOptionsMenu::onCreateClicked() {
    emit navPersonalRequested();
}
