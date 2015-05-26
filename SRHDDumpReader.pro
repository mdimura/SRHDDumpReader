#-------------------------------------------------
#
# Project created by QtCreator 2014-07-04T19:00:52
#
#-------------------------------------------------

COMMIT_BRANCH = $$system(git rev-parse --abbrev-ref HEAD)
COMMIT_DATE = $$system(git show -s --pretty='%ci')
COMMIT_DATE = $$first(COMMIT_DATE)
COMMIT_HASH = $$system(git log --pretty=format:'%h' -n 1)
DEFINES += APP_VERSION=\\\"$$COMMIT_DATE-$$COMMIT_BRANCH-$$COMMIT_HASH\\\"
CONFIG(release, debug|release): DEFINES+=NDEBUG

RC_FILE = SRHDDumpReader.rc

QT       += core gui
CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++14 #-pthread -Wl,--no-as-needed
QMAKE_LFLAGS += -std=c++14 #-pthread -Wl,--no-as-needed


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
    presets/detailed_treasures/equipmentReport/treasure Antigrav 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure arts 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure BlastLocaliser 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure despensable arts 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure Forsage 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure GiperJump 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure micromodules I 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure micromodules II 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure micromodules III 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure MPA 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure Nanitoids 45p.dr.json \
    presets/detailed_treasures/equipmentReport/treasure Transmitters all.dr.json \
    presets/detailed_treasures/equipmentReport/treasure WeaponToSpeed 45p.dr.json \
    presets/detailed_treasures/planets/big+ industrial gaal 45p.dr.json \
    presets/detailed_treasures/planets/huge industrial fei 45p.dr.json \
    presets/detailed_treasures/planets/huge industrial gaal 30p.dr.json \
    presets/detailed_treasures/planets/huge industrial gaal-fei 45p.dr.json \
    presets/detailed_treasures/planetsReport/big+ industrial gaal 45p.dr.json \
    presets/detailed_treasures/planetsReport/huge industrial fei 45p.dr.json \
    presets/detailed_treasures/planetsReport/huge industrial gaal 30p.dr.json \
    presets/detailed_treasures/planetsReport/huge industrial gaal-fei 45p.dr.json \
    presets/equipment/treasure arts or nods 45p.dr.json \
    presets/equipmentReport/treasure arts 45p.dr.json \
    presets/equipmentReport/treasure micromodules 45p.dr.json \
    presets/planets/big+ gaal 45p.dr.json \
    presets/planets/huge fei 45p.dr.json \
    presets/planets/huge gaal-fei 45p.dr.json \
    presets/planets/huge industrial gaal 30p.dr.json \
    presets/planetsReport/huge industrial gaal 30p.dr.json \
    SRHDDumpReader_ru.ts
