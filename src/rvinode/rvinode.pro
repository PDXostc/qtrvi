TARGET = QtRviNode
QT = core core-private
VERSION = 1.0.0

CMAKE_MODULE_TESTS = '-'

INCLUDEPATH *= .

HEADERS += \
    qrvinode.h \
    qtrvinode_global.h \
    qrvinodemonitor_p.h \
    qrviserviceinterface.h

SOURCES += \
    qrvinode.cpp \
    qrvinodemonitor.cpp \
    qrviserviceinterface.cpp

RVI_INCLUDE_PATH=$$(RVI_INCLUDE_PATH)
RVI_LIB_PATH=$$(RVI_LIB_PATH)

!isEmpty(RVI_LIB_PATH):!isEmpty(RVI_INCLUDE_PATH) {
    unix {
	INCLUDEPATH += $$RVI_INCLUDE_PATH
	LIBS += -L$$RVI_LIB_PATH
    }
} else {
    unix {
	message("Notice! RVI_LIB_PATH and/or RVI_INCLUDE_PATH environment variables not set, may not be able to see librvi")
    }
    qnx {
	message($$(QNX_TARGET))
	INCLUDEPATH += $$(QNX_TARGET)/x86/include
	LIBS += -L$$(QNX_TARGET)/x86/lib
    }
}

LIBS += -lrvi

load(qt_module)
