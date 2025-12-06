/****************************************************************************
** Meta object code from reading C++ file 'HifiClient.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/api/HifiClient.hpp"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HifiClient.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10HifiClientE_t {};
} // unnamed namespace

template <> constexpr inline auto HifiClient::qt_create_metaobjectdata<qt_meta_tag_ZN10HifiClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "HifiClient",
        "searchResults",
        "",
        "QJsonArray",
        "tracks",
        "trackStreamUrl",
        "url",
        "albumLoaded",
        "QJsonObject",
        "albumData",
        "artistLoaded",
        "artistData",
        "artistTopTracksLoaded",
        "artistAlbumsLoaded",
        "albums",
        "errorOccurred",
        "message",
        "onSearchFinished",
        "onTrackStreamFinished",
        "onAlbumFinished",
        "onArtistFinished",
        "onArtistTopTracksFinished",
        "onArtistAlbumsFinished"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'searchResults'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'trackStreamUrl'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'albumLoaded'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Signal 'artistLoaded'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 11 },
        }}),
        // Signal 'artistTopTracksLoaded'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'artistAlbumsLoaded'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 14 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 },
        }}),
        // Slot 'onSearchFinished'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTrackStreamFinished'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAlbumFinished'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onArtistFinished'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onArtistTopTracksFinished'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onArtistAlbumsFinished'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<HifiClient, qt_meta_tag_ZN10HifiClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject HifiClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10HifiClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10HifiClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10HifiClientE_t>.metaTypes,
    nullptr
} };

void HifiClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HifiClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->searchResults((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 1: _t->trackStreamUrl((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->albumLoaded((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 3: _t->artistLoaded((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 4: _t->artistTopTracksLoaded((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 5: _t->artistAlbumsLoaded((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 6: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onSearchFinished(); break;
        case 8: _t->onTrackStreamFinished(); break;
        case 9: _t->onAlbumFinished(); break;
        case 10: _t->onArtistFinished(); break;
        case 11: _t->onArtistTopTracksFinished(); break;
        case 12: _t->onArtistAlbumsFinished(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (HifiClient::*)(const QJsonArray & )>(_a, &HifiClient::searchResults, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (HifiClient::*)(const QString & )>(_a, &HifiClient::trackStreamUrl, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (HifiClient::*)(const QJsonObject & )>(_a, &HifiClient::albumLoaded, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (HifiClient::*)(const QJsonObject & )>(_a, &HifiClient::artistLoaded, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (HifiClient::*)(const QJsonArray & )>(_a, &HifiClient::artistTopTracksLoaded, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (HifiClient::*)(const QJsonArray & )>(_a, &HifiClient::artistAlbumsLoaded, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (HifiClient::*)(const QString & )>(_a, &HifiClient::errorOccurred, 6))
            return;
    }
}

const QMetaObject *HifiClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HifiClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10HifiClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int HifiClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void HifiClient::searchResults(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void HifiClient::trackStreamUrl(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void HifiClient::albumLoaded(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void HifiClient::artistLoaded(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void HifiClient::artistTopTracksLoaded(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void HifiClient::artistAlbumsLoaded(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void HifiClient::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP
