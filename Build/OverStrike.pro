
TEMPLATE = app
TARGET   = OverStrike

win32 {
    DESTDIR = C:/Users/labadmin/Desktop/SGLBUILD/FIXU/OverStrike/OverStrike-win
#    DESTDIR = C:/Users/labadmin/Desktop/SGLBUILD/FIXU/OverStrike/Debug
}

unix {
    DESTDIR = /home/billkarsh/Code/OverStrike/OverStrike-linux
}

QT += widgets

HEADERS +=              \
    CGBL.h              \
    Cmdline.h           \
    KVParams.h          \
    SGLTypes.h          \
    Subset.h            \
    Tool.h              \
    Util.h

SOURCES +=              \
    main.cpp            \
    CGBL.cpp            \
    Cmdline.cpp         \
    KVParams.cpp        \
    Subset.cpp          \
    Tool.cpp            \
    Util.cpp            \
    Util_osdep.cpp

win32 {
    LIBS    += -lWS2_32 -lUser32 -lwinmm
    DEFINES += _CRT_SECURE_NO_WARNINGS WIN32
}

QMAKE_TARGET_COMPANY = Bill Karsh
QMAKE_TARGET_PRODUCT = OverStrike
QMAKE_TARGET_DESCRIPTION = Zero probe data
QMAKE_TARGET_COPYRIGHT = (c) 2020, Bill Karsh, All rights reserved
VERSION = 1.0


