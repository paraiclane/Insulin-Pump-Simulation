#ifndef QLOGWINDOW_H
#define QLOGWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

#include "pump.h"

class QLogWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit QLogWindow(Pump* pump, QWidget *parent = nullptr);

private:
    QPushButton *homeButton;
    QLabel *log;
    Pump* pump;
};

#endif // QLOGWINDOW_H
