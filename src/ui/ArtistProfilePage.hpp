#pragma once

#include <QEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

class ArtistProfilePage : public QWidget {
  Q_OBJECT

public:
  explicit ArtistProfilePage(QWidget *parent = nullptr);

  void setArtistData(const QJsonObject &artistData);
  void setTopTracks(const QJsonArray &tracks);
  void setAlbums(const QJsonArray &albums);
  void setArtistCover(const QPixmap &cover);

signals:
  void backClicked();
  void trackClicked(int trackId);
  void albumClicked(int albumId);

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

private:
  // Header
  QLabel *artistCoverLabel;
  QLabel *artistNameLabel;
  QLabel *bioLabel;
  QPushButton *backButton;

  // Top Tracks section
  QWidget *topTracksContainer;
  QVBoxLayout *topTracksLayout;

  // Albums section
  QScrollArea *albumsScrollArea;
  QWidget *albumsContainer;
  QVBoxLayout *albumsLayout;

  // Data
  QJsonObject m_artistData;

  void setupUI();
  void clearTopTracks();
  void clearAlbums();
};
