#-------------------------------------------------
#
# Project created by QtCreator 2014-07-04T19:00:52
#
#-------------------------------------------------

VERSION = 20150310b
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
CONFIG(release, debug|release): DEFINES+=NDEBUG

RC_FILE = SRHDDumpReader.rc

QT       += core gui
CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++1y

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = SRHDDumpReader
TEMPLATE = app

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
    map_colors.json \
    scorers.json \
    presets/detailed_treasures/equipment/treasure arts or nods 45p.dr.json \
    presets/detailed_treasures/equipmentReport/01_GiperJump 45p.dr.json \
    presets/detailed_treasures/equipmentReport/02_Nanitoids 45p.dr.json \
    presets/detailed_treasures/equipmentReport/03_BlastLocaliser 45p.dr.json \
    presets/detailed_treasures/equipmentReport/22_Antigrav 45p.dr.json \
    presets/detailed_treasures/equipmentReport/23_MPA 45p.dr.json \
    presets/detailed_treasures/equipmentReport/24_Forsage 45p.dr.json \
    presets/detailed_treasures/equipmentReport/25_WeaponToSpeed 45p.dr.json \
    presets/detailed_treasures/equipmentReport/31_Transmitters all.dr.json \
    presets/detailed_treasures/equipmentReport/32_Arts_despensable 45p.dr.json \
    presets/detailed_treasures/equipmentReport/41_treasure_arts 45p.dr.json \
    presets/detailed_treasures/equipmentReport/52_treasure_micromodules_1 45p.dr.json \
    presets/detailed_treasures/equipmentReport/53_treasure_micromodules_2 45p.dr.json \
    presets/detailed_treasures/equipmentReport/54_treasure_micromodules_3 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure arts 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure micromodules 45p.dr.json \
    presets/detailed_treasures/planets/big+ gaal 45p.dr.json \
    presets/detailed_treasures/planets/huge fei 45p.dr.json \
    presets/detailed_treasures/planets/huge gaal-fei 45p.dr.json \
    presets/detailed_treasures/planets/huge industrial gaal 30p.dr.json \
    presets/detailed_treasures/planetsReport/huge_industrial_gaal 30p.dr.json \
    presets/equipment/treasure arts or nods 45p.dr.json \
    presets/equipmentReport/treasure arts 45p.dr.json \
    presets/equipmentReport/treasure micromodules 45p.dr.json \
    presets/planets/big+ gaal 45p.dr.json \
    presets/planets/huge fei 45p.dr.json \
    presets/planets/huge gaal-fei 45p.dr.json \
    presets/planets/huge industrial gaal 30p.dr.json \
    presets/planetsReport/huge industrial gaal 30p.dr.json
