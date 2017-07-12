CONFIG += plugin
TEMPLATE = lib
QT += widgets designer
TARGET = $$qtLibraryTarget($$TARGET)
HEADERS = CColorPickerPlugin.h \
    CImagePickerPlugin.h \
    EZWWidgets.h
SOURCES = CColorPickerPlugin.cpp \
    CImagePickerPlugin.cpp \
    EZWWidgets.cpp
RESOURCES = icons.qrc
LIBS += -L.
target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
include(ccolorpicker.pri)
include(cimagepicker.pri)
