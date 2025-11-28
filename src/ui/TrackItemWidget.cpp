#include "TrackItemWidget.hpp"
#include <QStyle>

TrackItemWidget::TrackItemWidget(const QJsonObject &trackData, bool isFavorite,
                                 QWidget *parent)
    : QWidget(parent), m_trackData(trackData), m_isFavorite(isFavorite) {

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setContentsMargins(5, 5, 5, 5);

  QString title = trackData["title"].toString();
  QString artist = trackData["artist"].toObject()["name"].toString();

  titleLabel = new QLabel(title, this);
  titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

  artistLabel = new QLabel(artist, this);
  artistLabel->setStyleSheet("color: #888;");

  starButton = new QPushButton(this);
  starButton->setFixedSize(24, 24);
  starButton->setFlat(true);
  starButton->setCursor(Qt::PointingHandCursor);

  // Connect click signal
  connect(starButton, &QPushButton::clicked, this, [this]() {
    setFavorite(!m_isFavorite);
    emit favoriteToggled(m_isFavorite);
  });

  layout->addWidget(titleLabel);
  layout->addWidget(artistLabel);
  layout->addStretch();
  layout->addWidget(starButton);

  updateStarIcon();

  // Initial state: if not favorite, hide star
  if (!m_isFavorite) {
    starButton->hide();
  }
}

void TrackItemWidget::setFavorite(bool favorite) {
  m_isFavorite = favorite;
  updateStarIcon();

  // If favorite, always show. If not, only show if hovering (handled by
  // enter/leave)
  if (m_isFavorite) {
    starButton->show();
  } else if (!underMouse()) {
    starButton->hide();
  }
}

void TrackItemWidget::enterEvent(QEnterEvent *event) {
  Q_UNUSED(event);
  starButton->show();
}

void TrackItemWidget::leaveEvent(QEvent *event) {
  Q_UNUSED(event);
  if (!m_isFavorite) {
    starButton->hide();
  }
}

void TrackItemWidget::updateStarIcon() {
  if (m_isFavorite) {
    starButton->setText("★"); // Filled star
    starButton->setStyleSheet(
        "QPushButton { color: #FFD700; border: none; font-size: 20px; }");
  } else {
    starButton->setText("☆"); // Empty star
    starButton->setStyleSheet(
        "QPushButton { color: #888; border: none; font-size: 20px; } "
        "QPushButton:hover { color: #FFD700; }");
  }
}
