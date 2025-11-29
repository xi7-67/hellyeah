#include "DownloadManager.hpp"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

DownloadManager::DownloadManager(QObject *parent) : QObject(parent) {
  manager = new QNetworkAccessManager(this);
  connect(manager, &QNetworkAccessManager::finished, this,
          &DownloadManager::onDownloadFinished);
}

void DownloadManager::downloadTrack(const QString &url, int trackId) {
  QUrl qurl(url);
  QNetworkRequest request(qurl);
  QNetworkReply *reply = manager->get(request);
  activeDownloads.insert(reply, trackId);
}

void DownloadManager::downloadCover(const QString &url, int trackId) {
  QUrl qurl(url);
  QNetworkRequest request(qurl);
  QNetworkReply *reply = manager->get(request);
  activeCoverDownloads.insert(reply, trackId);
}

void DownloadManager::onDownloadFinished(QNetworkReply *reply) {
  if (activeDownloads.contains(reply)) {
    int trackId = activeDownloads.take(reply);

    if (reply->error() == QNetworkReply::NoError) {
      QString dataPath =
          QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
      QDir dir(dataPath);
      if (!dir.exists("downloads")) {
        dir.mkpath("downloads");
      }

      QString filePath = dir.filePath("downloads/" + QString::number(trackId) +
                                      ".flac"); // Assuming mp3/m4a
      QFile file(filePath);
      if (file.open(QIODevice::WriteOnly)) {
        file.write(reply->readAll());
        file.close();
        emit downloadFinished(trackId, filePath);
        qDebug() << "Download finished for track" << trackId << "at"
                 << filePath;
      } else {
        qDebug() << "Failed to save file for track" << trackId;
      }
    } else {
      qDebug() << "Download error for track" << trackId << ":"
               << reply->errorString();
    }
    reply->deleteLater();
  } else if (activeCoverDownloads.contains(reply)) {
    int trackId = activeCoverDownloads.take(reply);

    if (reply->error() == QNetworkReply::NoError) {
      QString dataPath =
          QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
      QDir dir(dataPath);
      if (!dir.exists("covers")) {
        dir.mkpath("covers");
      }

      QString filePath =
          dir.filePath("covers/" + QString::number(trackId) + ".jpg");
      QFile file(filePath);
      if (file.open(QIODevice::WriteOnly)) {
        file.write(reply->readAll());
        file.close();
        emit coverDownloadFinished(trackId, filePath);
        qDebug() << "Cover download finished for track" << trackId << "at"
                 << filePath;
      } else {
        qDebug() << "Failed to save cover file for track" << trackId;
      }
    } else {
      qDebug() << "Cover download error for track" << trackId << ":"
               << reply->errorString();
    }
    reply->deleteLater();
  }
}
