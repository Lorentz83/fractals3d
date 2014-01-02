TEMPLATE = app
TARGET = fractals3d
QT += opengl

CONFIG += warn_on
unix:LIBS += -lGLEW
win32:LIBS += -lGLEW32
win32:RC_FILE = resource.rc

# Input
HEADERS += glfractal.h window.h colorpicker.h \
           object3d.h texture.h \
           geometry.h rotation.h 

FORMS += mainwindow.ui colorpicker.ui
SOURCES += glfractal.cpp window.cpp colorpicker.cpp \
           object3d.cpp texture.cpp \
           geometry.cpp rotation.cpp 

RESOURCES += resource.qrc

## Installs

extrafiles.path += ../bin
extrafiles.files += colori frattali

target.path += ../bin

INSTALLS += target extrafiles
