#pragma once

#include <QDateTime>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>
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
#include "AlbumCard.hpp"
#include "ArtistProfilePage.hpp"
#include "CreateAlbumDialog.hpp"
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
  void onAddAlbumClicked();
  void onAlbumCardClicked(int albumId);
  void onAddToAlbumClicked(const QJsonObject &track);
  void onAlbumDeleteClicked(int albumId);
  void onNextClicked();
  void onPrevClicked();
  void onArtistClicked(int artistId, const QString &artistName);

private:
  void generateAlbumCoverGrid(int albumId, AlbumCard *card);

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
  QLabel *playerTitleLabel;
  QPushButton *playerArtistLabel;

  QNetworkAccessManager *imageManager;
  bool isSeeking = false;

  QMap<int, QJsonObject> trackCache; // Cache tracks by ID
  int currentTrackIdForStream = -1;
  int currentTrackId = -1;

  // Navigation context
  QList<int> currentPlaylist; // Track IDs in current context
  int currentTrackIndex = -1;

  DownloadManager *downloadManager;

  void refreshFavoritesList();
  void refreshAlbumsList();

  // Page navigation
  QStackedWidget *pageStack;

  // Home page widgets
  QWidget *homePage;
  QScrollArea *favouritesScrollArea;
  QWidget *favouritesContainer;
  QGridLayout *favouritesGrid;
  QList<FavoriteCard *> favoriteCardsList; // Removed, replaced by below
  QWidget *albumsContainer;
  QHBoxLayout *albumsLayout;
  QPushButton *addAlbumButton;

  QMap<int, AlbumCard *> albumCards;
  // Album View
  QWidget *albumPage;
  QLabel *albumTitleLabel;
  QScrollArea *albumTracksScrollArea;
  QWidget *albumTracksContainer;
  QGridLayout *albumTracksGrid;
  int currentAlbumId = -1;
  QList<FavoriteCard *> favoriteCards; // Track ID -> Card widget
  QPushButton *backButton;

  // Search page widgets
  QWidget *searchPage;

  // Artist page
  ArtistProfilePage *artistPage;
  int currentArtistId = -1;
};
