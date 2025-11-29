#include "AlbumCard.hpp"
#include <QMouseEvent>
#include <QVBoxLayout>

AlbumCard::AlbumCard(const QJsonObject &albumData, QWidget *parent)
    : QWidget(parent), m_albumData(albumData) {

  setFixedHeight(80);
  setCursor(Qt::PointingHandCursor);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setContentsMargins(10, 10, 10, 10);
  layout->setSpacing(15);

  // Cover art
  coverLabel = new QLabel(this);
  coverLabel->setFixedSize(60, 60);
  // coverLabel->setScaledContents(true); // Removed to prevent stretching
  coverLabel->setStyleSheet("background-color: #282828; border-radius: 4px;");
  coverLabel->setAlignment(Qt::AlignCenter);
  coverLabel->setText("♪"); // Placeholder

  // Text container
  QWidget *textContainer = new QWidget(this);
  QVBoxLayout *textLayout = new QVBoxLayout(textContainer);
  textLayout->setContentsMargins(0, 0, 0, 0);
  textLayout->setSpacing(2);
  textLayout->setAlignment(Qt::AlignVCenter);

  // Title
  QString title = albumData["title"].toString();
  titleLabel = new QLabel(title, this);
  titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #fff;");

  // Artist
  QString artist = albumData["artist"].toObject()["name"].toString();
  artistLabel = new QLabel(artist, this);
  artistLabel->setStyleSheet("font-size: 12px; color: #b3b3b3;");

  textLayout->addWidget(titleLabel);
  textLayout->addWidget(artistLabel);

  layout->addWidget(coverLabel);
  layout->addWidget(textContainer);
  layout->addStretch();

  // Delete button (trash icon)
  deleteButton = new QPushButton(this);
  deleteButton->setText("×");
  deleteButton->setFixedSize(24, 24);
  deleteButton->setFlat(true);
  deleteButton->setCursor(Qt::PointingHandCursor);
  deleteButton->setStyleSheet("QPushButton { color: #888; border: none; "
                              "font-size: 20px; font-weight: bold; } "
                              "QPushButton:hover { color: #f44336; }");
  deleteButton->setToolTip("Delete Album");
  deleteButton->hide();

  connect(deleteButton, &QPushButton::clicked, this,
          [this]() { emit deleteClicked(getAlbumId()); });

  layout->addWidget(deleteButton);

  // Card styling
  setStyleSheet("AlbumCard { background-color: #181818; border-radius: 8px; }"
                "AlbumCard:hover { background-color: #282828; }");
}

void AlbumCard::setCoverImage(const QPixmap &pixmap) {
  if (!pixmap.isNull()) {
    coverLabel->setPixmap(pixmap.scaled(60, 60, Qt::KeepAspectRatioByExpanding,
                                        Qt::SmoothTransformation));
    coverLabel->setText("");
  }
}

void AlbumCard::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit clicked(getAlbumId());
  }
  QWidget::mousePressEvent(event);
}

void AlbumCard::enterEvent(QEnterEvent *event) {
  deleteButton->setVisible(true);
  QWidget::enterEvent(event);
}

void AlbumCard::leaveEvent(QEvent *event) {
  deleteButton->hide();
  QWidget::leaveEvent(event);
}
