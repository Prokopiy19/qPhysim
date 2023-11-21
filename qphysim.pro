QT += widgets

HEADERS = render_area.h \
          simulation.h \
          window.h

SOURCES = main.cpp \
          render_area.cpp \
          simulation.cpp \
          window.cpp

win32: LIBS += -lbox2d

CONFIG += c++20
