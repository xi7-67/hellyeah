#include "HifiClient.hpp"
#include <QDebug>

HifiClient::HifiClient(QObject *parent)
    : QObject(parent), currentEndpointIndex(0) {
  manager = new QNetworkAccessManager(this);

  // Multiple API endpoints from tidal-ui config for redundancy
  apiEndpoints << "https://katze.qqdl.site"
               << "https://triton.squid.wtf"
               << "https://zeus.squid.wtf"
               << "https://phoenix.squid.wtf"
               << "https://shiva.squid.wtf"
               << "https://chaos.squid.wtf"
               << "https://hund.qqdl.site"
               << "https://wolf.qqdl.site"
               << "https://hifi.prigoana.com";
}

QString HifiClient::getCurrentBaseUrl() const {
  return apiEndpoints[currentEndpointIndex];
}

void HifiClient::rotateEndpoint() {
  currentEndpointIndex = (currentEndpointIndex + 1) % apiEndpoints.size();
  qDebug() << "Rotated to endpoint:" << getCurrentBaseUrl();
}

void HifiClient::searchTracks(const QString &query) {
  lastQuery = query;
  lastRequestType = RequestType::Search;

  QUrl url(getCurrentBaseUrl() + "/search/");
  QUrlQuery q;
  q.addQueryItem("s", query);
  url.setQuery(q);

  QNetworkRequest request(url);
  QNetworkReply *reply = manager->get(request);
  connect(reply, &QNetworkReply::finished, this, &HifiClient::onSearchFinished);
}

void HifiClient::getTrackStream(int trackId) {
  lastTrackId = trackId;
  lastRequestType = RequestType::Track;

  QUrl url(getCurrentBaseUrl() + "/track/");
  QUrlQuery q;
  q.addQueryItem("id", QString::number(trackId));
  q.addQueryItem("quality", "LOSSLESS");
  url.setQuery(q);

  QNetworkRequest request(url);
  QNetworkReply *reply = manager->get(request);
  connect(reply, &QNetworkReply::finished, this,
          &HifiClient::onTrackStreamFinished);
}

void HifiClient::getAlbum(int albumId) {
  lastAlbumId = albumId;
  lastRequestType = RequestType::Album;

  QUrl url(getCurrentBaseUrl() + "/album/");
  QUrlQuery q;
  q.addQueryItem("id", QString::number(albumId));
  url.setQuery(q);

  QNetworkRequest request(url);
  QNetworkReply *reply = manager->get(request);
  connect(reply, &QNetworkReply::finished, this, &HifiClient::onAlbumFinished);
}

void HifiClient::onSearchFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;

  if (reply->error() == QNetworkReply::NoError) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();

    QJsonArray tracks;
    if (root.contains("tracks")) {
      QJsonObject tracksObj = root["tracks"].toObject();
      if (tracksObj.contains("items")) {
        tracks = tracksObj["items"].toArray();
      }
    } else if (root.contains("items")) {
      tracks = root["items"].toArray();
    }

    emit searchResults(tracks);
  } else {
    // Try next endpoint on error
    QString errorMsg = reply->errorString();
    qDebug() << "Search failed on" << getCurrentBaseUrl() << ":" << errorMsg;
    rotateEndpoint();
    // Retry
    searchTracks(lastQuery);
  }
  reply->deleteLater();
}

void HifiClient::onTrackStreamFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;

  if (reply->error() == QNetworkReply::NoError) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    QString streamUrl;

    // Handle array response
    if (doc.isArray()) {
      QJsonArray arr = doc.array();
      for (const auto &val : arr) {
        QJsonObject obj = val.toObject();
        if (obj.contains("manifest")) {
          QString manifest = obj["manifest"].toString();
          QByteArray decoded = QByteArray::fromBase64(manifest.toUtf8());
          QJsonDocument manifestDoc = QJsonDocument::fromJson(decoded);
          if (manifestDoc.object().contains("urls")) {
            QJsonArray urls = manifestDoc.object()["urls"].toArray();
            if (!urls.isEmpty()) {
              streamUrl = urls[0].toString();
            }
          }
        }
      }
    } else {
      QJsonObject root = doc.object();
      if (root.contains("manifest")) {
        QString manifest = root["manifest"].toString();
        QByteArray decoded = QByteArray::fromBase64(manifest.toUtf8());
        QJsonDocument manifestDoc = QJsonDocument::fromJson(decoded);
        if (manifestDoc.object().contains("urls")) {
          QJsonArray urls = manifestDoc.object()["urls"].toArray();
          if (!urls.isEmpty()) {
            streamUrl = urls[0].toString();
          }
        }
      }
    }

    if (!streamUrl.isEmpty()) {
      emit trackStreamUrl(streamUrl);
    } else {
      emit errorOccurred("Could not find stream URL in response");
    }

  } else {
    QString errorMsg = reply->errorString();
    qDebug() << "Track stream failed on" << getCurrentBaseUrl() << ":"
             << errorMsg;
    rotateEndpoint();
    getTrackStream(lastTrackId);
  }
  reply->deleteLater();
}

void HifiClient::onAlbumFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;

  if (reply->error() == QNetworkReply::NoError) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    emit albumLoaded(doc.object());
  } else {
    QString errorMsg = reply->errorString();
    qDebug() << "Album failed on" << getCurrentBaseUrl() << ":" << errorMsg;
    rotateEndpoint();
    getAlbum(lastAlbumId);
  }
  reply->deleteLater();
}
