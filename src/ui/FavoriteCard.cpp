#include "FavoriteCard.hpp"
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QVBoxLayout>

FavoriteCard::FavoriteCard(const QJsonObject &trackData, QWidget *parent)
    : QWidget(parent), m_trackData(trackData) {

  setFixedSize(120, 180); // Increased height for wrapped text
  setCursor(Qt::PointingHandCursor);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(10, 10, 10, 10);
  layout->setSpacing(5); // Reduced spacing
  layout->setAlignment(Qt::AlignCenter);

  // Cover art (larger, square)
  coverLabel = new QLabel(this);
  coverLabel->setFixedSize(100, 100);
  coverLabel->setScaledContents(true);
  coverLabel->setStyleSheet("background-color: #282828; border-radius: 6px;");
  coverLabel->setAlignment(Qt::AlignCenter);
  coverLabel->setText("♪");

  // Text container
  QWidget *textContainer = new QWidget(this);
  QVBoxLayout *textLayout = new QVBoxLayout(textContainer);
  textLayout->setContentsMargins(0, 0, 0, 0);
  textLayout->setSpacing(2);
  textLayout->setAlignment(Qt::AlignCenter); // Center text

  // Title
  QString title = trackData["title"].toString();
  titleLabel = new QLabel(title, this);
  titleLabel->setStyleSheet(
      "font-weight: bold; font-size: 11px; color: #fff;"); // Smaller font
  titleLabel->setWordWrap(true);                           // Enable word wrap
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setFixedHeight(40); // More height for wrapped text

  // Artist
  QString artist = trackData["artist"].toObject()["name"].toString();
  artistLabel = new QLabel(artist, this);
  artistLabel->setStyleSheet("font-size: 11px; color: #b3b3b3;");
  artistLabel->setWordWrap(false);
  artistLabel->setAlignment(Qt::AlignCenter);
  artistLabel->setMaximumWidth(110);

  textLayout->addWidget(titleLabel);
  textLayout->addWidget(artistLabel);

  // Unfavorite button (overlay or bottom? let's put it top-right overlay style
  // later, but for now maybe just hide it or put it in layout? User didn't
  // specify, but sketch didn't show it. Let's keep it hidden/hover logic but
  // maybe absolute pos? For simplicity, let's keep it in layout but maybe at
  // the bottom or top? Actually, let's make it an overlay if possible, or just
  // append to layout for now. Given the sketch, let's put it at the bottom for
  // now if needed, or rely on hover. The original code had it in the layout.
  // Let's add it to layout but center it.

  unfavoriteButton = new QPushButton("★", this);
  unfavoriteButton->setFixedSize(28, 28);
  unfavoriteButton->setFlat(true);
  unfavoriteButton->setCursor(Qt::PointingHandCursor);
  unfavoriteButton->setStyleSheet(
      "QPushButton { "
      "  color: #FFD700; "
      "  border: none; "
      "  font-size: 18px; "
      "  background-color: rgba(0, 0, 0, 0.5); " // Semi-transparent black by
                                                 // default
      "  border-radius: 14px; "
      "}"
      "QPushButton:hover { "
      "  color: #FFD700; "
      "  background-color: #000000; " // Full black on hover
      "}");

  // Position button on the top-right
  unfavoriteButton->move(92, 10); // 120 - 28 = 92 for right alignment
  unfavoriteButton->hide();       // Hidden by default, shown on card hover

  connect(unfavoriteButton, &QPushButton::clicked, this,
          [this]() { emit unfavoriteClicked(getTrackId()); });

  layout->addWidget(coverLabel);
  layout->addWidget(textContainer);

  // Card styling
  setStyleSheet(
      "FavoriteCard { background-color: #181818; border-radius: 8px; }"
      "FavoriteCard:hover { background-color: #282828; }");
}

void FavoriteCard::setCoverImage(const QPixmap &pixmap) {
  if (!pixmap.isNull()) {
    coverLabel->setPixmap(
        pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    coverLabel->setText("");
  }
}

void FavoriteCard::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit clicked(getTrackId());
  } else if (event->button() == Qt::RightButton) {
    // Context Menu
    QMenu contextMenu(this);
    contextMenu.setStyleSheet(
        "QMenu { background-color: #282828; color: #fff; border: 1px solid "
        "#333; }"
        "QMenu::item { padding: 5px 20px; }"
        "QMenu::item:selected { background-color: #333; }");

    QAction *addToAlbumAction = contextMenu.addAction("Add to Album");
    connect(addToAlbumAction, &QAction::triggered, this,
            [this]() { emit addToAlbumClicked(m_trackData); });

    contextMenu.exec(event->globalPosition().toPoint());
  }
  QWidget::mousePressEvent(event);
}

void FavoriteCard::setFavorite(bool isFavorite) {
  if (isFavorite) {
    unfavoriteButton->setText("★");
    unfavoriteButton->setStyleSheet("QPushButton { "
                                    "  color: #FFD700; "
                                    "  border: none; "
                                    "  font-size: 18px; "
                                    "  background-color: rgba(0, 0, 0, 0.5); "
                                    "  border-radius: 14px; "
                                    "}"
                                    "QPushButton:hover { "
                                    "  color: #FFD700; "
                                    "  background-color: #000000; "
                                    "}");
    // Disconnect previous connections to avoid duplicates or wrong logic
    unfavoriteButton->disconnect();
    connect(unfavoriteButton, &QPushButton::clicked, this,
            [this]() { emit unfavoriteClicked(getTrackId()); });

  } else {
    unfavoriteButton->setText("☆");
    unfavoriteButton->setStyleSheet("QPushButton { "
                                    "  color: #b3b3b3; "
                                    "  border: none; "
                                    "  font-size: 18px; "
                                    "  background-color: rgba(0, 0, 0, 0.5); "
                                    "  border-radius: 14px; "
                                    "}"
                                    "QPushButton:hover { "
                                    "  color: #FFD700; "
                                    "  background-color: #000000; "
                                    "}");
    unfavoriteButton->disconnect();
    connect(unfavoriteButton, &QPushButton::clicked, this,
            [this]() { emit favoriteToggled(m_trackData, true); });
  }
}

void FavoriteCard::enterEvent(QEnterEvent *event) {
  // Show star button when hovering over card
  unfavoriteButton->setVisible(true);
  QWidget::enterEvent(event);
}

void FavoriteCard::leaveEvent(QEvent *event) {
  // Hide star button when not hovering over card
  unfavoriteButton->hide();
  QWidget::leaveEvent(event);
}
