#pragma once

#include <QFile>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class DownloadManager : public QObject {
  Q_OBJECT

public:
  explicit DownloadManager(QObject *parent = nullptr);
  void downloadTrack(const QString &url, int trackId);

signals:
  void downloadFinished(int trackId, const QString &filePath);

private slots:
  void onDownloadFinished(QNetworkReply *reply);

private:
  QNetworkAccessManager *manager;
  QMap<QNetworkReply *, int> activeDownloads;
};
