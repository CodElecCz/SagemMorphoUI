QT += widgets serialport
requires(qtConfig(combobox))

TARGET = terminal
TEMPLATE = app

SOURCES += \
    Morpho/Ilv_crc.c \
    Morpho/Ilv_errors.c \
    Morpho/morpho.c \
    Morpho/morpho_add_base_record.c \
    Morpho/morpho_configure_uart.c \
    Morpho/morpho_get_base_config.c \
    Morpho/morpho_get_data.c \
    Morpho/morpho_get_descriptor.c \
    main.cpp \
    mainwindow.cpp \
    sagemmorpho.cpp \
    settingsdialog.cpp \
    console.cpp \
    support/style/CustomStyle.cpp

HEADERS += \
    Morpho/Ilv_crc.h \
    Morpho/Ilv_definitions.h \
    Morpho/Ilv_errors.h \
    Morpho/morpho.h \
    Morpho/morpho_add_base_record.h \
    Morpho/morpho_configure_uart.h \
    Morpho/morpho_definitions.h \
    Morpho/morpho_get_base_config.h \
    Morpho/morpho_get_data.h \
    Morpho/morpho_get_descriptor.h \
    mainwindow.h \
    sagemmorpho.h \
    settingsdialog.h \
    console.h \
    support/style/CustomStyle.h

FORMS += \
    mainwindow.ui \
    sagemmorpho.ui \
    settingsdialog.ui

RESOURCES += \
    morpho.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/serialport/terminal
INSTALLS += target
