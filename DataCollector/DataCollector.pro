QT += core gui serialport sql webview

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    components/datavisualizer.cpp \
    components/setdb.cpp \
    components/setserial.cpp \
    components/setstation.cpp \
    main.cpp \
    services/database.cpp \
    services/global.cpp \
    services/pressuretempgraph.cpp \
    services/schemas/schemaData.cpp \
    services/schemas/schemaFront.cpp \
    services/schemas/schemas.cpp \
    services/serialportmanager.cpp \
    utils/qcustomplot.cpp \
    utils/shareddata.cpp \
    utils/sharedstations.cpp \
    utils/simplecrypt.cpp \
    utils/station.cpp \
    utils/stationlist.cpp

HEADERS += \
    components/datavisualizer.h \
    components/setdb.h \
    components/setserial.h \
    components/setstation.h \
    defines.h \
    services/database.h \
    services/global.h \
    services/pressuretempgraph.h \
    services/schemas/schemaData.h \
    services/schemas/schemaFront.h \
    services/schemas/schemas.h \
    services/serialportmanager.h \
    utils/qcustomplot.h \
    utils/shareddata.h \
    utils/sharedstations.h \
    utils/simplecrypt.h \
    utils/station.h \
    utils/stationlist.h

FORMS += \
    components/datavisualizer.ui \
    components/setdb.ui \
    components/setserial.ui \
    components/setstation.ui

# QMAKE_CXXFLAGS += -Wa,-mbig-obj, -Wno-ignored-qualifiers, -Wsign-compare, -Wignored-qualifiers
# QMAKE_CXXFLAGS += -Wa,-mbig-obj#,-Wdeprecated-delcarations,-Wignored-qualifiers

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Icons.qrc
