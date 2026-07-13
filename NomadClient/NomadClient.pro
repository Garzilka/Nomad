QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Core/Events/newmessage.cpp \
    Core/connection.cpp \
    UI/Authorization/authorizationwidget.cpp \
    UI/Messager/message.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Core/Events/newmessage.h \
    Core/connection.h \
    UI/Authorization/authorizationwidget.h \
    UI/Messager/message.h \
    mainwindow.h

FORMS += \
    UI/Authorization/authorizationwidget.ui \
    UI/Messager/message.ui \
    mainwindow.ui

QT += core network

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
