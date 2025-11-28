#pragma once

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QNetworkReply>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "../api/HifiClient.hpp"
#include "../db/DatabaseManager.hpp"
#include "../net/DownloadManager.hpp"
#include "../player/AudioPlayer.hpp"
#include "FavoriteCard.hpp"
#include "TrackItemWidget.hpp"

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
  void onFavoriteToggled(const QJsonObject &track, bool isFavorite);
  void onDownloadFinished(int trackId, const QString &filePath);
  void onHomeClicked();
  void onFavoriteCardClicked(int trackId);
  void onFavoriteCoverDownloaded(QNetworkReply *reply);

private:
  HifiClient *hifiClient;
  AudioPlayer *player;

  QPushButton *homeButton;
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
  int currentTrackIdForStream = -1;

  DownloadManager *downloadManager;

  void refreshFavoritesList();

  // Page navigation
  QStackedWidget *pageStack;

  // Home page widgets
  QWidget *homePage;
  QScrollArea *favouritesScrollArea;
  QWidget *favouritesContainer;
  QGridLayout *favouritesGrid;
  QListWidget *albumsList;

  QMap<int, FavoriteCard *> favoriteCards; // Track ID -> Card widget

  // Search page widgets
  QWidget *searchPage;
};
