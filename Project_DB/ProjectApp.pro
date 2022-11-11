QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chattingform_client.cpp \
    client.cpp \
    clientmanager.cpp \
    logdata.cpp \
    main.cpp \
    mainwindow.cpp \
    product.cpp \
    productmanager.cpp \
    serverside.cpp \
    shopping.cpp \
    shoppingmanager.cpp

HEADERS += \
    chattingform_client.h \
    client.h \
    clientmanager.h \
    logdata.h \
    mainwindow.h \
    product.h \
    productmanager.h \
    serverside.h \
    shopping.h \
    shoppingmanager.h

FORMS += \
    clientmanager.ui \
    mainwindow.ui \
    productmanager.ui \
    serverside.ui \
    shoppingmanager.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
