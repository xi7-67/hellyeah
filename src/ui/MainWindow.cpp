#include "MainWindow.hpp"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), hifiClient(new HifiClient(this)),
      player(new AudioPlayer(this)),
      imageManager(new QNetworkAccessManager(this)),
      downloadManager(new DownloadManager(this)) {

  connect(downloadManager, &DownloadManager::downloadFinished, this,
          &MainWindow::onDownloadFinished);

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

  // Albums Header with + Button
  QHBoxLayout *albumsHeaderLayout = new QHBoxLayout();
  QLabel *albumsLabel = new QLabel("Albums");
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

  albumTracksList = new QListWidget();
  albumTracksList->setStyleSheet(
      "QListWidget { background-color: #121212; border: none; outline: none; }"
      "QListWidget::item { border-bottom: 1px solid #282828; padding: 5px; }"
      "QListWidget::item:selected { background-color: #282828; }");
  albumViewLayout->addWidget(albumTracksList);

  pageStack->addWidget(homePage);   // index 0 - Home
  pageStack->addWidget(searchPage); // index 1 - Search Results
  pageStack->addWidget(albumPage);  // index 2 - Album View
  pageStack->setCurrentIndex(0);    // Start with Home
  // Shared UI (Cover + Controls)
  coverLabel = new QLabel(this);
  coverLabel->setFixedSize(200, 200);
  coverLabel->setStyleSheet("background-color: #333; border: 1px solid #555;");
  coverLabel->setAlignment(Qt::AlignCenter);
  coverLabel->setText("No Cover");
  coverLabel->hide(); // Hide by default

  playPauseButton = new QPushButton("Play", this);
  volumeSlider = new QSlider(Qt::Horizontal, this);
  volumeSlider->setRange(0, 100);
  volumeSlider->setValue(100);

  seekSlider = new QSlider(Qt::Horizontal, this);
  seekSlider->setRange(0, 0);

  statusLabel = new QLabel("Ready", this);

  // Content layout (page stack + cover)
  QHBoxLayout *contentLayout = new QHBoxLayout();
  contentLayout->addWidget(pageStack, 1);
  contentLayout->addWidget(coverLabel);

  // Controls layout
  QHBoxLayout *controlsLayout = new QHBoxLayout();
  controlsLayout->addWidget(playPauseButton);
  controlsLayout->addWidget(seekSlider);
  controlsLayout->addWidget(new QLabel("Vol:"));
  controlsLayout->addWidget(volumeSlider);

  // Assemble main layout
  mainLayout->addLayout(searchLayout);
  mainLayout->addLayout(contentLayout);
  mainLayout->addLayout(controlsLayout);
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

  connect(playPauseButton, &QPushButton::clicked, this,
          &MainWindow::onPlayPauseClicked);

  connect(volumeSlider, &QSlider::valueChanged,
          this, // Changed lambda to method call
          &MainWindow::onVolumeChanged);

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
  statusLabel->setText(
      QString("Fetching stream for track ID: %1").arg(trackId));

  // Fetch cover for the selected track
  if (trackCache.contains(trackId)) {
    QJsonObject track = trackCache[trackId];
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
          coverLabel->hide();
        }
      } else {
        coverLabel->hide();
      }
    } else {
      coverLabel->hide();
    }
  } else {
    coverLabel->hide();
  }

  // Check if we have a local file for this track
  QString localPath = DatabaseManager::instance().getFilePath(trackId);
  if (!localPath.isEmpty() && QFile::exists(localPath)) {
    statusLabel->setText("Playing from local cache...");
    player->playUrl(QUrl::fromLocalFile(localPath).toString());
    playPauseButton->setText("Pause");
    // Show cover
    if (!coverLabel->pixmap().isNull()) {
      coverLabel->show();
    }
  } else {
    hifiClient->getTrackStream(trackId);
  }
}

void MainWindow::onAddAlbumClicked() {
  bool ok;
  QString text =
      QInputDialog::getText(this, tr("Create New Album"), tr("Album Name:"),
                            QLineEdit::Normal, "", &ok);
  if (ok && !text.isEmpty()) {
    int albumId = DatabaseManager::instance().createAlbum(text);
    if (albumId != -1) {
      statusLabel->setText("Album created: " + text);
      refreshAlbumsList();
    } else {
      statusLabel->setText("Failed to create album");
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

    // Generate composite cover from track covers
    generateAlbumCoverGrid(album["id"].toInt(), card);
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

  // Load tracks (placeholder for now)
  albumTracksList->clear();
  QList<QJsonObject> tracks =
      DatabaseManager::instance().getAlbumTracks(albumId);

  if (tracks.isEmpty()) {
    QListWidgetItem *item = new QListWidgetItem("No tracks in this album yet.");
    item->setForeground(QBrush(QColor("#b3b3b3")));
    albumTracksList->addItem(item);
  } else {
    for (const QJsonObject &track : tracks) {
      int trackId = track["id"].toInt();
      trackCache.insert(trackId, track); // Add to cache for playback

      TrackItemWidget *itemWidget = new TrackItemWidget(track, true);
      QListWidgetItem *item = new QListWidgetItem(albumTracksList);
      item->setSizeHint(itemWidget->sizeHint());
      item->setData(Qt::UserRole, trackId); // Store track ID for playback
      albumTracksList->setItemWidget(item, itemWidget);

      // Connect signals
      connect(itemWidget, &TrackItemWidget::favoriteToggled, this,
              [this, track](bool isFavorite) {
                onFavoriteToggled(track, isFavorite);
              });
      connect(itemWidget, &TrackItemWidget::addToAlbumClicked, this,
              [this, track]() { onAddToAlbumClicked(track); });
    }
  }

  // Connect double-click to play tracks
  connect(albumTracksList, &QListWidget::itemDoubleClicked, this,
          [this](QListWidgetItem *item) {
            int trackId = item->data(Qt::UserRole).toInt();
            if (trackCache.contains(trackId)) {
              onTrackSelected(item);
            }
          });

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

  // For now, just use the first track's cover
  // TODO: Implement proper 2x2 grid composition
  QJsonObject firstTrack = tracks.first();
  QString coverId = firstTrack["album"].toObject()["cover"].toString();

  if (coverId.isEmpty()) {
    return;
  }

  // Fetch cover image using the same method as track covers
  QString coverUrl = "https://api.napster.com/imageserver/v2/albums/" +
                     coverId + "/images/70x70.jpg";
  QNetworkRequest request{QUrl(coverUrl)};
  QNetworkReply *reply = imageManager->get(request);

  connect(reply, &QNetworkReply::finished, this, [this, reply, card]() {
    if (reply->error() == QNetworkReply::NoError) {
      QPixmap pixmap;
      pixmap.loadFromData(reply->readAll());

      if (!pixmap.isNull()) {
        card->setCoverImage(pixmap);
      }
    }
    reply->deleteLater();
  });
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
  playPauseButton->setText("Pause");
  // Show cover when playback starts
  if (!coverLabel->pixmap().isNull()) {
    coverLabel->show();
  }
}

void MainWindow::onPlayerError(const QString &message) {
  statusLabel->setText("Player Error: " + message);
}

void MainWindow::onApiError(const QString &message) {
  statusLabel->setText("API Error: " + message);
}

void MainWindow::onPlayPauseClicked() {
  if (playPauseButton->text() == "Pause") {
    player->pause();
    playPauseButton->setText("Play");
  } else {
    player->play();
    playPauseButton->setText("Pause");
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
    pixmap.loadFromData(data);
    coverLabel->setPixmap(pixmap.scaled(coverLabel->size(), Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
    // Only show if we're actively playing (not just loaded)
    // Cover will be shown when playback starts
  } else {
    coverLabel->hide();
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
        // Set the trackId for the stream URL handler to know it's for download
        currentTrackIdForStream = trackId;
        hifiClient->getTrackStream(trackId);
      } else {
        statusLabel->setText("Track already downloaded.");
      }
    }
  } else {
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
  // Clear existing cards
  for (auto *card : favoriteCards) {
    card->deleteLater();
  }
  favoriteCards.clear();

  // Clear layout
  QLayoutItem *item;
  while ((item = favouritesGrid->takeAt(0)) != nullptr) {
    delete item->widget();
    delete item;
  }

  QList<QJsonObject> favs = DatabaseManager::instance().getFavorites();

  if (favs.isEmpty()) {
    QLabel *emptyLabel =
        new QLabel("No favourites yet - search and star tracks to add!");
    emptyLabel->setStyleSheet(
        "color: #b3b3b3; font-size: 14px; padding: 20px;");
    emptyLabel->setAlignment(Qt::AlignCenter);
    favouritesGrid->addWidget(emptyLabel);
    return;
  }

  int row = 0;
  int col = 0;
  int maxCols = 6; // Adjusted for 120px cards

  for (const auto &track : favs) {
    int id = track["id"].toInt();
    trackCache.insert(id, track);

    FavoriteCard *card = new FavoriteCard(track);
    // card->setFixedWidth(280); // Let grid handle width or set reasonable max
    // card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Fixed
    // size now handled in card
    connect(card, &FavoriteCard::clicked, this,
            [this, id]() { onFavoriteCardClicked(id); });
    connect(card, &FavoriteCard::unfavoriteClicked, this,
            [this, track](int trackId) {
              DatabaseManager::instance().removeFavorite(trackId);
              refreshFavoritesList();
            });

    favouritesGrid->addWidget(card, row, col);
    favoriteCards.append(card);

    // Fetch cover
    if (track.contains("album")) {
      QJsonObject album = track["album"].toObject();
      if (album.contains("cover")) {
        QString coverId = album["cover"].toString();
        // Use Tidal URL format
        QString coverUrl =
            QString("https://resources.tidal.com/images/%1/640x640.jpg")
                .arg(coverId.replace("-", "/"));

        qDebug() << "Cover URL for track" << id << ":" << coverUrl;
        qDebug() << "Cover ID:" << coverId;

        QNetworkRequest request{QUrl(coverUrl)};
        QNetworkReply *reply = imageManager->get(request);

        connect(reply, &QNetworkReply::finished, this,
                [this, reply, card, id]() {
                  if (reply->error() == QNetworkReply::NoError) {
                    QPixmap pixmap;
                    pixmap.loadFromData(reply->readAll());
                    if (!pixmap.isNull()) {
                      card->setCoverImage(pixmap);
                    } else {
                      qDebug() << "Failed to load pixmap for track" << id;
                    }
                  } else {
                    qDebug() << "Network error for track" << id << ":"
                             << reply->errorString();
                  }
                  reply->deleteLater();
                });
      }
    }

    col++;
    if (col >= maxCols) {
      col = 0;
      row++;
    }
  }

  // Add stretch to push items to top-left if needed, or rely on grid behavior
  // favouritesGrid->setRowStretch(row + 1, 1);
  // favouritesGrid->setColumnStretch(maxCols, 1);
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
  // Same as onTrackSelected - trigger playback
  statusLabel->setText(
      QString("Fetching stream for track ID: %1").arg(trackId));

  // Fetch cover for the selected track (already loaded in card, but needed for
  // playback)
  if (trackCache.contains(trackId)) {
    QJsonObject track = trackCache[trackId];
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
          coverLabel->hide();
        }
      } else {
        coverLabel->hide();
      }
    } else {
      coverLabel->hide();
    }
  } else {
    coverLabel->hide();
  }

  // Check if we have a local file for this track
  QString localPath = DatabaseManager::instance().getFilePath(trackId);
  qDebug() << "Playing favorite track" << trackId;
  qDebug() << "Database file path:" << localPath;
  qDebug() << "File exists:" << QFile::exists(localPath);

  if (!localPath.isEmpty() && QFile::exists(localPath)) {
    statusLabel->setText("Playing from local cache...");
    qDebug() << "Playing from local file:" << localPath;
    player->playUrl(QUrl::fromLocalFile(localPath).toString());
    playPauseButton->setText("Pause");
    // Show cover
    if (!coverLabel->pixmap().isNull()) {
      coverLabel->show();
    }
  } else {
    qDebug() << "File not available locally, fetching stream...";
    hifiClient->getTrackStream(trackId);
  }
}

void MainWindow::onFavoriteCoverDownloaded(QNetworkReply *reply) {
  if (reply->error() == QNetworkReply::NoError) {
    int trackId = reply->request().attribute(QNetworkRequest::User).toInt();
    for (auto *card : favoriteCards) {
      if (card->getTrackId() == trackId) {
        QByteArray data = reply->readAll();
        QPixmap pixmap;
        pixmap.loadFromData(data);
        card->setCoverImage(pixmap);
        break;
      }
    }
  }
  reply->deleteLater();
}
