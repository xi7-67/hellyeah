/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/ui/MainWindow.hpp"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "onSearchClicked",
        "",
        "onTrackSelected",
        "QListWidgetItem*",
        "item",
        "onTrackStreamUrl",
        "url",
        "onPlayerError",
        "message",
        "onApiError",
        "onPlayPauseClicked",
        "onAlbumLoaded",
        "QJsonObject",
        "album",
        "onVolumeChanged",
        "volume",
        "onSeekSliderMoved",
        "position",
        "onSeekSliderPressed",
        "onSeekSliderReleased",
        "onPositionChanged",
        "onDurationChanged",
        "duration",
        "onCoverImageDownloaded",
        "QNetworkReply*",
        "reply",
        "onFavoriteToggled",
        "track",
        "isFavorite",
        "onDownloadFinished",
        "trackId",
        "filePath",
        "onHomeClicked",
        "onFavoriteCardClicked",
        "onFavoriteCoverDownloaded",
        "onAddAlbumClicked",
        "onAlbumCardClicked",
        "albumId",
        "onAddToAlbumClicked",
        "onAlbumDeleteClicked",
        "onNextClicked",
        "onPrevClicked",
        "onArtistClicked",
        "artistId",
        "artistName"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onSearchClicked'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTrackSelected'
        QtMocHelpers::SlotData<void(QListWidgetItem *)>(3, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Slot 'onTrackStreamUrl'
        QtMocHelpers::SlotData<void(const QString &)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onPlayerError'
        QtMocHelpers::SlotData<void(const QString &)>(8, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'onApiError'
        QtMocHelpers::SlotData<void(const QString &)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'onPlayPauseClicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAlbumLoaded'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Slot 'onVolumeChanged'
        QtMocHelpers::SlotData<void(int)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 16 },
        }}),
        // Slot 'onSeekSliderMoved'
        QtMocHelpers::SlotData<void(int)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 18 },
        }}),
        // Slot 'onSeekSliderPressed'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSeekSliderReleased'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPositionChanged'
        QtMocHelpers::SlotData<void(qint64)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 18 },
        }}),
        // Slot 'onDurationChanged'
        QtMocHelpers::SlotData<void(qint64)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 23 },
        }}),
        // Slot 'onCoverImageDownloaded'
        QtMocHelpers::SlotData<void(QNetworkReply *)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 25, 26 },
        }}),
        // Slot 'onFavoriteToggled'
        QtMocHelpers::SlotData<void(const QJsonObject &, bool)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 13, 28 }, { QMetaType::Bool, 29 },
        }}),
        // Slot 'onDownloadFinished'
        QtMocHelpers::SlotData<void(int, const QString &)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 31 }, { QMetaType::QString, 32 },
        }}),
        // Slot 'onHomeClicked'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFavoriteCardClicked'
        QtMocHelpers::SlotData<void(int)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 31 },
        }}),
        // Slot 'onFavoriteCoverDownloaded'
        QtMocHelpers::SlotData<void(QNetworkReply *)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 25, 26 },
        }}),
        // Slot 'onAddAlbumClicked'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAlbumCardClicked'
        QtMocHelpers::SlotData<void(int)>(37, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 38 },
        }}),
        // Slot 'onAddToAlbumClicked'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(39, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 13, 28 },
        }}),
        // Slot 'onAlbumDeleteClicked'
        QtMocHelpers::SlotData<void(int)>(40, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 38 },
        }}),
        // Slot 'onNextClicked'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPrevClicked'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onArtistClicked'
        QtMocHelpers::SlotData<void(int, const QString &)>(43, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 44 }, { QMetaType::QString, 45 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onSearchClicked(); break;
        case 1: _t->onTrackSelected((*reinterpret_cast<std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 2: _t->onTrackStreamUrl((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->onPlayerError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->onApiError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onPlayPauseClicked(); break;
        case 6: _t->onAlbumLoaded((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 7: _t->onVolumeChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->onSeekSliderMoved((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->onSeekSliderPressed(); break;
        case 10: _t->onSeekSliderReleased(); break;
        case 11: _t->onPositionChanged((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 12: _t->onDurationChanged((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 13: _t->onCoverImageDownloaded((*reinterpret_cast<std::add_pointer_t<QNetworkReply*>>(_a[1]))); break;
        case 14: _t->onFavoriteToggled((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 15: _t->onDownloadFinished((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 16: _t->onHomeClicked(); break;
        case 17: _t->onFavoriteCardClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 18: _t->onFavoriteCoverDownloaded((*reinterpret_cast<std::add_pointer_t<QNetworkReply*>>(_a[1]))); break;
        case 19: _t->onAddAlbumClicked(); break;
        case 20: _t->onAlbumCardClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 21: _t->onAddToAlbumClicked((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 22: _t->onAlbumDeleteClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 23: _t->onNextClicked(); break;
        case 24: _t->onPrevClicked(); break;
        case 25: _t->onArtistClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QNetworkReply* >(); break;
            }
            break;
        case 18:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QNetworkReply* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    }
    return _id;
}
QT_WARNING_POP
