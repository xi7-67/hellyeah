#include "MainWindow.hpp"
#include <QDebug>
#include <QKeySequence>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPainter>
#include <QPixmap>
#include <QShortcut>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), hifiClient(new HifiClient(this)),
      player(new AudioPlayer(this)),
      imageManager(new QNetworkAccessManager(this)),
      downloadManager(new DownloadManager(this)) {

  connect(downloadManager, &DownloadManager::downloadFinished, this,
          &MainWindow::onDownloadFinished);
  connect(downloadManager, &DownloadManager::coverDownloadFinished, this,
          [this](int trackId, const QString &coverPath) {
            DatabaseManager::instance().updateCoverPath(trackId, coverPath);
            // Refresh card if it exists
            for (auto *card : favoriteCards) {
              if (card->getTrackId() == trackId) {
                QPixmap pixmap(coverPath);
                if (!pixmap.isNull()) {
                  card->setCoverImage(pixmap);
                }
                break;
              }
            }
          });

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  // Main layout
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

  // Search bar with home button
  QHBoxLayout *searchLayout = new QHBoxLayout();
  homeButton = new QPushButton("⌂", this);
  homeButton->setFixedSize(40, 30);
  homeButton->setStyleSheet("font-size: 20px; font-weight: bold;");
  homeButton->setToolTip("Home");
  searchLayout->addWidget(homeButton);
  searchLayout->addStretch();
  searchBox = new QLineEdit();
  searchBox->setPlaceholderText("Search for tracks...");
  searchBox->setMinimumWidth(400);
  searchButton = new QPushButton("Search");
  searchLayout->addWidget(searchBox);
  searchLayout->addWidget(searchButton);
  searchLayout->addStretch();

  // Content area (stack to toggle between home and search)
  pageStack = new QStackedWidget();

  // HOME VIEW
  homePage = new QWidget();
  QVBoxLayout *homeLayout = new QVBoxLayout(homePage);

  QLabel *favouritesLabel = new QLabel("Favourites");
  favouritesLabel->setStyleSheet(
      "font-size: 20px; font-weight: bold; margin-top: 10px; color: #fff;");

  // Horizontal scroll layout for favorites (like albums)
  favouritesScrollArea = new QScrollArea();
  favouritesScrollArea->setWidgetResizable(true);
  favouritesScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  favouritesScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  // No height limit - show all tracks
  favouritesScrollArea->setStyleSheet(
      "QScrollArea { background-color: #121212; border: none; }"
      "QScrollBar:horizontal { height: 8px; background: #282828; }"
      "QScrollBar::handle:horizontal { background: #535353; border-radius: "
      "4px; }"
      "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { "
      "border: none; background: none; }");

  favouritesContainer = new QWidget();
  favouritesGrid = new QGridLayout(favouritesContainer);
  favouritesGrid->setSpacing(8);
  favouritesGrid->setContentsMargins(10, 10, 10, 10);
  // More columns for smaller cards
  for (int i = 0; i < 6; i++) {
    favouritesGrid->setColumnStretch(i, 1);
  }
  favouritesContainer->setStyleSheet("background-color: #121212;");

  favouritesScrollArea->setWidget(favouritesContainer);

  refreshFavoritesList();

  homeLayout->addWidget(favouritesLabel);
  homeLayout->addWidget(favouritesScrollArea);

  // Playlist section
  QHBoxLayout *albumsHeaderLayout = new QHBoxLayout();
  QLabel *albumsLabel = new QLabel("Playlists");
  albumsLabel->setStyleSheet(
      "font-size: 20px; font-weight: bold; color: #fff;");

  addAlbumButton = new QPushButton("+");
  addAlbumButton->setFixedSize(30, 30);
  addAlbumButton->setCursor(Qt::PointingHandCursor);
  addAlbumButton->setStyleSheet(
      "QPushButton { background-color: transparent; color: #b3b3b3; font-size: "
      "24px; border: none; }"
      "QPushButton:hover { color: #fff; }");
  connect(addAlbumButton, &QPushButton::clicked, this,
          &MainWindow::onAddAlbumClicked);

  albumsHeaderLayout->addWidget(albumsLabel);
  albumsHeaderLayout->addWidget(addAlbumButton);
  albumsHeaderLayout->addStretch();

  homeLayout->addLayout(albumsHeaderLayout);

  // Albums Container - Horizontal Scroll
  QScrollArea *albumsScrollArea = new QScrollArea();
  albumsScrollArea->setWidgetResizable(true);
  albumsScrollArea->setStyleSheet(
      "background-color: transparent; border: none;");
  albumsScrollArea->setFixedHeight(100); // Height for cards + scrollbar

  albumsContainer = new QWidget();
  albumsLayout =
      new QHBoxLayout(albumsContainer); // Re-assign to member variable
  albumsLayout->setSpacing(10);
  albumsLayout->setContentsMargins(0, 0, 0, 0);
  albumsLayout->setAlignment(Qt::AlignLeft);

  albumsScrollArea->setWidget(albumsContainer);
  homeLayout->addWidget(albumsScrollArea);

  refreshAlbumsList();

  // SEARCH RESULTS VIEW
  searchPage = new QWidget();
  QVBoxLayout *searchResultsLayout = new QVBoxLayout(
      searchPage); // Renamed from searchLayout to avoid conflict
  resultsList = new QListWidget();
  searchResultsLayout->addWidget(resultsList);

  // ALBUM VIEW
  albumPage = new QWidget();
  QVBoxLayout *albumViewLayout = new QVBoxLayout(albumPage);

  // Header for Album View
  QWidget *albumHeader = new QWidget();
  QHBoxLayout *albumHeaderLayout = new QHBoxLayout(albumHeader);

  backButton = new QPushButton("← Back");
  backButton->setCursor(Qt::PointingHandCursor);
  backButton->setStyleSheet(
      "color: #fff; font-weight: bold; border: none; font-size: 16px;");
  connect(backButton, &QPushButton::clicked, this, &MainWindow::onHomeClicked);

  albumTitleLabel = new QLabel("Album Title");
  albumTitleLabel->setStyleSheet(
      "color: #fff; font-size: 24px; font-weight: bold;");

  albumHeaderLayout->addWidget(backButton);
  albumHeaderLayout->addSpacing(20);
  albumHeaderLayout->addWidget(albumTitleLabel);
  albumHeaderLayout->addStretch();

  albumViewLayout->addWidget(albumHeader);

  albumTracksScrollArea = new QScrollArea();
  albumTracksScrollArea->setWidgetResizable(true);
  albumTracksScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  albumTracksScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  albumTracksScrollArea->setStyleSheet(
      "QScrollArea { background-color: #121212; border: none; }"
      "QScrollBar:horizontal { height: 8px; background: #282828; }"
      "QScrollBar::handle:horizontal { background: #535353; border-radius: "
      "4px; }"
      "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { "
      "border: none; background: none; }");

  albumTracksContainer = new QWidget();
  albumTracksGrid = new QGridLayout(albumTracksContainer);
  albumTracksGrid->setSpacing(8);
  albumTracksGrid->setContentsMargins(10, 10, 10, 10);
  // More columns for smaller cards
  for (int i = 0; i < 6; i++) {
    albumTracksGrid->setColumnStretch(i, 1);
  }
  albumTracksContainer->setStyleSheet("background-color: #121212;");

  albumTracksScrollArea->setWidget(albumTracksContainer);
  albumViewLayout->addWidget(albumTracksScrollArea);

  pageStack->addWidget(homePage);   // index 0 - Home
  pageStack->addWidget(searchPage); // index 1 - Search Results
  pageStack->addWidget(albumPage);  // index 2 - Album View
  pageStack->setCurrentIndex(0);    // Start with Home
  // Shared UI (Cover + Controls)
  coverLabel = new QLabel(this);
  coverLabel->setFixedSize(200, 200);
  coverLabel->setStyleSheet("background-color: #333; border: 1px solid #555;");
  coverLabel->setAlignment(Qt::AlignCenter);
  // Don't set text or show - start hidden
  coverLabel->hide(); // Hide by default

  playerTitleLabel = new QLabel("", this);
  playerTitleLabel->setStyleSheet(
      "font-weight: bold; font-size: 14px; color: #fff; margin-top: 10px;");
  playerTitleLabel->setAlignment(Qt::AlignCenter);
  playerTitleLabel->setWordWrap(true);
  playerTitleLabel->setMaximumWidth(200);
  playerTitleLabel->hide();

  playerArtistLabel = new QLabel("", this);
  playerArtistLabel->setStyleSheet("font-size: 12px; color: #b3b3b3;");
  playerArtistLabel->setAlignment(Qt::AlignCenter);
  playerArtistLabel->setWordWrap(true);
  playerArtistLabel->setMaximumWidth(200);
  playerArtistLabel->hide();

  // Play/Pause Button
  playPauseButton = new QPushButton(this);
  playPauseButton->setFixedSize(40, 40);
  playPauseButton->setCursor(Qt::PointingHandCursor);
  playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  playPauseButton->setIconSize(QSize(24, 24));
  playPauseButton->setStyleSheet("QPushButton { "
                                 "  background-color: #282828; "
                                 "  border: 2px solid #535353; "
                                 "  border-radius: 20px; "
                                 "}"
                                 "QPushButton:hover { "
                                 "  background-color: #333; "
                                 "  border-color: #888; "
                                 "}");

  // Previous Button
  QPushButton *prevButton = new QPushButton(this);
  prevButton->setFixedSize(28, 28);
  prevButton->setCursor(Qt::PointingHandCursor);
  prevButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
  prevButton->setIconSize(QSize(20, 20));
  prevButton->setStyleSheet("QPushButton { "
                            "  background-color: #282828; "
                            "  border: none; "
                            "  border-radius: 14px; "
                            "}"
                            "QPushButton:hover { "
                            "  background-color: #333; "
                            "}");

  // Next Button
  QPushButton *nextButton = new QPushButton(this);
  nextButton->setFixedSize(28, 28);
  nextButton->setCursor(Qt::PointingHandCursor);
  nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
  nextButton->setIconSize(QSize(20, 20));
  nextButton->setStyleSheet("QPushButton { "
                            "  background-color: #282828; "
                            "  border: none; "
                            "  border-radius: 14px; "
                            "}"
                            "QPushButton:hover { "
                            "  background-color: #333; "
                            "}");

  // Common slider stylesheet - for seek slider
  // Common slider stylesheet - for seek slider
  QString seekSliderStyle = "QSlider::groove:horizontal { "
                            "  border: 1px solid #333; "
                            "  height: 4px; "
                            "  background: #333; "
                            "  margin: 2px 0; "
                            "  border-radius: 2px; "
                            "}"
                            "QSlider::handle:horizontal { "
                            "  background: #b3b3b3; "
                            "  border: 1px solid #888; "
                            "  width: 16px; "
                            "  height: 16px; "
                            "  margin: -6px 0; " // (16-4)/2 = 6
                            "  border-radius: 8px; "
                            "}"
                            "QSlider::handle:horizontal:hover { "
                            "  background: #fff; "
                            "}"
                            "QSlider::sub-page:horizontal { "
                            "  background: #b3b3b3; "
                            "  border-radius: 2px; "
                            "}";

  // Volume slider stylesheet - smaller handle and groove
  // Volume slider stylesheet - smaller handle and groove
  QString volumeSliderStyle = "QSlider::groove:horizontal { "
                              "  border: 1px solid #333; "
                              "  height: 2px; "
                              "  background: #333; "
                              "  margin: 2px 0; "
                              "  border-radius: 1px; "
                              "}"
                              "QSlider::handle:horizontal { "
                              "  background: #b3b3b3; "
                              "  border: 1px solid #888; "
                              "  width: 12px; "
                              "  height: 12px; "
                              "  margin: -5px 0; " // (12-2)/2 = 5
                              "  border-radius: 6px; "
                              "}"
                              "QSlider::handle:horizontal:hover { "
                              "  background: #fff; "
                              "}"
                              "QSlider::sub-page:horizontal { "
                              "  background: #b3b3b3; "
                              "  border-radius: 1px; "
                              "}";

  volumeSlider = new QSlider(Qt::Horizontal, this);
  volumeSlider->setRange(0, 100);
  volumeSlider->setValue(45); // Default to 45%
  volumeSlider->setStyleSheet(volumeSliderStyle);
  player->setVolume(0.45f); // Set initial volume

  seekSlider = new QSlider(Qt::Horizontal, this);
  seekSlider->setRange(0, 0);
  seekSlider->setStyleSheet(seekSliderStyle);

  statusLabel = new QLabel("Ready", this);

  // Content layout (page stack + cover)
  // Content layout (page stack + cover area)
  QHBoxLayout *contentLayout = new QHBoxLayout();
  contentLayout->addWidget(pageStack, 1);

  // Right side player info layout
  QVBoxLayout *playerInfoLayout = new QVBoxLayout();
  playerInfoLayout->setAlignment(Qt::AlignCenter);    // Center vertically
  playerInfoLayout->setContentsMargins(20, 0, 20, 0); // Horizontal margins only
  playerInfoLayout->addWidget(coverLabel);
  playerInfoLayout->addWidget(playerTitleLabel);
  playerInfoLayout->addWidget(playerArtistLabel);

  contentLayout->addLayout(playerInfoLayout);

  // Controls layout
  QVBoxLayout *playerControlsLayout = new QVBoxLayout();
  playerControlsLayout->setSpacing(5);

  // Top row: Buttons and Volume
  QHBoxLayout *buttonsLayout = new QHBoxLayout();

  // Center the playback buttons
  buttonsLayout->addStretch(1);
  buttonsLayout->addWidget(prevButton);
  buttonsLayout->addSpacing(15);
  buttonsLayout->addWidget(playPauseButton);
  buttonsLayout->addSpacing(15);
  buttonsLayout->addWidget(nextButton);
  buttonsLayout->addStretch(1);

  // Volume on the right
  QLabel *volumeIcon = new QLabel(this);
  volumeIcon->setPixmap(
      style()->standardIcon(QStyle::SP_MediaVolume).pixmap(16, 16));
  buttonsLayout->addWidget(volumeIcon);
  buttonsLayout->addWidget(volumeSlider);
  // volumeSlider is already set to fixed width or ratio?
  // It was set to ratio 1 in previous layout. Here it's just added.
  // Let's set a fixed width for volume slider to look neat
  volumeSlider->setFixedWidth(100);

  playerControlsLayout->addLayout(buttonsLayout);
  playerControlsLayout->addWidget(
      seekSlider); // Seek bar at the bottom full width

  // Assemble main layout
  mainLayout->addLayout(searchLayout);
  mainLayout->addLayout(homeLayout);
  mainLayout->addLayout(contentLayout);
  mainLayout->addLayout(playerControlsLayout); // Use new layout
  mainLayout->addWidget(statusLabel);

  // Logic
  // client = new HifiClient(this); // Removed, now in initializer list
  // player = new AudioPlayer(this); // Removed, now in initializer list

  // Connections
  connect(homeButton, &QPushButton::clicked, this, &MainWindow::onHomeClicked);
  connect(searchButton, &QPushButton::clicked, this,
          &MainWindow::onSearchClicked);
  connect(searchBox, &QLineEdit::returnPressed, this,
          &MainWindow::onSearchClicked);
  connect(playPauseButton, &QPushButton::clicked, this,
          &MainWindow::onPlayPauseClicked);
  connect(prevButton, &QPushButton::clicked, this, &MainWindow::onPrevClicked);
  connect(nextButton, &QPushButton::clicked, this, &MainWindow::onNextClicked);

  connect(hifiClient, &HifiClient::searchResults, this,
          [this](const QJsonArray &tracks) {
            resultsList->clear();
            trackCache.clear();

            // Switch to search results page
            pageStack->setCurrentIndex(1);

            if (!tracks.isEmpty()) {
              QJsonObject first = tracks[0].toObject();
              QString title = first["title"].toString();
              QString artist = first["artist"].toObject()["name"].toString();
              statusLabel->setText("Found: " + title + " by " + artist);
            } else {
              statusLabel->setText("No results found.");
            }

            for (const auto &val : tracks) {
              QJsonObject track = val.toObject();
              int id = track["id"].toInt();
              trackCache.insert(id, track);

              QListWidgetItem *item = new QListWidgetItem(resultsList);
              item->setSizeHint(QSize(0, 50)); // Set height for custom widget
              item->setData(Qt::UserRole, id); // Store ID for playback

              TrackItemWidget *widget = new TrackItemWidget(
                  track, DatabaseManager::instance().isFavorite(id));
              connect(widget, &TrackItemWidget::favoriteToggled, this,
                      [this, track](bool isFav) {
                        onFavoriteToggled(track, isFav);
                      });
              connect(widget, &TrackItemWidget::addToAlbumClicked, this,
                      [this, track]() { onAddToAlbumClicked(track); });

              resultsList->setItemWidget(item, widget);
            }
          });

  connect(hifiClient, &HifiClient::trackStreamUrl,
          this, // Changed client to hifiClient
          &MainWindow::onTrackStreamUrl);
  connect(hifiClient, &HifiClient::errorOccurred, this,
          &MainWindow::onApiError); // Changed client to hifiClient

  connect(resultsList, &QListWidget::itemDoubleClicked, this,
          &MainWindow::onTrackSelected);

  connect(player, &AudioPlayer::positionChanged, this,
          &MainWindow::onPositionChanged);
  connect(player, &AudioPlayer::durationChanged, this,
          &MainWindow::onDurationChanged);
  connect(player, &AudioPlayer::errorOccurred, this,
          [this](const QString &msg) {
            statusLabel->setText("Player Error: " + msg);
          });

  connect(seekSlider, &QSlider::sliderMoved, this,
          &MainWindow::onSeekSliderMoved);
  connect(seekSlider, &QSlider::sliderPressed, this,
          &MainWindow::onSeekSliderPressed);
  connect(seekSlider, &QSlider::sliderReleased, this,
          &MainWindow::onSeekSliderReleased);

  connect(imageManager, &QNetworkAccessManager::finished, this,
          &MainWindow::onCoverImageDownloaded);

  connect(volumeSlider, &QSlider::valueChanged,
          this, // Changed lambda to method call
          &MainWindow::onVolumeChanged);

  // Keyboard Shortcuts
  // Spacebar to toggle Play/Pause
  QShortcut *spaceShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
  connect(spaceShortcut, &QShortcut::activated, this,
          &MainWindow::onPlayPauseClicked);

  // Media Keys
  QShortcut *mediaPlayShortcut =
      new QShortcut(QKeySequence(Qt::Key_MediaPlay), this);
  connect(mediaPlayShortcut, &QShortcut::activated, this,
          &MainWindow::onPlayPauseClicked);

  QShortcut *mediaPauseShortcut =
      new QShortcut(QKeySequence(Qt::Key_MediaPause), this);
  connect(mediaPauseShortcut, &QShortcut::activated, this,
          &MainWindow::onPlayPauseClicked);

  QShortcut *mediaToggleShortcut =
      new QShortcut(QKeySequence(Qt::Key_MediaTogglePlayPause), this);
  connect(mediaToggleShortcut, &QShortcut::activated, this,
          &MainWindow::onPlayPauseClicked);

  resize(800, 600);
}

void MainWindow::onSearchClicked() {
  QString query = searchBox->text();
  if (query.isEmpty()) {
    pageStack->setCurrentIndex(0); // Go back to Home
    statusLabel->setText("Ready");
    return;
  }

  statusLabel->setText("Searching...");
  hifiClient->searchTracks(query);
}

void MainWindow::onTrackSelected(QListWidgetItem *item) {
  int trackId = item->data(Qt::UserRole).toInt();

  if (trackCache.contains(trackId)) {
    QJsonObject track = trackCache[trackId];
    QString title = track["title"].toString();
    QString artist = track["artist"].toObject()["name"].toString();

    statusLabel->setText("Fetching stream for: " + title + " by " + artist);

    // Update player info
    playerTitleLabel->setText(title);
    playerArtistLabel->setText(artist);
    playerTitleLabel->show();
    playerArtistLabel->show();

    // Fetch cover for the selected track
    if (track.contains("album")) {
      QJsonObject album = track["album"].toObject();
      if (album.contains("cover")) {
        QString coverId = album["cover"].toString();
        if (!coverId.isEmpty()) {
          QString coverUrl =
              QString("https://resources.tidal.com/images/%1/640x640.jpg")
                  .arg(coverId.replace("-", "/"));
          imageManager->get(QNetworkRequest(QUrl(coverUrl)));
        } else {
          qDebug() << "Track" << trackId << "album has empty cover ID.";
          coverLabel->hide();
        }
      } else {
        qDebug() << "Track" << trackId << "album has no 'cover' field.";
        coverLabel->hide();
      }
    } else {
      qDebug() << "Track" << trackId << "has no 'album' field.";
      coverLabel->hide();
    }
  } else {
    statusLabel->setText("Error: Track not found in cache.");
    coverLabel->hide();
  }

  // Update playlist context for search results
  currentPlaylist.clear();
  currentTrackIndex = -1;
  for (int i = 0; i < resultsList->count(); ++i) {
    int id = resultsList->item(i)->data(Qt::UserRole).toInt();
    currentPlaylist.append(id);
    if (id == trackId) {
      currentTrackIndex = i;
    }
  }

  // Check if we have a local file for this track
  QString localPath = DatabaseManager::instance().getFilePath(trackId);
  if (!localPath.isEmpty() && QFile::exists(localPath)) {
    statusLabel->setText("Playing from local cache...");
    player->playUrl(QUrl::fromLocalFile(localPath).toString());
    playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    // Show cover
    if (!coverLabel->pixmap().isNull()) {
      coverLabel->setVisible(true);
    }
  } else {
    hifiClient->getTrackStream(trackId);
  }
}

void MainWindow::onAddAlbumClicked() {
  CreateAlbumDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    QString name = dialog.getAlbumName();
    QString coverPath = dialog.getCoverPath();

    if (!name.isEmpty()) {
      int albumId = DatabaseManager::instance().createAlbum(name, coverPath);
      if (albumId != -1) {
        statusLabel->setText("Album created: " + name);
        refreshAlbumsList();
      } else {
        statusLabel->setText("Failed to create album");
      }
    }
  }
}

void MainWindow::refreshAlbumsList() {
  // Clear existing cards
  QLayoutItem *child;
  while ((child = albumsContainer->layout()->takeAt(0)) != nullptr) {
    delete child->widget();
    delete child;
  }
  albumCards.clear();

  QList<QJsonObject> albums = DatabaseManager::instance().getAlbums();
  for (const QJsonObject &album : albums) {
    AlbumCard *card = new AlbumCard(album);
    // Ensure card has a reasonable width for horizontal layout
    card->setFixedWidth(200);
    connect(card, &AlbumCard::clicked, this, &MainWindow::onAlbumCardClicked);
    connect(card, &AlbumCard::deleteClicked, this,
            &MainWindow::onAlbumDeleteClicked);
    albumsContainer->layout()->addWidget(card);
    albumCards.insert(album["id"].toInt(), card);

    // Check if album has a cover path
    if (album.contains("coverPath") &&
        !album["coverPath"].toString().isEmpty()) {
      QPixmap pixmap(album["coverPath"].toString());
      if (!pixmap.isNull()) {
        card->setCoverImage(pixmap);
      }
    } else {
      // Generate composite cover from track covers
      generateAlbumCoverGrid(album["id"].toInt(), card);
    }
  }
}

void MainWindow::onAlbumCardClicked(int albumId) {
  currentAlbumId = albumId;

  // Find the album name
  QString albumName = "Unknown Album";
  if (albumCards.contains(albumId)) {
    albumName = albumCards[albumId]->getAlbumData()["title"].toString();
  }
  albumTitleLabel->setText(albumName);

  // Clear existing cards
  QLayoutItem *child;
  while ((child = albumTracksGrid->layout()->takeAt(0)) != nullptr) {
    delete child->widget();
    delete child;
  }
  favoriteCards
      .clear(); // Re-using this list for tracking cards in album view too?
                // Actually, favoriteCards is used for the home view updates.
                // We should probably have a separate list or just not track
                // them globally if we don't need to update them from outside.
                // But wait, if we play a track, we might want to update UI.
                // For now, let's just create them and relying on signals.

  QList<QJsonObject> tracks =
      DatabaseManager::instance().getAlbumTracks(albumId);

  if (tracks.isEmpty()) {
    // Show empty state?
    QLabel *emptyLabel =
        new QLabel("No tracks in this album yet.", albumTracksContainer);
    emptyLabel->setStyleSheet(
        "color: #b3b3b3; font-size: 16px; margin-top: 20px;");
    emptyLabel->setAlignment(Qt::AlignCenter);
    albumTracksGrid->addWidget(emptyLabel, 0, 0, 1, 6, Qt::AlignCenter);
  } else {
    int row = 0;
    int col = 0;
    int maxCols = 6; // Responsive? Fixed for now.

    for (const QJsonObject &track : tracks) {
      int trackId = track["id"].toInt();
      trackCache.insert(trackId, track); // Add to cache for playback

      // Use FavoriteCard for consistency
      FavoriteCard *card = new FavoriteCard(track, albumTracksContainer);

      // Check actual favorite status
      bool isFav = DatabaseManager::instance().isFavorite(trackId);
      card->setFavorite(isFav);

      // Connect signals
      // Connect signals
      // Capture the full list of IDs for this album context
      QList<int> albumTrackIds;
      for (const auto &t : tracks)
        albumTrackIds.append(t["id"].toInt());

      connect(card, &FavoriteCard::clicked, this,
              [this, trackId, albumTrackIds]() {
                currentPlaylist = albumTrackIds;
                currentTrackIndex = currentPlaylist.indexOf(trackId);
                onFavoriteCardClicked(trackId);
              });
      connect(card, &FavoriteCard::addToAlbumClicked, this,
              &MainWindow::onAddToAlbumClicked);
      connect(card, &FavoriteCard::unfavoriteClicked, this,
              [this, track](int id) {
                // If unfavorited from album view, what happens?
                // It removes from favorites. Should it remove from album?
                // Probably yes, since album tracks are favorites.
                // But for now let's just toggle the star.
                onFavoriteToggled(track, false);
              });
      connect(card, &FavoriteCard::favoriteToggled, this,
              [this](const QJsonObject &t, bool isFav) {
                onFavoriteToggled(t, isFav);
              });

      // Check if we have a cover image for this track
      QString coverPath = DatabaseManager::instance().getCoverPath(trackId);
      if (!coverPath.isEmpty()) {
        QPixmap pixmap(coverPath);
        if (!pixmap.isNull()) {
          card->setCoverImage(pixmap);
        }
      } else {
        // Trigger download if needed?
        // MainWindow::onFavoriteCardClicked handles playback, but cover loading
        // is usually done via refreshFavoritesList logic or on-demand. Let's
        // rely on the existing logic or maybe trigger a check.
      }

      albumTracksGrid->addWidget(card, row, col);

      col++;
      if (col >= maxCols) {
        col = 0;
        row++;
      }
    }
  }

  pageStack->setCurrentIndex(2); // Album View
  statusLabel->setText("Viewing album: " + albumName);
}

void MainWindow::onAddToAlbumClicked(const QJsonObject &track) {
  // Get list of albums
  QList<QJsonObject> albums = DatabaseManager::instance().getAlbums();

  if (albums.isEmpty()) {
    statusLabel->setText("No albums found. Create an album first!");
    return;
  }

  // Create list of album names for the dialog
  QStringList albumNames;
  QList<int> albumIds;
  for (const QJsonObject &album : albums) {
    albumNames.append(album["title"].toString());
    albumIds.append(album["id"].toInt());
  }

  // Show selection dialog
  bool ok;
  QString selected = QInputDialog::getItem(
      this, tr("Add to Album"), tr("Select Album:"), albumNames, 0, false, &ok);

  if (ok && !selected.isEmpty()) {
    // Find the selected album ID
    int index = albumNames.indexOf(selected);
    if (index >= 0) {
      int albumId = albumIds[index];

      // Add track to album
      bool success =
          DatabaseManager::instance().addTrackToAlbum(albumId, track);

      if (success) {
        QString trackTitle = track["title"].toString();
        statusLabel->setText("Added \"" + trackTitle + "\" to " + selected);

        // Check if we need to download the cover
        int trackId = track["id"].toInt();
        QString existingCover =
            DatabaseManager::instance().getCoverPath(trackId);
        if (existingCover.isEmpty()) {
          if (track.contains("album")) {
            QJsonObject album = track["album"].toObject();
            if (album.contains("cover")) {
              QString coverId = album["cover"].toString();
              if (!coverId.isEmpty()) {
                QString coverUrl =
                    QString("https://resources.tidal.com/images/%1/640x640.jpg")
                        .arg(coverId.replace("-", "/"));
                downloadManager->downloadCover(coverUrl, trackId);
              }
            }
          }
        }

        // Refresh albums list to update cover if needed
        refreshAlbumsList();
      } else {
        statusLabel->setText("Failed to add track to album");
      }
    }
  }
}

void MainWindow::onAlbumDeleteClicked(int albumId) {
  // Find album name for confirmation
  QString albumName = "Unknown Album";
  if (albumCards.contains(albumId)) {
    albumName = albumCards[albumId]->getAlbumData()["title"].toString();
  }

  // First confirmation dialog
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(
      this, "Delete Album",
      "Are you sure you want to delete the album '" + albumName + "'?\n" +
          "This will remove the album but not the tracks.",
      QMessageBox::Yes | QMessageBox::No);

  if (reply != QMessageBox::Yes) {
    return; // User cancelled
  }

  // Second confirmation dialog
  reply = QMessageBox::warning(this, "Confirm Delete",
                               "This action cannot be undone. Delete '" +
                                   albumName + "'?",
                               QMessageBox::Yes | QMessageBox::Cancel);

  if (reply == QMessageBox::Yes) {
    // Perform deletion
    bool success = DatabaseManager::instance().deleteAlbum(albumId);

    if (success) {
      statusLabel->setText("Deleted album: " + albumName);
      refreshAlbumsList(); // Refresh to remove from UI
    } else {
      statusLabel->setText("Failed to delete album");
      QMessageBox::critical(this, "Error",
                            "Failed to delete the album from the database.");
    }
  }
}

void MainWindow::generateAlbumCoverGrid(int albumId, AlbumCard *card) {
  QList<QJsonObject> tracks =
      DatabaseManager::instance().getAlbumTracks(albumId);

  if (tracks.isEmpty()) {
    return; // No tracks, use default cover
  }

  // Collect up to 4 covers
  QList<QPixmap> covers;
  for (const QJsonObject &track : tracks) {
    if (covers.size() >= 4)
      break;

    int trackId = track["id"].toInt();
    QString coverPath = DatabaseManager::instance().getCoverPath(trackId);
    if (!coverPath.isEmpty()) {
      QPixmap pix(coverPath);
      if (!pix.isNull()) {
        covers.append(pix);
      }
    }
  }

  if (covers.isEmpty())
    return;

  // Create composite image
  QPixmap composite(200, 200);
  composite.fill(Qt::black);
  QPainter painter(&composite);

  if (covers.size() == 1) {
    painter.drawPixmap(0, 0, 200, 200, covers[0]);
  } else if (covers.size() == 2) {
    painter.drawPixmap(0, 0, 100, 200, covers[0]);
    painter.drawPixmap(100, 0, 100, 200, covers[1]);
  } else if (covers.size() == 3) {
    painter.drawPixmap(0, 0, 100, 200, covers[0]);
    painter.drawPixmap(100, 0, 100, 100, covers[1]);
    painter.drawPixmap(100, 100, 100, 100, covers[2]);
  } else {
    painter.drawPixmap(0, 0, 100, 100, covers[0]);
    painter.drawPixmap(100, 0, 100, 100, covers[1]);
    painter.drawPixmap(0, 100, 100, 100, covers[2]);
    painter.drawPixmap(100, 100, 100, 100, covers[3]);
  }

  card->setCoverImage(composite);
}

void MainWindow::onTrackStreamUrl(const QString &url) {
  // Check if this is for downloading a favorite
  if (currentTrackIdForStream != -1) {
    int trackId = currentTrackIdForStream;
    currentTrackIdForStream = -1; // Reset

    statusLabel->setText("Downloading favorite track " +
                         QString::number(trackId) + "...");
    downloadManager->downloadTrack(url, trackId);
    return;
  }

  // Otherwise, play normally
  statusLabel->setText("Playing...");
  player->playUrl(url);
  playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
  // Show cover when playback starts
  if (!coverLabel->pixmap().isNull()) {
    coverLabel->setVisible(true);
  }
}

void MainWindow::onPlayerError(const QString &message) {
  statusLabel->setText("Player Error: " + message);
}

void MainWindow::onApiError(const QString &message) {
  statusLabel->setText("API Error: " + message);
}

void MainWindow::onPlayPauseClicked() {
  // Check current icon state by comparing with standard icons
  // But easier to check player state or just toggle based on a flag.
  // Let's check player state if possible, or just toggle.
  // Actually, QIcon comparison isn't straightforward.
  // Let's rely on player state.

  if (player->isPlaying()) {
    player->pause();
    playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  } else {
    player->play();
    playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
  }
}

void MainWindow::onVolumeChanged(int volume) {
  player->setVolume(volume / 100.0f);
}

void MainWindow::onSeekSliderMoved(int position) {
  // Visual feedback only, actual seek happens on release
}

void MainWindow::onSeekSliderPressed() { isSeeking = true; }

void MainWindow::onSeekSliderReleased() {
  player->seek(seekSlider->value());
  isSeeking = false;
}

void MainWindow::onPositionChanged(qint64 position) {
  if (!isSeeking) {
    seekSlider->setValue(position);
  }
}

void MainWindow::onDurationChanged(qint64 duration) {
  seekSlider->setRange(0, duration);
}

void MainWindow::onCoverImageDownloaded(QNetworkReply *reply) {
  // Check if this is for a favorite card (has User attribute with track ID)
  if (reply->request().attribute(QNetworkRequest::User).isValid()) {
    onFavoriteCoverDownloaded(reply);
    return;
  }

  // Otherwise, it's for the playback cover
  if (reply->error() == QNetworkReply::NoError) {
    QByteArray data = reply->readAll();
    QPixmap pixmap;

    // Try loading with explicit format hint first
    if (!pixmap.loadFromData(data, "JPG")) {
      // Fallback to auto-detection
      if (!pixmap.loadFromData(data)) {
        qDebug() << "Failed to load pixmap from downloaded data";
        // Do NOT hide the label here if we might have shown a local cover
        // already! But wait, if we are here, it means we decided to download.
        // If we decided to download, it means we didn't find a local cover.
        // So it's safe to hide?
        // EXCEPT if the race condition happened:
        // 1. check local -> empty
        // 2. start download
        // 3. check local (in play block) -> found (maybe?) or found fallback
        // 4. show cover
        // 5. download finishes -> fails -> hides cover

        // So, only hide if we don't have a valid pixmap on the label?
        // Or just log error and don't hide.
        // coverLabel->hide();
        return;
      }
    }

    coverLabel->setPixmap(pixmap.scaled(coverLabel->size(), Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
    coverLabel->show();

    // Save this cover!
    // We should probably save it if we went through the trouble of downloading
    // it But DownloadManager handles downloads usually. This is the old ad-hoc
    // download. Let's leave it for now, but ensure we don't hide the label
    // unnecessarily.

  } else {
    qDebug() << "Cover download error:" << reply->errorString();
    // coverLabel->hide(); // Don't hide, might have local cover shown
  }
  reply->deleteLater();
}

void MainWindow::onFavoriteCoverDownloaded(QNetworkReply *reply) {
  QVariant trackIdVar = reply->request().attribute(QNetworkRequest::User);
  if (!trackIdVar.isValid()) {
    qDebug() << "Favorite cover download has no track ID";
    reply->deleteLater();
    return;
  }

  int trackId = trackIdVar.toInt();

  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Network error for favorite track" << trackId << ":"
             << reply->errorString();
    reply->deleteLater();
    return;
  }

  QByteArray data = reply->readAll();
  qDebug() << "Downloaded" << data.size() << "bytes for track" << trackId;
  if (data.size() > 0) {
    qDebug() << "First 20 bytes:" << data.left(20).toHex();
  }

  QPixmap pixmap;
  // Try loading with explicit format since loadFromData might fail on some
  // images
  if (!pixmap.loadFromData(data, "JPG")) {
    qDebug() << "Failed to load as JPG, trying default format for track"
             << trackId;
    if (!pixmap.loadFromData(data)) {
      qDebug() << "Failed to load pixmap with any format for track" << trackId;
      reply->deleteLater();
      return;
    }
  }

  // Find the card for this track
  for (auto *card : favoriteCards) {
    if (card->getTrackId() == trackId) {
      card->setCoverImage(pixmap);
      qDebug() << "Set cover image for track" << trackId;
      break;
    }
  }

  reply->deleteLater();
}

void MainWindow::onAlbumLoaded(const QJsonObject &album) {
  // Placeholder for album handling
  qDebug() << "Album loaded:" << album;
}

void MainWindow::onFavoriteToggled(const QJsonObject &track, bool isFavorite) {
  int trackId = track["id"].toInt();

  if (isFavorite) {
    if (DatabaseManager::instance().addFavorite(track)) {
      // Start download if not already downloaded
      QString localPath = DatabaseManager::instance().getFilePath(trackId);
      if (localPath.isEmpty() || !QFile::exists(localPath)) {
        statusLabel->setText("Fetching stream URL to download favorite...");
        // Set the trackId for the stream URL handler to know it's for
        // download
        currentTrackIdForStream = trackId;
        hifiClient->getTrackStream(trackId);
      } else {
        statusLabel->setText("Track already downloaded.");
      }
    } else {
      // This else block means addFavorite failed, which shouldn't happen if
      // track is valid. If it means "if not isFavorite", then the logic is
      // inverted. Assuming the original intent was: if isFavorite, add it. If
      // not, remove it. The original code had an issue here. Correcting based
      // on common pattern.
      qDebug() << "Failed to add favorite track" << trackId;
    }
  } else { // if not isFavorite
    DatabaseManager::instance().removeFavorite(trackId);
    // Optional: delete file
    QString localPath = DatabaseManager::instance().getFilePath(trackId);
    if (!localPath.isEmpty() && QFile::exists(localPath)) {
      QFile::remove(localPath);
      DatabaseManager::instance().updateFilePath(trackId,
                                                 ""); // Clear path in DB
    }
  }

  refreshFavoritesList();
}

void MainWindow::refreshFavoritesList() {
  // Clear existing items
  QLayoutItem *item;
  while ((item = favouritesGrid->takeAt(0)) != nullptr) {
    delete item->widget();
    delete item;
  }
  favoriteCards.clear();

  auto favs = DatabaseManager::instance().getFavorites();

  if (favs.isEmpty()) {
    QLabel *emptyLabel = new QLabel("No favorites yet", this);
    emptyLabel->setStyleSheet("color: #b3b3b3; font-size: 14px;");
    emptyLabel->setAlignment(Qt::AlignCenter);
    favouritesGrid->addWidget(emptyLabel, 0, 0);
    return;
  }

  int row = 0;
  int col = 0;
  int maxCols = 6; // Adjusted for 120px cards

  for (const auto &track : favs) {
    int id = track["id"].toInt();
    trackCache.insert(id, track);

    FavoriteCard *card = new FavoriteCard(track);
    connect(card, &FavoriteCard::clicked, this, [this, id]() {
      // Populate playlist with all favorites
      currentPlaylist.clear();
      auto favs = DatabaseManager::instance().getFavorites();
      for (const auto &f : favs)
        currentPlaylist.append(f["id"].toInt());

      currentTrackIndex = currentPlaylist.indexOf(id);
      onFavoriteCardClicked(id);
    });
    connect(card, &FavoriteCard::unfavoriteClicked, this,
            [this, track](int trackId) {
              DatabaseManager::instance().removeFavorite(trackId);
              refreshFavoritesList();
            });

    favouritesGrid->addWidget(card, row, col);
    favoriteCards.append(card);

    // Check if we have a local cover first
    QString localCoverPath = track["coverPath"].toString();
    if (!localCoverPath.isEmpty() && QFile::exists(localCoverPath)) {
      QPixmap pixmap(localCoverPath);
      if (!pixmap.isNull()) {
        card->setCoverImage(pixmap);
      } else {
        // If local file is bad, try downloading again
        QString coverId = track["album"].toObject()["cover"].toString();
        if (!coverId.isEmpty()) {
          QString coverUrl =
              QString("https://resources.tidal.com/images/%1/640x640.jpg")
                  .arg(coverId.replace("-", "/"));
          downloadManager->downloadCover(coverUrl, id);
        }
      }
    } else {
      // No local cover, try downloading
      QString coverId = track["album"].toObject()["cover"].toString();
      if (!coverId.isEmpty()) {
        QString coverUrl =
            QString("https://resources.tidal.com/images/%1/640x640.jpg")
                .arg(coverId.replace("-", "/"));
        downloadManager->downloadCover(coverUrl, id);
      }
    }

    col++;
    if (col >= maxCols) {
      col = 0;
      row++;
    }
  }
}

void MainWindow::onDownloadFinished(int trackId, const QString &filePath) {
  DatabaseManager::instance().updateFilePath(trackId, filePath);
  statusLabel->setText("Download complete for track " +
                       QString::number(trackId));
}

void MainWindow::onHomeClicked() {
  pageStack->setCurrentIndex(0);
  statusLabel->setText("Ready");
}

void MainWindow::onFavoriteCardClicked(int trackId) {
  // Update player info
  QJsonObject track;

  if (trackCache.contains(trackId)) {
    track = trackCache[trackId];
  } else {
    // Track not in cache, fetch from database
    track = DatabaseManager::instance().getTrack(trackId);
    if (!track.isEmpty()) {
      // Add to cache for future use
      trackCache.insert(trackId, track);
    } else {
      statusLabel->setText("Error: Favorite track not found in database.");
      coverLabel->hide();
      return;
    }
  }

  QString title = track["title"].toString();
  QString artist = track["artist"].toObject()["name"].toString();

  playerTitleLabel->setText(title);
  playerArtistLabel->setText(artist);
  playerTitleLabel->show();
  playerArtistLabel->show();

  statusLabel->setText("Playing: " + title + " by " + artist);

  if (track.contains("album")) {
    // Check for local cover first
    QString localCoverPath = DatabaseManager::instance().getCoverPath(trackId);

    if (localCoverPath.isEmpty()) {
      QString appData =
          QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
      QString standardPath =
          appData + "/covers/" + QString::number(trackId) + ".jpg";
      if (QFile::exists(standardPath)) {
        localCoverPath = standardPath;
        // Optionally update DB here?
        DatabaseManager::instance().updateCoverPath(trackId, standardPath);
      }
    }

    if (!localCoverPath.isEmpty() && QFile::exists(localCoverPath)) {
      // running locally, no fetch brother
      qDebug() << "Cover available locally at" << localCoverPath
               << "- skipping network fetch";
    } else {
      qDebug() << "Cover NOT found locally for" << trackId
               << "- fetching from network";
      QJsonObject album = track["album"].toObject();
      if (album.contains("cover")) {
        QString coverId = album["cover"].toString();
        if (!coverId.isEmpty()) {
          QString coverUrl =
              QString("https://resources.tidal.com/images/%1/640x640.jpg")
                  .arg(coverId.replace("-", "/"));

          QNetworkRequest request{QUrl(coverUrl)};
          QNetworkReply *reply = imageManager->get(request);

          connect(reply, &QNetworkReply::finished, this,
                  [this, reply]() { onCoverImageDownloaded(reply); });
        } else {
          qDebug() << "Favorite track" << trackId
                   << "album has empty cover ID.";
          coverLabel->hide();
        }
      } else {
        qDebug() << "Favorite track" << trackId
                 << "album has no 'cover' field.";
        coverLabel->hide();
      }
    }
  } else {
    qDebug() << "Favorite track" << trackId << "has no 'album' field.";
    coverLabel->hide();
  }

  // Check if we have the file locally for this track
  QString localPath = DatabaseManager::instance().getFilePath(trackId);
  qDebug() << "Playing favorite track" << trackId;
  qDebug() << "Database file path:" << localPath;
  qDebug() << "File exists:" << QFile::exists(localPath);

  if (!localPath.isEmpty() && QFile::exists(localPath)) {
    statusLabel->setText("Playing from local cache...");
    qDebug() << "Playing from local file:" << localPath;
    player->playUrl(QUrl::fromLocalFile(localPath).toString());
    // Show cover
    QString coverPath = DatabaseManager::instance().getCoverPath(trackId);
    if (!coverPath.isEmpty() && QFile::exists(coverPath)) {
      QPixmap pixmap(coverPath);
      if (!pixmap.isNull()) {
        coverLabel->setPixmap(pixmap.scaled(
            coverLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        coverLabel->show();
        playerTitleLabel->show();
        playerArtistLabel->show();
      }
    } else {
      // Try standard location as fallback
      QString appData =
          QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
      QString potentialCover =
          appData + "/covers/" + QString::number(trackId) + ".jpg";
      if (QFile::exists(potentialCover)) {
        QPixmap pixmap(potentialCover);
        if (!pixmap.isNull()) {
          coverLabel->setPixmap(pixmap.scaled(coverLabel->size(),
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation));
          coverLabel->show();
          playerTitleLabel->show();
          playerArtistLabel->show();
        }
      }
    }
  } else {
    qDebug() << "File not available locally, fetching stream...";
    hifiClient->getTrackStream(trackId);
  }
}

void MainWindow::onNextClicked() {
  if (currentPlaylist.isEmpty() || currentTrackIndex == -1)
    return;

  int nextIndex = currentTrackIndex + 1;
  if (nextIndex >= currentPlaylist.size()) {
    nextIndex = 0; // Loop back to start
  }

  currentTrackIndex = nextIndex;
  int nextTrackId = currentPlaylist[nextIndex];

  // Reuse logic - if we have the track item widget, we can simulate click,
  // but better to just call the playback logic directly.
  // However, onTrackSelected does UI updates too.
  // For simplicity, let's try to find the item in the list if we are in search
  // view. But we might be in favorites view.

  // Let's just trigger playback directly.
  // We need to update UI (title, artist, cover).
  // We can reuse onFavoriteCardClicked logic if it's a favorite/album track.
  // Or we can extract a common playTrack(int id) method.

  // For now, let's call onFavoriteCardClicked which handles fetching metadata
  // from DB/Cache This works well if the track is in DB or Cache.
  onFavoriteCardClicked(nextTrackId);
}

void MainWindow::onPrevClicked() {
  if (currentPlaylist.isEmpty() || currentTrackIndex == -1)
    return;

  int prevIndex = currentTrackIndex - 1;
  if (prevIndex < 0) {
    prevIndex = currentPlaylist.size() - 1; // Loop to end
  }

  currentTrackIndex = prevIndex;
  int prevTrackId = currentPlaylist[prevIndex];

  onFavoriteCardClicked(prevTrackId);
}
