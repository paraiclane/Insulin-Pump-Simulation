#ifndef QOPTIONSMENU_H
#define QOPTIONSMENU_H

#include <QWidget>
#include "pump.h"

namespace Ui {
class OptionsMenu;
}

class QOptionsMenu : public QWidget {
    Q_OBJECT
public:
    explicit QOptionsMenu(Pump* pump, QWidget *parent = nullptr);
    ~QOptionsMenu();

signals:
    void navHomeRequested();
    void navPersonalRequested();

private slots:
    void onHomeButtonClicked();
    void onPowerOffClicked();
    void onCreateClicked();

private:
    Ui::OptionsMenu *ui;
    Pump* pump;
};

#endif // QOPTIONSMENU_H
