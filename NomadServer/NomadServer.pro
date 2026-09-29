QT = core

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Core/Client/client.cpp \
        Core/Client/clientmanager.cpp \
        Core/DataBase/Rules/userscheckrule.cpp \
        Core/DataBase/checkdatabaserule.cpp \
        Core/DataBase/repository.cpp \
        Core/Lib/cryptolib.cpp \
        Core/core.cpp \
        main.cpp

QT += core network
QT += sql

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Core/Client/client.h \
    Core/Client/clientmanager.h \
    Core/Data/CoreData.h \
    Core/DataBase/Rules/userscheckrule.h \
    Core/DataBase/checkdatabaserule.h \
    Core/DataBase/repository.h \
    Core/Lib/cryptolib.h \
    Core/core.h


INCLUDEPATH += "C:/Program Files/OpenSSL-Win64/include"
LIBS += "C:/Program Files/OpenSSL-Win64/lib/VC/x64/MD/libcrypto.lib"
LIBS += "C:/Program Files/OpenSSL-Win64/lib/VC/x64/MD/libssl.lib"