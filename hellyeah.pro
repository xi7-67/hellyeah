QT += core network widgets sql
TARGET = hellyeah
TEMPLATE = app

CONFIG += c++17 cmdline

LIBS += -ldl -lm -lpthread

SOURCES += src/main.cpp \
           src/api/HifiClient.cpp \
           src/player/AudioPlayer.cpp \
           src/ui/MainWindow.cpp \
           src/ui/TrackItemWidget.cpp \
           src/ui/FavoriteCard.cpp \
           src/ui/AlbumCard.cpp \
           src/db/DatabaseManager.cpp \
           src/net/DownloadManager.cpp

HEADERS += src/api/HifiClient.hpp \
           src/player/AudioPlayer.hpp \
           src/ui/MainWindow.hpp \
           src/ui/TrackItemWidget.hpp \
           src/ui/FavoriteCard.hpp \
           src/ui/AlbumCard.hpp \
           src/db/DatabaseManager.hpp \
           src/net/DownloadManager.hpp

# Test target
test {
    TARGET = api_test
    SOURCES = src/api_test.cpp src/api/HifiClient.cpp
    HEADERS = src/api/HifiClient.hpp
    QT -= widgets
}

