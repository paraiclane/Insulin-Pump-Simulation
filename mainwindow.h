#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "qhomewindow.h"
#include "qboluswindow.h"
#include "qlogwindow.h"
#include "qoptionsmenu.h"
#include "qpersonalprofiles.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
   Q_OBJECT
public:
   explicit MainWindow(QWidget *parent = nullptr);
   ~MainWindow();

private slots:
   void showHomeWindow();
   void showBolusWindow();
   void showOptionsMenu();
   void showPersonalProfiles();

private:
   Ui::MainWindow *ui;
   QStackedWidget *stackedWidget;
   QHomeWindow *homeWindow;
   QBolusWindow *bolusWindow;
   QLogWindow *logWindow;
   QOptionsMenu *optionsMenu;
   QPersonalProfiles *personalProfiles;

   Pump *pump;
};

#endif // MAINWINDOW_H


