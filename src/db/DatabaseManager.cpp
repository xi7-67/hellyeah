#include "DatabaseManager.hpp"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

DatabaseManager &DatabaseManager::instance() {
  static DatabaseManager instance;
  return instance;
}

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {
  initDatabase();
}

DatabaseManager::~DatabaseManager() {
  if (m_db.isOpen()) {
    m_db.close();
  }
}

void DatabaseManager::initDatabase() {
  QString dataPath =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir dir(dataPath);
  if (!dir.exists()) {
    dir.mkpath(".");
  }

  QString dbPath = dir.filePath("hellyeah.db");
  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setDatabaseName(dbPath);

  if (!m_db.open()) {
    qDebug() << "Error: connection with database failed" << m_db.lastError();
  } else {
    qDebug() << "Database: connection ok";
  }

  QSqlQuery query;
  query.exec("CREATE TABLE IF NOT EXISTS favorites ("
             "id INTEGER PRIMARY KEY, "
             "title TEXT, "
             "artist TEXT, "
             "album TEXT, "
             "cover_id TEXT, "
             "file_path TEXT, "
             "cover_path TEXT, "
             "json_data TEXT)");

  // Migration: Ensure cover_path exists for existing databases
  if (!query.exec("SELECT cover_path FROM favorites LIMIT 1")) {
    qDebug() << "Migrating database: adding cover_path column";
    query.exec("ALTER TABLE favorites ADD COLUMN cover_path TEXT");
  }

  query.exec("CREATE TABLE IF NOT EXISTS albums ("
             "id INTEGER PRIMARY KEY AUTOINCREMENT, "
             "name TEXT, "
             "cover_id TEXT, "
             "created_at INTEGER)");

  query.exec("CREATE TABLE IF NOT EXISTS album_tracks ("
             "album_id INTEGER, "
             "track_id INTEGER, "
             "added_at INTEGER, "
             "PRIMARY KEY (album_id, track_id))");
}

bool DatabaseManager::addFavorite(const QJsonObject &track) {
  QSqlQuery query;
  query.prepare("INSERT OR REPLACE INTO favorites (id, title, artist, album, "
                "cover_id, json_data, cover_path) "
                "VALUES (:id, :title, :artist, :album, :cover_id, :json_data, "
                ":cover_path)");

  int id = track["id"].toInt();
  QString title = track["title"].toString();
  QString artist = track["artist"].toObject()["name"].toString();
  QString album = track["album"].toObject()["title"].toString();
  QString coverId = track["album"].toObject()["cover"].toString();
  QString jsonData = QJsonDocument(track).toJson(QJsonDocument::Compact);

  query.bindValue(":id", id);
  query.bindValue(":title", title);
  query.bindValue(":artist", artist);
  query.bindValue(":album", album);
  query.bindValue(":cover_id", coverId);
  query.bindValue(":cover_id", coverId);
  query.bindValue(":json_data", jsonData);

  // Check if we have a cover path
  QString coverPath = "";
  if (track.contains("coverPath")) {
    coverPath = track["coverPath"].toString();
  }
  query.bindValue(":cover_path", coverPath);

  if (!query.exec()) {
    qDebug() << "addFavorite error:" << query.lastError();
    return false;
  }
  return true;
}

bool DatabaseManager::removeFavorite(int trackId) {
  // Get file paths before deleting the record
  QString filePath = getFilePath(trackId);
  QString coverPath = getCoverPath(trackId);

  QSqlQuery query;
  query.prepare("DELETE FROM favorites WHERE id = :id");
  query.bindValue(":id", trackId);

  if (query.exec()) {
    // If DB deletion was successful, remove the files
    if (!filePath.isEmpty() && QFile::exists(filePath)) {
      if (QFile::remove(filePath)) {
        qDebug() << "Deleted audio file:" << filePath;
      } else {
        qDebug() << "Failed to delete audio file:" << filePath;
      }
    }

    if (!coverPath.isEmpty() && QFile::exists(coverPath)) {
      if (QFile::remove(coverPath)) {
        qDebug() << "Deleted cover file:" << coverPath;
      } else {
        qDebug() << "Failed to delete cover file:" << coverPath;
      }
    }
    return true;
  }
  return false;
}

bool DatabaseManager::isFavorite(int trackId) {
  QSqlQuery query;
  query.prepare("SELECT id FROM favorites WHERE id = :id");
  query.bindValue(":id", trackId);
  if (query.exec() && query.next()) {
    return true;
  }
  return false;
}

QList<QJsonObject> DatabaseManager::getFavorites() {
  QList<QJsonObject> list;
  QSqlQuery query("SELECT json_data, file_path, cover_path FROM favorites");
  while (query.next()) {
    QString jsonStr = query.value("json_data").toString();
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject track = doc.object();

    // Add file path and cover path from DB
    track["filePath"] = query.value("file_path").toString();
    track["coverPath"] = query.value("cover_path").toString();

    list.append(track);
  }
  return list;
}

QJsonObject DatabaseManager::getTrack(int trackId) {
  QSqlQuery query;
  query.prepare(
      "SELECT json_data, file_path, cover_path FROM favorites WHERE id = :id");
  query.bindValue(":id", trackId);

  if (query.exec() && query.next()) {
    QString jsonStr = query.value("json_data").toString();
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject track = doc.object();

    // Add file path and cover path from DB
    track["filePath"] = query.value("file_path").toString();
    track["coverPath"] = query.value("cover_path").toString();

    return track;
  }

  return QJsonObject(); // Return empty object if not found
}

bool DatabaseManager::updateCoverPath(int trackId, const QString &coverPath) {
  qDebug() << "DB: updateCoverPath for" << trackId << "to" << coverPath;
  QSqlQuery query;
  query.prepare("UPDATE favorites SET cover_path = :path WHERE id = :id");
  query.bindValue(":path", coverPath);
  query.bindValue(":id", trackId);
  bool success = query.exec();
  if (!success)
    qDebug() << "DB: updateCoverPath failed:" << query.lastError();
  return success;
}

bool DatabaseManager::updateFilePath(int trackId, const QString &filePath) {
  QSqlQuery query;
  query.prepare("UPDATE favorites SET file_path = :path WHERE id = :id");
  query.bindValue(":path", filePath);
  query.bindValue(":id", trackId);
  return query.exec();
}

QString DatabaseManager::getFilePath(int trackId) {
  QSqlQuery query;
  query.prepare("SELECT file_path FROM favorites WHERE id = :id");
  query.bindValue(":id", trackId);
  if (query.exec() && query.next()) {
    return query.value(0).toString();
  }
  return QString();
}

QString DatabaseManager::getCoverPath(int trackId) {
  QSqlQuery query;
  query.prepare("SELECT cover_path FROM favorites WHERE id = :id");
  query.bindValue(":id", trackId);
  if (query.exec() && query.next()) {
    QString path = query.value(0).toString();
    qDebug() << "DB: getCoverPath for" << trackId << "returned" << path;
    return path;
  }
  qDebug() << "DB: getCoverPath for" << trackId
           << "returned empty (or query failed)";
  return QString();
}

int DatabaseManager::createAlbum(const QString &name) {
  QSqlQuery query;
  query.prepare(
      "INSERT INTO albums (name, created_at) VALUES (:name, :created_at)");
  query.bindValue(":name", name);
  query.bindValue(":created_at", QDateTime::currentSecsSinceEpoch());

  if (query.exec()) {
    return query.lastInsertId().toInt();
  }
  qDebug() << "createAlbum error:" << query.lastError();
  return -1;
}

QList<QJsonObject> DatabaseManager::getAlbums() {
  QList<QJsonObject> list;
  QSqlQuery query(
      "SELECT id, name, cover_id FROM albums ORDER BY created_at DESC");
  while (query.next()) {
    QJsonObject album;
    album["id"] = query.value(0).toInt();
    album["title"] = query.value(1).toString();

    // Construct a minimal album object compatible with our UI
    QJsonObject artistObj;
    artistObj["name"] = "User Album"; // Placeholder
    album["artist"] = artistObj;

    // If cover_id is set, use it. Otherwise we might need to fetch it from
    // first track later. For now, let's leave it empty or use a placeholder if
    // null.
    QString coverId = query.value(2).toString();
    if (!coverId.isEmpty()) {
      album["cover"] = coverId;
    }

    list.append(album);
  }
  return list;
}

bool DatabaseManager::addTrackToAlbum(int albumId, const QJsonObject &track) {
  int trackId = track["id"].toInt();

  // Ensure track is in favorites database (so we can display it later)
  addFavorite(track);

  // Check if album has a cover, if not, use this track's cover
  QSqlQuery checkCover;
  checkCover.prepare("SELECT cover_id FROM albums WHERE id = :album_id");
  checkCover.bindValue(":album_id", albumId);
  if (checkCover.exec() && checkCover.next()) {
    QString currentCover = checkCover.value(0).toString();
    if (currentCover.isEmpty()) {
      // Set album cover to this track's cover
      QString trackCover = track["album"].toObject()["cover"].toString();
      if (!trackCover.isEmpty()) {
        QSqlQuery updateCover;
        updateCover.prepare(
            "UPDATE albums SET cover_id = :cover WHERE id = :album_id");
        updateCover.bindValue(":cover", trackCover);
        updateCover.bindValue(":album_id", albumId);
        updateCover.exec();
      }
    }
  }

  QSqlQuery query;
  query.prepare(
      "INSERT OR IGNORE INTO album_tracks (album_id, track_id, added_at) "
      "VALUES (:album_id, :track_id, :added_at)");
  query.bindValue(":album_id", albumId);
  query.bindValue(":track_id", trackId);
  query.bindValue(":added_at", QDateTime::currentSecsSinceEpoch());

  return query.exec();
}

QList<QJsonObject> DatabaseManager::getAlbumTracks(int albumId) {
  QList<QJsonObject> list;
  QSqlQuery query;
  // Join with favorites to get track data.
  // This assumes tracks in albums MUST be in favorites table.
  // If we want to support non-favorites in albums, we'd need a separate
  // 'tracks' table that is a superset of favorites. For simplicity, let's
  // assume we only show tracks that are in our favorites cache.
  query.prepare("SELECT f.json_data FROM favorites f "
                "JOIN album_tracks at ON f.id = at.track_id "
                "WHERE at.album_id = :album_id "
                "ORDER BY at.added_at ASC");
  query.bindValue(":album_id", albumId);

  if (query.exec()) {
    while (query.next()) {
      QString jsonStr = query.value(0).toString();
      QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
      list.append(doc.object());
    }
  }
  return list;
}

bool DatabaseManager::deleteAlbum(int albumId) {
  // First delete all track associations
  QSqlQuery deleteTracksQuery;
  deleteTracksQuery.prepare(
      "DELETE FROM album_tracks WHERE album_id = :album_id");
  deleteTracksQuery.bindValue(":album_id", albumId);
  deleteTracksQuery.exec();

  // Then delete the album itself
  QSqlQuery deleteAlbumQuery;
  deleteAlbumQuery.prepare("DELETE FROM albums WHERE id = :id");
  deleteAlbumQuery.bindValue(":id", albumId);

  if (!deleteAlbumQuery.exec()) {
    qDebug() << "deleteAlbum error:" << deleteAlbumQuery.lastError();
    return false;
  }
  return true;
}
