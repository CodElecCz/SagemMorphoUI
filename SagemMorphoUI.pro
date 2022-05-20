QT += widgets serialport
requires(qtConfig(combobox))

TARGET = terminal
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    sagemmorpho.cpp \
    settingsdialog.cpp \
    console.cpp

HEADERS += \
    mainwindow.h \
    sagemmorpho.h \
    settingsdialog.h \
    console.h

FORMS += \
    mainwindow.ui \
    sagemmorpho.ui \
    settingsdialog.ui

RESOURCES += \
    morpho.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/serialport/terminal
INSTALLS += target
