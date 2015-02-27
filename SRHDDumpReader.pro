#-------------------------------------------------
#
# Project created by QtCreator 2014-07-04T19:00:52
#
#-------------------------------------------------

VERSION = 20150218b
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
CONFIG(release, debug|release): DEFINES+=NDEBUG

RC_FILE = SRHDDumpReader.rc

QT       += core gui
CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++1y

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = SRHDDumpReader
TEMPLATE = app
CONFIG += c++11

win32:LIBS += -L"$$_PRO_FILE_PWD_/3rdparty/libs/" -lpsapi

SOURCES += main.cpp\
	MainWindow.cpp \
    Equipment.cpp \
    Ship.cpp \
    Planet.cpp \
    BlackHole.cpp \
    Star.cpp \
    GoodsArr.cpp \
    Galaxy.cpp \
    TradeTableModel.cpp \
    HierarchicalHeaderView.cpp \
    EquipmentTableModel.cpp \
    BlackHolesTableModel.cpp \
    PlanetsTableModel.cpp \
    FilterHorizontalHeaderView.cpp \
    SortMultiFilterProxyModel.cpp

HEADERS  += MainWindow.h \
    Equipment.h \
    Ship.h \
    Planet.h \
    BlackHole.h \
    Star.h \
    GoodsArr.h \
    Galaxy.h \
    TradeTableModel.h \
    HierarchicalHeaderView.h \
    EquipmentTableModel.h \
    BlackHolesTableModel.h \
    PlanetsTableModel.h \
    FilterHorizontalHeaderView.h \
    SortMultiFilterProxyModel.h

FORMS    += MainWindow.ui

RESOURCES += \
    icons.qrc

DISTFILES += \
    .gitignore \
    bonus_descriptions_micromodules_en.json \
    bonus_descriptions_micromodules_ru.json \
    Click1.wav \
    make_ru.sh \
    translate_micromodules.sh \
    galaxy_magnifying_glass.ico \
    bonus_descriptions.json \
    bonus_descriptions_artifacts.json \
    bonus_descriptions_hulls.json \
    bonus_descriptions_micromodules.json \
    translate_descriptions.sh \
    bonus_descriptions.json.en \
    bonus_descriptions_hulls.json.en \
    bonus_descriptions_micromodules.json.ru \
    en-ru.txt \
    micromodules_ru-en.txt \
    minRowsPreset.json \
    map_colors.json
