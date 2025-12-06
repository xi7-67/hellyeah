#include "ArtistProfilePage.hpp"
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QScrollBar>

ArtistProfilePage::ArtistProfilePage(QWidget *parent) : QWidget(parent) {
  setupUI();
}

void ArtistProfilePage::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(20, 20, 20, 20);
  mainLayout->setSpacing(20);

  // Back button
  backButton = new QPushButton("← Back", this);
  backButton->setCursor(Qt::PointingHandCursor);
  backButton->setStyleSheet("QPushButton { "
                            "  color: #fff; "
                            "  font-weight: bold; "
                            "  border: none; "
                            "  font-size: 16px; "
                            "  background: transparent; "
                            "  padding: 5px 10px; "
                            "}"
                            "QPushButton:hover { "
                            "  color: #FFFFFF; "
                            "}");
  connect(backButton, &QPushButton::clicked, this,
          &ArtistProfilePage::backClicked);

  // Hero Header Section
  QWidget *headerWidget = new QWidget(this);
  QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
  headerLayout->setSpacing(30);
  headerLayout->setContentsMargins(0, 0, 0, 0);

  // Artist cover
  artistCoverLabel = new QLabel(this);
  artistCoverLabel->setFixedSize(220, 220);
  artistCoverLabel->setScaledContents(false);
  artistCoverLabel->setAlignment(Qt::AlignCenter);
  artistCoverLabel->setStyleSheet("background-color: #282828; "
                                  "border-radius: 110px; " // Circular
                                  "border: 4px solid #FFFFFF;");
  artistCoverLabel->setText("♪");

  // Artist info (name + bio)
  QWidget *infoWidget = new QWidget(this);
  QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
  infoLayout->setSpacing(15);
  infoLayout->setContentsMargins(0, 0, 0, 0);

  artistNameLabel = new QLabel("Artist Name", this);
  artistNameLabel->setStyleSheet("font-size: 64px; "  // Larger, more impactful
                                 "font-weight: 800; " // Extra bold
                                 "color: #fff; "
                                 "margin: 0; "
                                 "qproperty-alignment: AlignLeft;");
  // Add a subtle text shadow for depth
  QGraphicsDropShadowEffect *textShadow = new QGraphicsDropShadowEffect(this);
  textShadow->setBlurRadius(10);
  textShadow->setColor(QColor(0, 0, 0, 150));
  textShadow->setOffset(2, 2);
  artistNameLabel->setGraphicsEffect(textShadow);

  artistNameLabel->setWordWrap(true);

  bioLabel = new QLabel("", this);
  bioLabel->setStyleSheet("font-size: 14px; "
                          "color: #b3b3b3; "
                          "line-height: 1.5;");
  bioLabel->setWordWrap(true);
  bioLabel->setMaximumWidth(600);
  bioLabel->hide(); // Hide initially

  infoLayout->addWidget(artistNameLabel);
  infoLayout->addWidget(bioLabel);
  infoLayout->addStretch();

  headerLayout->addWidget(artistCoverLabel);
  headerLayout->addWidget(infoWidget, 1);

  // Top Tracks Section
  QLabel *topTracksTitle = new QLabel("Top Tracks", this);
  topTracksTitle->setStyleSheet("font-size: 24px; "
                                "font-weight: bold; "
                                "color: #fff; "
                                "margin-top: 20px;");

  topTracksContainer = new QWidget(this);
  topTracksLayout = new QVBoxLayout(topTracksContainer);
  topTracksLayout->setSpacing(2);
  topTracksLayout->setContentsMargins(0, 0, 0, 0);
  topTracksContainer->setStyleSheet("background-color: transparent;");

  // Albums Section
  QLabel *albumsTitle = new QLabel("Albums", this);
  albumsTitle->setStyleSheet("font-size: 24px; "
                             "font-weight: bold; "
                             "color: #fff; "
                             "margin-top: 30px;");

  albumsScrollArea = new QScrollArea(this);
  albumsScrollArea->setWidgetResizable(true);
  albumsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  albumsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  albumsScrollArea->setFixedHeight(200);
  albumsScrollArea->setStyleSheet(
      "QScrollArea { background-color: transparent; border: none; }"
      "QScrollBar:horizontal { height: 8px; background: #282828; }"
      "QScrollBar::handle:horizontal { background: #535353; border-radius: "
      "4px; }"
      "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { "
      "border: none; background: none; }");

  albumsContainer = new QWidget();
  albumsLayout = new QVBoxLayout(albumsContainer);
  albumsLayout->setSpacing(10);
  albumsLayout->setContentsMargins(0, 0, 0, 0);
  albumsContainer->setStyleSheet("background-color: transparent;");
  albumsScrollArea->setWidget(albumsContainer);

  // Add all to main layout
  mainLayout->addWidget(backButton, 0, Qt::AlignLeft);
  mainLayout->addWidget(headerWidget);
  mainLayout->addWidget(topTracksTitle);
  mainLayout->addWidget(topTracksContainer);
  mainLayout->addWidget(albumsTitle);
  mainLayout->addWidget(albumsScrollArea);
  mainLayout->addStretch();

  // Overall page styling with Gradient Background
  // Using a subtle gradient from a dark grey/green to black to remove "black
  // sections"
  setStyleSheet("ArtistProfilePage { "
                "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                "    stop:0 #1a1a1a, stop:1 #000000); "
                "}");
}

void ArtistProfilePage::setArtistData(const QJsonObject &artistData) {
  m_artistData = artistData;

  if (artistData.isEmpty()) {
    artistNameLabel->setText("Artist Not Found");
    bioLabel->setText("Could not load artist data. Please try again later.");
    bioLabel->show();
    return;
  }

  QString name = artistData["name"].toString();
  QString bio = artistData["bio"].toString();

  artistNameLabel->setText(name);

  if (!bio.isEmpty()) {
    // Limit bio to first 300 characters for preview
    if (bio.length() > 300) {
      bio = bio.left(297) + "...";
    }
    bioLabel->setText(bio);
    bioLabel->show();
  } else {
    bioLabel->hide();
  }
}

void ArtistProfilePage::setArtistCover(const QPixmap &cover) {
  if (!cover.isNull()) {
    artistCoverLabel->setPixmap(cover.scaled(
        220, 220, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    artistCoverLabel->setText("");
  }
}

void ArtistProfilePage::setTopTracks(const QJsonArray &tracks) {
  clearTopTracks();

  int count = 0;
  for (const auto &val : tracks) {
    if (count >= 10)
      break; // Limit to top 10

    QJsonObject track = val.toObject();
    int trackId = track["id"].toInt();
    QString title = track["title"].toString();
    QString duration = track["duration"].toString(); // Might need formatting

    // Create track row
    QWidget *trackRow = new QWidget(this);
    QHBoxLayout *rowLayout = new QHBoxLayout(trackRow);
    rowLayout->setContentsMargins(15, 12, 15, 12);
    rowLayout->setSpacing(15);

    // Track number
    QLabel *numberLabel = new QLabel(QString::number(count + 1), trackRow);
    numberLabel->setFixedWidth(30);
    numberLabel->setAlignment(Qt::AlignCenter);
    numberLabel->setStyleSheet("font-size: 16px; "
                               "color: #b3b3b3; "
                               "font-weight: bold;");

    // Track title
    QLabel *titleLabel = new QLabel(title, trackRow);
    titleLabel->setStyleSheet("font-size: 14px; "
                              "color: #fff; "
                              "font-weight: 500;");

    rowLayout->addWidget(numberLabel);
    rowLayout->addWidget(titleLabel, 1);
    rowLayout->addStretch();

    // Style the row
    trackRow->setStyleSheet(
        "QWidget { "
        "  background-color: rgba(0, 0, 0, 0.3); " // Darker background for
                                                   // readability
        "  border-radius: 6px; "
        "}"
        "QWidget:hover { "
        "  background-color: rgba(255, 255, 255, 0.1); " // Brighter on hover
        "}");
    trackRow->setCursor(Qt::PointingHandCursor);

    // Make clickable
    trackRow->installEventFilter(this);
    trackRow->setProperty("trackId", trackId);

    topTracksLayout->addWidget(trackRow);
    count++;
  }

  if (count == 0) {
    QLabel *emptyLabel = new QLabel("No top tracks available", this);
    emptyLabel->setStyleSheet("color: #b3b3b3; font-size: 14px;");
    topTracksLayout->addWidget(emptyLabel);
  }
}

void ArtistProfilePage::setAlbums(const QJsonArray &albums) {
  clearAlbums();

  // Use horizontal layout for albums
  QHBoxLayout *albumsHLayout = new QHBoxLayout();
  albumsHLayout->setSpacing(15);
  albumsHLayout->setContentsMargins(0, 0, 0, 0);

  for (const auto &val : albums) {
    QJsonObject album = val.toObject();
    int albumId = album["id"].toInt();
    QString title = album["title"].toString();

    // Create album card
    QWidget *albumCard = new QWidget(this);
    albumCard->setFixedSize(140, 180);
    albumCard->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *cardLayout = new QVBoxLayout(albumCard);
    cardLayout->setContentsMargins(10, 10, 10, 10);
    cardLayout->setSpacing(8);

    // Album cover placeholder
    QLabel *coverLabel = new QLabel(albumCard);
    coverLabel->setFixedSize(120, 120);
    coverLabel->setScaledContents(true);
    coverLabel->setStyleSheet("background-color: #282828; "
                              "border-radius: 6px;");
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setText("♪");

    // Album title
    QLabel *titleLabel = new QLabel(title, albumCard);
    titleLabel->setWordWrap(true);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedHeight(40);
    titleLabel->setStyleSheet("font-size: 12px; "
                              "color: #fff; "
                              "font-weight: 500;");

    cardLayout->addWidget(coverLabel);
    cardLayout->addWidget(titleLabel);

    albumCard->setStyleSheet("QWidget { "
                             "  background-color: rgba(255, 255, 255, 0.05); "
                             "  border-radius: 8px; "
                             "}"
                             "QWidget:hover { "
                             "  background-color: rgba(255, 255, 255, 0.1); "
                             "  margin-top: -2px; " // Subtle lift effect
                             "}");

    albumCard->setProperty("albumId", albumId);
    albumCard->installEventFilter(this);

    albumsHLayout->addWidget(albumCard);
  }

  albumsHLayout->addStretch();

  // Clear existing layout and set new one
  QLayoutItem *item;
  while ((item = albumsLayout->takeAt(0)) != nullptr) {
    delete item->widget();
    delete item;
  }

  QWidget *albumsWidget = new QWidget();
  albumsWidget->setLayout(albumsHLayout);
  albumsLayout->addWidget(albumsWidget);

  if (albums.isEmpty()) {
    QLabel *emptyLabel = new QLabel("No albums available", this);
    emptyLabel->setStyleSheet("color: #b3b3b3; font-size: 14px;");
    albumsLayout->addWidget(emptyLabel);
  }
}

void ArtistProfilePage::clearTopTracks() {
  QLayoutItem *item;
  while ((item = topTracksLayout->takeAt(0)) != nullptr) {
    delete item->widget();
    delete item;
  }
}

void ArtistProfilePage::clearAlbums() {
  QLayoutItem *item;
  while ((item = albumsLayout->takeAt(0)) != nullptr) {
    delete item->widget();
    delete item;
  }
}

bool ArtistProfilePage::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::MouseButtonPress) {
    QWidget *widget = qobject_cast<QWidget *>(obj);
    if (widget) {
      // Check if it's a track row
      QVariant trackIdVar = widget->property("trackId");
      if (trackIdVar.isValid()) {
        emit trackClicked(trackIdVar.toInt());
        return true;
      }

      // Check if it's an album card
      QVariant albumIdVar = widget->property("albumId");
      if (albumIdVar.isValid()) {
        emit albumClicked(albumIdVar.toInt());
        return true;
      }
    }
  }
  return QWidget::eventFilter(obj, event);
}
