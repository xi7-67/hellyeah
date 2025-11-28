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

  // Grid layout for favorites
  favouritesScrollArea = new QScrollArea();
  favouritesScrollArea->setWidgetResizable(true);
  favouritesScrollArea->setStyleSheet(
      "background-color: #121212; border: none;");

  favouritesContainer = new QWidget();
  favouritesGrid = new QGridLayout(favouritesContainer);
  favouritesGrid->setSpacing(15);
  favouritesGrid->setContentsMargins(10, 10, 10, 10);
  favouritesContainer->setStyleSheet("background-color: #121212;");

  favouritesScrollArea->setWidget(favouritesContainer);

  refreshFavoritesList();

  QLabel *albumsLabel = new QLabel("Albums");
  albumsLabel->setStyleSheet(
      "font-size: 20px; font-weight: bold; margin-top: 10px; color: #fff;");
  albumsList = new QListWidget();
  albumsList->addItem("No albums yet");

  homeLayout->addWidget(favouritesLabel);
  homeLayout->addWidget(favouritesScrollArea, 1);
  homeLayout->addWidget(albumsLabel);
  homeLayout->addWidget(albumsList);

  // SEARCH RESULTS VIEW
  searchPage = new QWidget();
  QVBoxLayout *searchResultsLayout = new QVBoxLayout(searchPage);
  resultsList = new QListWidget();
  searchResultsLayout->addWidget(resultsList);

  pageStack->addWidget(homePage);   // index 0 - Home
  pageStack->addWidget(searchPage); // index 1 - Search Results
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
  } else {
    hifiClient->getTrackStream(trackId);
  }
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
    coverLabel->show();
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

  // Clear grid
  QLayoutItem *item;
  while ((item = favouritesGrid->takeAt(0)) != nullptr) {
    delete item;
  }

  QList<QJsonObject> favs = DatabaseManager::instance().getFavorites();

  if (favs.isEmpty()) {
    QLabel *emptyLabel =
        new QLabel("No favourites yet - search and star tracks to add!");
    emptyLabel->setStyleSheet(
        "color: #b3b3b3; font-size: 14px; padding: 20px;");
    emptyLabel->setAlignment(Qt::AlignCenter);
    favouritesGrid->addWidget(emptyLabel, 0, 0);
    return;
  }

  int row = 0;
  int col = 0;
  const int columns = 5;

  for (const auto &track : favs) {
    int id = track["id"].toInt();
    trackCache.insert(id, track);

    FavoriteCard *card = new FavoriteCard(track);
    connect(card, &FavoriteCard::clicked, this,
            &MainWindow::onFavoriteCardClicked);
    connect(card, &FavoriteCard::unfavoriteClicked, this,
            [this, track](int) { onFavoriteToggled(track, false); });

    favoriteCards.insert(id, card);
    favouritesGrid->addWidget(card, row, col);

    // Fetch cover image
    if (track.contains("album")) {
      QJsonObject album = track["album"].toObject();
      if (album.contains("cover")) {
        QString coverId = album["cover"].toString();
        if (!coverId.isEmpty()) {
          QString coverUrl =
              QString("https://resources.tidal.com/images/%1/640x640.jpg")
                  .arg(coverId.replace("-", "/"));
          QNetworkRequest req{QUrl(coverUrl)};
          req.setAttribute(QNetworkRequest::User, id); // Store track ID
          imageManager->get(req);
        }
      }
    }

    col++;
    if (col >= columns) {
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
  } else {
    qDebug() << "File not available locally, fetching stream...";
    hifiClient->getTrackStream(trackId);
  }
}

void MainWindow::onFavoriteCoverDownloaded(QNetworkReply *reply) {
  if (reply->error() == QNetworkReply::NoError) {
    int trackId = reply->request().attribute(QNetworkRequest::User).toInt();
    if (favoriteCards.contains(trackId)) {
      QByteArray data = reply->readAll();
      QPixmap pixmap;
      pixmap.loadFromData(data);
      favoriteCards[trackId]->setCoverImage(pixmap);
    }
  }
  reply->deleteLater();
}
