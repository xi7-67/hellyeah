#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QNetworkReply>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>

#include "../api/HifiClient.hpp"
#include "../player/AudioPlayer.hpp"

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private slots:
  void onSearchClicked();
  void onTrackSelected(QListWidgetItem *item);
  void onTrackStreamUrl(const QString &url);
  void onPlayerError(const QString &message);
  void onApiError(const QString &message);
  void onPlayPauseClicked();
  void onAlbumLoaded(const QJsonObject &album);
  void onVolumeChanged(int volume);
  void onSeekSliderMoved(int position);
  void onSeekSliderPressed();
  void onSeekSliderReleased();
  void onPositionChanged(qint64 position);
  void onDurationChanged(qint64 duration);
  void onCoverImageDownloaded(QNetworkReply *reply);

private:
  HifiClient *hifiClient;
  AudioPlayer *player;

  QLineEdit *searchBox;
  QPushButton *searchButton;
  QListWidget *resultsList;

  QPushButton *playPauseButton;
  QSlider *volumeSlider;
  QSlider *seekSlider;
  QLabel *statusLabel;
  QLabel *coverLabel;

  QNetworkAccessManager *imageManager;
  bool isSeeking = false;

  QMap<int, QJsonObject> trackCache; // Cache tracks by ID
};
