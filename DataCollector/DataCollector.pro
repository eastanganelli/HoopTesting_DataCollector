QT += core gui serialport sql webview

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    database.service.cpp \
    datavisualizer.controller.cpp \
    global.service.cpp \
    main.cpp \
    pressuretempgraph.controller.cpp \
    qcustomplot.controller.cpp \
    serialportmanager.controller.cpp \
    setdb.controller.cpp \
    setserial.controller.cpp \
    setstation.controller.cpp \
    shareddata.controller.cpp \
    simplecrypt.cpp \
    station.controller.cpp \
    stationlist.controller.cpp

HEADERS += \
    database.service.h \
    datavisualizer.controller.h \
    defines.h \
    global.service.h \
    pressuretempgraph.controller.h \
    qcustomplot.controller.h \
    serialportmanager.controller.h \
    setdb.controller.h \
    setserial.controller.h \
    setstation.controller.h \
    shareddata.controller.h \
    simplecrypt.h \
    station.controller.h \
    stationlist.controller.h

FORMS += \
    datavisualizer.controller.ui \
    setdb.controller.ui \
    setserial.controller.ui \
    setstation.controller.ui

# QMAKE_CXXFLAGS += -Wa,-mbig-obj, -Wno-ignored-qualifiers, -Wsign-compare, -Wignored-qualifiers
QMAKE_CXXFLAGS += -Wa,-mbig-obj#,-Wdeprecated-delcarations,-Wignored-qualifiers

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Icons.qrc
