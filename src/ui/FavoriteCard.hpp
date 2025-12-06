#pragma once

#include <QJsonObject>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class FavoriteCard : public QWidget {
  Q_OBJECT

public:
  explicit FavoriteCard(const QJsonObject &trackData,
                        QWidget *parent = nullptr);

  void setCoverImage(const QPixmap &pixmap);
  int getTrackId() const { return m_trackData["id"].toInt(); }
  QJsonObject getTrackData() const { return m_trackData; }
  void setFavorite(bool isFavorite);

signals:
  void clicked(int trackId);
  void unfavoriteClicked(int trackId);
  void addToAlbumClicked(const QJsonObject &track);
  void favoriteToggled(const QJsonObject &track, bool isFavorite);
  void artistClicked(int artistId, const QString &artistName);

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;

private:
  QJsonObject m_trackData;
  QLabel *coverLabel;
  QLabel *titleLabel;
  QPushButton *artistLabel;
  QPushButton *unfavoriteButton;
};
