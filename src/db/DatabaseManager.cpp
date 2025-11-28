#include "DatabaseManager.hpp"
#include <QDebug>
#include <QDir>
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
             "json_data TEXT)");
}

bool DatabaseManager::addFavorite(const QJsonObject &track) {
  QSqlQuery query;
  query.prepare("INSERT OR REPLACE INTO favorites (id, title, artist, album, "
                "cover_id, json_data) "
                "VALUES (:id, :title, :artist, :album, :cover_id, :json_data)");

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
  query.bindValue(":json_data", jsonData);

  if (!query.exec()) {
    qDebug() << "addFavorite error:" << query.lastError();
    return false;
  }
  return true;
}

bool DatabaseManager::removeFavorite(int trackId) {
  QSqlQuery query;
  query.prepare("DELETE FROM favorites WHERE id = :id");
  query.bindValue(":id", trackId);
  return query.exec();
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
  QSqlQuery query("SELECT json_data FROM favorites");
  while (query.next()) {
    QString jsonStr = query.value(0).toString();
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    list.append(doc.object());
  }
  return list;
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
