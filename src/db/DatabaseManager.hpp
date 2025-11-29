#pragma once

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QSqlDatabase>

class DatabaseManager : public QObject {
  Q_OBJECT

public:
  static DatabaseManager &instance();

  bool addFavorite(const QJsonObject &track);
  bool removeFavorite(int trackId);
  bool isFavorite(int trackId);
  QList<QJsonObject> getFavorites();
  QJsonObject getTrack(int trackId);
  bool updateFilePath(int trackId, const QString &filePath);
  bool updateCoverPath(int trackId, const QString &coverPath);
  QString getFilePath(int trackId);
  QString getCoverPath(int trackId);

  // Album methods
  int createAlbum(const QString &name);
  QList<QJsonObject> getAlbums();
  bool addTrackToAlbum(int albumId, const QJsonObject &track);
  QList<QJsonObject> getAlbumTracks(int albumId);
  bool deleteAlbum(int albumId);

private:
  explicit DatabaseManager(QObject *parent = nullptr);
  ~DatabaseManager();
  DatabaseManager(const DatabaseManager &) = delete;
  DatabaseManager &operator=(const DatabaseManager &) = delete;

  void initDatabase();
  QSqlDatabase m_db;
};
