QT       += core gui
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bolus.cpp \
    home.cpp \
    log.cpp \
    main.cpp \
    mainwindow.cpp \
    pump.cpp \
    qboluswindow.cpp \
    qerrormessage.cpp \
    qhomewindow.cpp \
    qlogwindow.cpp \
    qoptionsmenu.cpp \
    qpersonalprofiles.cpp \
    profile.cpp \
    profileManager.cpp \


HEADERS += \
    bolus.h \
    home.h \
    log.h \
    mainwindow.h \
    pump.h \
    qboluswindow.h \
    qerrormessage.h \
    qhomewindow.h \
    qlogwindow.h \
    qoptionsmenu.h \
    qpersonalprofiles.h \
    profile.h \
    profileManager.h

FORMS += \
    mainwindow.ui \
    homewindow.ui \
    boluswindow.ui \
    logwindow.ui \
    optionsmenu.ui \
    personalprofiles.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
