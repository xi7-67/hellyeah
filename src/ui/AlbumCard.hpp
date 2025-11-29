#pragma once

#include <QHBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>

class AlbumCard : public QWidget {
  Q_OBJECT

public:
  explicit AlbumCard(const QJsonObject &albumData, QWidget *parent = nullptr);

  void setCoverImage(const QPixmap &pixmap);
  int getAlbumId() const { return m_albumData["id"].toInt(); }
  QJsonObject getAlbumData() const { return m_albumData; }

signals:
  void clicked(int albumId);
  void deleteClicked(int albumId);

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;

private:
  QJsonObject m_albumData;
  QLabel *coverLabel;
  QLabel *titleLabel;
  QLabel *artistLabel;
  QPushButton *deleteButton;
};
