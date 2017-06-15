TEMPLATE = app

QT += qml quick rvinode
CONFIG += c++11

TARGET = rviserviceping

HEADERS += \
    rviserviceping.h

SOURCES += \
    main.cpp \
    rviserviceping.cpp

RESOURCES += qml.qrc

OTHER_FILES += \
    conf.json \
    ex/keys/* \
    ex/credentials/* \
    ex/certificates/*

# need to grab the libs so we can run examples without installing
LIBS += -L../../lib

# makes QtRviNode/... style include work without installing
INCLUDEPATH += $$PWD/../../include

rvi-demo-config.path = $$[QT_INSTALL_EXAMPLES]/rviserviceping
rvi-demo-config.files = ex

target.path = $$[QT_INSTALL_EXAMPLES]/rviserviceping

sources.files = $$SOURCES $$HEADERS $$RESOURCES rviserviceping.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/rviserviceping

INSTALLS += target sources rvi-demo-config
