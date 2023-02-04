QT       += core gui concurrent

equals(QT_MAJOR_VERSION, 5){
  win32:QT += winextras
}
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

PRECOMPILED_HEADER = src/pch.h

INCLUDEPATH += src src/ui ../Qtlib/src ../Qtlib/src/ui ../Qtlib/include

CONFIG( debug, debug|release ) {
    # debug
    win32:LIBS += -L../Qtlib/debug -L../Qtlib/QtLibSound/debug
} else {
    # release
    win32:LIBS += -L../Qtlib/release -L../Qtlib/QtLibSound/release
}

CONFIG(release, debug|release): DEFINES += NDEBUG

LIBS += Qtlib.lib

RESOURCES += \
  src/res.qrc

RC_ICONS = src/icon/AppIcon.ico

SOURCES += \
    src/App.cpp \
    src/Config.cpp \
    src/DriveCache.cpp \
    src/FileDB.cpp \
    src/ActionFile.cpp \
    src/ContextMenu.cpp \
    src/ItemFileInfo.cpp \
    src/ItemL.cpp \
    src/ItemR.cpp \
    src/SelectiHistory.cpp \
    src/UIRowLinkItem.cpp \
    src/UIViewL.cpp \
    src/UIViewR.cpp \
    src/utils.cpp \
    src/UIDriveButton.cpp \
    src/UIMainWindow.cpp

HEADERS += \
    src/App.h \
    src/Config.h \
    src/DriveCache.h \
    src/Enums.h \
    src/FileDB.h \
    src/ActionFile.h \
    src/ContextMenu.h \
    src/ItemFileInfo.h \
    src/ItemL.h \
    src/ItemR.h \
    src/SelectiHistory.h \
    src/UIRowLinkItem.h \
    src/UIViewL.h \
    src/UIViewR.h \
    src/utils.h \
    src/UIDriveButton.h \
    src/UIMainWindow.h

FORMS += \
    src/UIDriveButton.ui \
    src/UIMainWindow.ui \
    src/UIRowLinkItem.ui \
    src/UIViewL.ui \
    src/UIViewR.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
