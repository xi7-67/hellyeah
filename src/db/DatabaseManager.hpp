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
  bool updateFilePath(int trackId, const QString &filePath);
  QString getFilePath(int trackId);

private:
  explicit DatabaseManager(QObject *parent = nullptr);
  ~DatabaseManager();
  DatabaseManager(const DatabaseManager &) = delete;
  DatabaseManager &operator=(const DatabaseManager &) = delete;

  void initDatabase();
  QSqlDatabase m_db;
};
