TEMPLATE = app

QT += core rvinode

TARGET = rviservicepong

HEADERS += \
    rviservicepong.h

SOURCES += \
    main.cpp \
    rviservicepong.cpp

OTHER_FILES += \
    conf.json \
    ex/keys/* \
    ex/credentials/* \
    ex/certificates/*

# need to grab the libs so we can run examples without installing
LIBS += -L../../lib

# makes QtRviNode/... style include work without installing
INCLUDEPATH += $$PWD/../../include

rvi-demo-config.path = $$[QT_INSTALL_EXAMPLES]/rviservicepong
rvi-demo-config.files = ex

target.path = $$[QT_INSTALL_EXAMPLES]/rviservicepong

sources.files = $$SOURCES $$HEADERS $$OTHER_FILES rviservicepong.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/rviservicepong

INSTALLS += target sources rvi-demo-config
