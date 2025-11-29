#include "FavoriteCard.hpp"
#include <QMouseEvent>
#include <QVBoxLayout>

FavoriteCard::FavoriteCard(const QJsonObject &trackData, QWidget *parent)
    : QWidget(parent), m_trackData(trackData) {

  setFixedSize(120, 160); // Adjusted for 100x100 cover
  setCursor(Qt::PointingHandCursor);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(10, 10, 10, 10);
  layout->setSpacing(8);
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
  titleLabel->setStyleSheet("font-weight: bold; font-size: 13px; color: #fff;");
  titleLabel->setWordWrap(false);
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setMaximumWidth(110);

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
  unfavoriteButton->setFixedSize(24, 24);
  unfavoriteButton->setFlat(true);
  unfavoriteButton->setCursor(Qt::PointingHandCursor);
  unfavoriteButton->setStyleSheet(
      "QPushButton { color: #FFD700; border: none; font-size: 18px; }"
      "QPushButton:hover { color: #FFA500; }");
  unfavoriteButton->hide();

  connect(unfavoriteButton, &QPushButton::clicked, this,
          [this]() { emit unfavoriteClicked(getTrackId()); });

  layout->addWidget(coverLabel);
  layout->addWidget(textContainer);
  // layout->addWidget(unfavoriteButton); // Optional: add back if needed, or
  // make overlay

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
