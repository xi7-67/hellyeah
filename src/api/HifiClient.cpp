#include "HifiClient.hpp"
#include <QDebug>

HifiClient::HifiClient(QObject *parent)
    : QObject(parent), currentEndpointIndex(0) {
  manager = new QNetworkAccessManager(this);

  // Multiple API endpoints from tidal-ui config for redundancy
  apiEndpoints << "https://wolf.qqdl.site"
               << "https://frankfurt.monochrome.tf"
               << "https://tidal-api-2.binimum.org"
               << "https://katze.qqdl.site"
               << "https://triton.squid.wtf"
               << "https://zeus.squid.wtf"
               << "https://phoenix.squid.wtf"
               << "https://shiva.squid.wtf"
               << "https://chaos.squid.wtf"
               << "https://hund.qqdl.site"
               << "https://vogel.qqdl.site"
               << "https://maus.qqdl.site"
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

  cancelPendingSearch();

  // Try top 3 endpoints in parallel
  int count = 0;
  for (int i = 0; i < apiEndpoints.size() && count < 3; ++i) {
    int idx = (currentEndpointIndex + i) % apiEndpoints.size();
    QString baseUrl = apiEndpoints[idx];

    QUrl url(baseUrl + "/search/");
    QUrlQuery q;
    q.addQueryItem("s", query);
    url.setQuery(q);

    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
    pendingSearchReplies.append(reply);

    connect(reply, &QNetworkReply::finished, this,
            &HifiClient::onSearchFinished);
    count++;
  }
}

void HifiClient::getTrackStream(int trackId) {
  lastTrackId = trackId;
  lastRequestType = RequestType::Track;

  cancelPendingTrack();

  // Try top 3 endpoints in parallel
  int count = 0;
  for (int i = 0; i < apiEndpoints.size() && count < 3; ++i) {
    int idx = (currentEndpointIndex + i) % apiEndpoints.size();
    QString baseUrl = apiEndpoints[idx];

    QUrl url(baseUrl + "/track/");
    QUrlQuery q;
    q.addQueryItem("id", QString::number(trackId));
    q.addQueryItem("quality", "LOSSLESS");
    url.setQuery(q);

    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
    pendingTrackReplies.append(reply);

    connect(reply, &QNetworkReply::finished, this,
            &HifiClient::onTrackStreamFinished);
    count++;
  }
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

    // If we got valid results, cancel other pending requests and emit
    if (!tracks.isEmpty()) {
      cancelPendingSearch();
      emit searchResults(tracks);
    } else {
      // Empty results, treat as error/next? No, just wait for others or emit
      // empty if all done. For now, if one returns empty, maybe others will
      // return something? Let's remove this reply from pending and check if any
      // left.
      pendingSearchReplies.removeAll(reply);
      reply->deleteLater();

      if (pendingSearchReplies.isEmpty()) {
        emit searchResults(QJsonArray());
      }
    }
  } else {
    // Error
    pendingSearchReplies.removeAll(reply);
    reply->deleteLater();

    if (pendingSearchReplies.isEmpty()) {
      // All failed
      rotateEndpoint(); // Move to next set?
      // emit errorOccurred("All search endpoints failed");
      // Or just emit empty
      emit searchResults(QJsonArray());
    }
  }
}

void HifiClient::cancelPendingSearch() {
  for (QNetworkReply *reply : pendingSearchReplies) {
    if (reply->isRunning()) {
      reply->abort();
    }
    reply->deleteLater();
  }
  pendingSearchReplies.clear();
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
      cancelPendingTrack();
      emit trackStreamUrl(streamUrl);
    } else {
      // Failed to parse stream URL, treat as error for this reply
      pendingTrackReplies.removeAll(reply);
      reply->deleteLater();

      if (pendingTrackReplies.isEmpty()) {
        emit errorOccurred("Could not find stream URL in response");
      }
    }

  } else {
    pendingTrackReplies.removeAll(reply);
    reply->deleteLater();

    if (pendingTrackReplies.isEmpty()) {
      QString errorMsg = reply->errorString();
      qDebug() << "Track stream failed on all tried endpoints:" << errorMsg;
      rotateEndpoint();
      // Retry? Or just fail. Let's fail to avoid infinite loops if all down.
      // getTrackStream(lastTrackId);
      emit errorOccurred("Track stream failed: " + errorMsg);
    }
  }
}

void HifiClient::cancelPendingTrack() {
  for (QNetworkReply *reply : pendingTrackReplies) {
    if (reply->isRunning()) {
      reply->abort();
    }
    reply->deleteLater();
  }
  pendingTrackReplies.clear();
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
