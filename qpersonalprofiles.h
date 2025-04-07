#ifndef QPERSONALPROFILES_H
#define QPERSONALPROFILES_H

#include <QWidget>
#include "pump.h"

namespace Ui {
    class PersonalProfiles;
}

class QPersonalProfiles : public QWidget {
    Q_OBJECT

public:
    explicit QPersonalProfiles(Pump* p, QWidget *parent = nullptr);
    ~QPersonalProfiles();

signals:
    void navHomeRequested();

private slots:
    void onHomeButtonClicked();

    void createProfile();
    void readProfile();
    void updateProfile();
    void deleteProfile();

    void listProfiles();

private:
    Ui::PersonalProfiles *ui;
    Pump* pump;

};

#endif // QPERSONALPROFILES_H
