#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QUrlQuery>

class HifiClient : public QObject {
  Q_OBJECT

public:
  explicit HifiClient(QObject *parent = nullptr);

  void searchTracks(const QString &query);
  void getTrackStream(int trackId);
  void getAlbum(int albumId);

signals:
  void searchResults(const QJsonArray &tracks);
  void trackStreamUrl(const QString &url);
  void albumLoaded(const QJsonObject &albumData);
  void errorOccurred(const QString &message);

private slots:
  void onSearchFinished();
  void onTrackStreamFinished();
  void onAlbumFinished();

private:
  QNetworkAccessManager *manager;
  QStringList apiEndpoints;
  int currentEndpointIndex;

  QString getCurrentBaseUrl() const;
  void rotateEndpoint();

  // State for retries
  QString lastQuery;
  int lastTrackId = -1;
  int lastAlbumId = -1;
  enum class RequestType { None, Search, Track, Album };
  RequestType lastRequestType = RequestType::None;
};
