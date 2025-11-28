#include "FavoriteCard.hpp"
#include <QMouseEvent>

FavoriteCard::FavoriteCard(const QJsonObject &trackData, QWidget *parent)
    : QWidget(parent), m_trackData(trackData) {

  setFixedSize(180, 240);
  setCursor(Qt::PointingHandCursor);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(10, 10, 10, 10);
  layout->setSpacing(5);

  // Cover art
  coverLabel = new QLabel(this);
  coverLabel->setFixedSize(160, 160);
  coverLabel->setScaledContents(true);
  coverLabel->setStyleSheet("background-color: #282828; border-radius: 8px;");
  coverLabel->setAlignment(Qt::AlignCenter);
  coverLabel->setText("♪"); // Placeholder

  // Title
  QString title = trackData["title"].toString();
  titleLabel = new QLabel(title, this);
  titleLabel->setStyleSheet("font-weight: bold; font-size: 13px; color: #fff;");
  titleLabel->setWordWrap(true);
  titleLabel->setMaximumHeight(40);

  // Artist
  QString artist = trackData["artist"].toObject()["name"].toString();
  artistLabel = new QLabel(artist, this);
  artistLabel->setStyleSheet("font-size: 11px; color: #b3b3b3;");
  artistLabel->setWordWrap(true);

  // Unfavorite button (hidden by default)
  unfavoriteButton = new QPushButton("★", this);
  unfavoriteButton->setFixedSize(30, 30);
  unfavoriteButton->setStyleSheet(
      "QPushButton { background-color: rgba(0,0,0,0.7); color: #FFD700; "
      "border: none; border-radius: 15px; font-size: 18px; }"
      "QPushButton:hover { background-color: rgba(0,0,0,0.9); }");
  unfavoriteButton->hide();
  unfavoriteButton->move(140, 10); // Top right corner
  unfavoriteButton->raise();

  connect(unfavoriteButton, &QPushButton::clicked, this,
          [this]() { emit unfavoriteClicked(getTrackId()); });

  layout->addWidget(coverLabel);
  layout->addWidget(titleLabel);
  layout->addWidget(artistLabel);
  layout->addStretch();

  // Card styling
  setStyleSheet(
      "FavoriteCard { background-color: #181818; border-radius: 8px; }"
      "FavoriteCard:hover { background-color: #282828; }");
}

void FavoriteCard::setCoverImage(const QPixmap &pixmap) {
  if (!pixmap.isNull()) {
    coverLabel->setPixmap(
        pixmap.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    coverLabel->setText("");
  }
}

void FavoriteCard::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit clicked(getTrackId());
  }
  QWidget::mousePressEvent(event);
}

void FavoriteCard::enterEvent(QEnterEvent *event) {
  unfavoriteButton->show();
  QWidget::enterEvent(event);
}

void FavoriteCard::leaveEvent(QEvent *event) {
  unfavoriteButton->hide();
  QWidget::leaveEvent(event);
}
