#pragma once

#include <QEvent>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class TrackItemWidget : public QWidget {
  Q_OBJECT

public:
  explicit TrackItemWidget(const QJsonObject &trackData, bool isFavorite,
                           QWidget *parent = nullptr);

  void setFavorite(bool favorite);
  bool isFavorite() const { return m_isFavorite; }
  QJsonObject getTrackData() const { return m_trackData; }

signals:
  void favoriteToggled(bool isFavorite);

protected:
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;

private:
  QJsonObject m_trackData;
  bool m_isFavorite;

  QLabel *titleLabel;
  QLabel *artistLabel;
  QPushButton *starButton;

  void updateStarIcon();
};
