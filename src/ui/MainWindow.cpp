#include "MainWindow.hpp"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), hifiClient(new HifiClient(this)),
      player(new AudioPlayer(this)),
      imageManager(new QNetworkAccessManager(this)) {

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  // UI Components
  searchBox = new QLineEdit(this);
  searchBox->setPlaceholderText("Search for tracks...");
  searchButton = new QPushButton("Search", this);

  resultsList = new QListWidget(this);

  playPauseButton = new QPushButton("Play", this);
  volumeSlider = new QSlider(Qt::Horizontal, this);
  volumeSlider->setRange(0, 100);
  volumeSlider->setValue(100);

  seekSlider = new QSlider(Qt::Horizontal, this);
  seekSlider->setRange(0, 0);

  coverLabel = new QLabel(this);
  coverLabel->setFixedSize(200, 200);
  coverLabel->setStyleSheet("background-color: #333; border: 1px solid #555;");
  coverLabel->setAlignment(Qt::AlignCenter);
  coverLabel->setText("No Cover");

  statusLabel = new QLabel("Ready", this);

  // Layout
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

  QHBoxLayout *topLayout = new QHBoxLayout();
  topLayout->addWidget(searchBox); // Changed from searchBar to searchBox
  topLayout->addWidget(searchButton);

  QHBoxLayout *contentLayout = new QHBoxLayout();
  contentLayout->addWidget(resultsList);
  contentLayout->addWidget(coverLabel);

  QHBoxLayout *controlsLayout = new QHBoxLayout();
  controlsLayout->addWidget(playPauseButton);
  controlsLayout->addWidget(seekSlider);
  controlsLayout->addWidget(new QLabel("Vol:"));
  controlsLayout->addWidget(volumeSlider);

  mainLayout->addLayout(topLayout);
  mainLayout->addLayout(contentLayout);
  mainLayout->addLayout(controlsLayout);
  mainLayout->addWidget(statusLabel);

  // Logic
  // client = new HifiClient(this); // Removed, now in initializer list
  // player = new AudioPlayer(this); // Removed, now in initializer list

  // Connections
  connect(searchButton, &QPushButton::clicked, this,
          &MainWindow::onSearchClicked);
  connect(searchBox, &QLineEdit::returnPressed, this,
          &MainWindow::onSearchClicked);

  connect(hifiClient, &HifiClient::searchResults, this,
          [this](const QJsonArray &tracks) {
            resultsList->clear();
            trackCache.clear();

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
              QString title = track["title"].toString();
              int id = track["id"].toInt();

              QJsonObject artist = track["artist"].toObject();
              QString artistName = artist["name"].toString();

              // Cache the track data
              trackCache[id] = track;

              QListWidgetItem *item = new QListWidgetItem(
                  QString("%1 - %2").arg(title, artistName));
              item->setData(Qt::UserRole, id);
              resultsList->addItem(item);
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
  if (query.isEmpty())
    return;

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
        }
      }
    }
  }

  hifiClient->getTrackStream(trackId);
}

void MainWindow::onTrackStreamUrl(const QString &url) {
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
  if (reply->error() == QNetworkReply::NoError) {
    QByteArray data = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(data);
    coverLabel->setPixmap(pixmap.scaled(coverLabel->size(), Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
  }
  reply->deleteLater();
}

void MainWindow::onAlbumLoaded(const QJsonObject &album) {
  // Placeholder for album handling
  qDebug() << "Album loaded:" << album;
}
