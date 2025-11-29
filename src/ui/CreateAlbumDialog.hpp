#ifndef CREATE_ALBUM_DIALOG_HPP
#define CREATE_ALBUM_DIALOG_HPP

#include <QDialog>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class CreateAlbumDialog : public QDialog {
  Q_OBJECT

public:
  explicit CreateAlbumDialog(QWidget *parent = nullptr);

  QString getAlbumName() const;
  QString getCoverPath() const;

private slots:
  void onSelectCoverClicked();

private:
  QLineEdit *nameEdit;
  QLabel *coverPreview;
  QString selectedCoverPath;
  QPushButton *selectCoverButton;
  QPushButton *createButton;
  QPushButton *cancelButton;
};

#endif // CREATE_ALBUM_DIALOG_HPP
