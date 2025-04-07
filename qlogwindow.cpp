#include "qlogwindow.h"

QLogWindow::QLogWindow(Pump* pump, QWidget *parent) : QMainWindow(parent), pump(pump){
    homeButton = new QPushButton("Home", this);
    log = new QLabel(this);
}
