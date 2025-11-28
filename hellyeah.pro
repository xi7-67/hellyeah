QT += core network widgets
TARGET = hellyeah
TEMPLATE = app

CONFIG += c++17 cmdline

LIBS += -ldl -lm -lpthread

SOURCES += src/main.cpp \
           src/api/HifiClient.cpp \
           src/player/AudioPlayer.cpp \
           src/ui/MainWindow.cpp

HEADERS += src/api/HifiClient.hpp \
           src/player/AudioPlayer.hpp \
           src/ui/MainWindow.hpp

# Test target
test {
    TARGET = api_test
    SOURCES = src/api_test.cpp src/api/HifiClient.cpp
    HEADERS = src/api/HifiClient.hpp
    QT -= widgets
}

