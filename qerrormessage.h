#ifndef QERRORMESSAGE_H
#define QERRORMESSAGE_H

#include <QTextEdit>
#include "qwidget.h"
#include "pump.h"

class QErrorMessage : public QTextEdit {
    Q_OBJECT
public:
    explicit QErrorMessage(Pump* pump, QWidget *parent = nullptr);
    void showMessage(QString message);
    void updateAlert();

private:
    QString message;
    Pump* pump;
};

#endif // QERRORMESSAGE_H
