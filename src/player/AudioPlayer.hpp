#pragma once

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QTemporaryFile>
#include <QTimer>

// Forward declaration for miniaudio types to avoid including the header here
struct ma_engine;
struct ma_sound;

class AudioPlayer : public QObject {
  Q_OBJECT

public:
  explicit AudioPlayer(QObject *parent = nullptr);
  ~AudioPlayer();

  void playUrl(const QString &url);
  void play();
  void pause();
  void stop();
  void setVolume(float volume); // 0.0 to 1.0
  void seek(qint64 positionMs);
  qint64 position() const;
  qint64 duration() const;
  bool isPlaying() const;

signals:
  void positionChanged(qint64 position);
  void durationChanged(qint64 duration);
  void errorOccurred(const QString &message);

private slots:
  void onDownloadFinished();
  void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
  void onPositionTimer();

private:
  QNetworkAccessManager *manager;
  QNetworkReply *currentReply;
  QTemporaryFile *tempFile;
  QTimer *positionTimer;

  // Miniaudio state
  ma_engine *engine;
  ma_sound *sound;
  bool isEngineInitialized;
  bool isSoundInitialized;
  float m_volume; // Store volume

  void initMiniaudio();
  void cleanupMiniaudio();
  void startPlayback(const QString &filePath);
};
