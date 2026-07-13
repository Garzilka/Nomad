QT = core

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Core/Client/client.cpp \
        Core/Client/clientmanager.cpp \
        Core/Data/repository.cpp \
        Core/core.cpp \
        main.cpp

QT += core network

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Core/Client/client.h \
    Core/Client/clientmanager.h \
    Core/Data/repository.h \
    Core/core.h
