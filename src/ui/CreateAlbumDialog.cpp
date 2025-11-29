#include "CreateAlbumDialog.hpp"
#include <QStyle>

CreateAlbumDialog::CreateAlbumDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle("Create Playlist");
  setFixedSize(400, 300);
  setStyleSheet("background-color: #121212; color: #fff;");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(20);
  layout->setContentsMargins(30, 30, 30, 30);

  // Header
  QLabel *header = new QLabel("Create Playlist");
  header->setStyleSheet(
      "font-size: 24px; font-weight: bold; margin-bottom: 10px;");
  header->setAlignment(Qt::AlignCenter);
  layout->addWidget(header);

  // Name Input
  nameEdit = new QLineEdit(this);
  nameEdit->setPlaceholderText("Playlist Name");
  nameEdit->setStyleSheet("QLineEdit { "
                          "  background-color: #282828; "
                          "  border: 1px solid #333; "
                          "  border-radius: 4px; "
                          "  padding: 10px; "
                          "  font-size: 14px; "
                          "  color: #fff; "
                          "}"
                          "QLineEdit:focus { border: 1px solid #fff; }");
  layout->addWidget(nameEdit);

  // Cover Selection
  QHBoxLayout *coverLayout = new QHBoxLayout();

  coverPreview = new QLabel(this);
  coverPreview->setFixedSize(60, 60);
  coverPreview->setStyleSheet("background-color: #282828; border-radius: 4px; "
                              "border: 1px dashed #555;");
  coverPreview->setAlignment(Qt::AlignCenter);
  coverPreview->setText("No\nCover");

  selectCoverButton = new QPushButton("Upload Cover Image", this);
  selectCoverButton->setCursor(Qt::PointingHandCursor);
  selectCoverButton->setStyleSheet("QPushButton { "
                                   "  background-color: transparent; "
                                   "  border: 1px solid #555; "
                                   "  border-radius: 20px; "
                                   "  padding: 8px 16px; "
                                   "  color: #fff; "
                                   "  font-weight: bold; "
                                   "}"
                                   "QPushButton:hover { border-color: #fff; }");
  connect(selectCoverButton, &QPushButton::clicked, this,
          &CreateAlbumDialog::onSelectCoverClicked);

  coverLayout->addWidget(coverPreview);
  coverLayout->addSpacing(15);
  coverLayout->addWidget(selectCoverButton);
  coverLayout->addStretch();

  layout->addLayout(coverLayout);

  // Buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();

  cancelButton = new QPushButton("Cancel", this);
  cancelButton->setCursor(Qt::PointingHandCursor);
  cancelButton->setStyleSheet("QPushButton { "
                              "  background-color: transparent; "
                              "  color: #b3b3b3; "
                              "  font-weight: bold; "
                              "  border: none; "
                              "  font-size: 14px; "
                              "}"
                              "QPushButton:hover { color: #fff; }");
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

  createButton = new QPushButton("Create", this);
  createButton->setCursor(Qt::PointingHandCursor);
  createButton->setStyleSheet("QPushButton { "
                              "  background-color: #fff; "
                              "  color: #000; "
                              "  font-weight: bold; "
                              "  border-radius: 20px; "
                              "  padding: 10px 30px; "
                              "  font-size: 14px; "
                              "}"
                              "QPushButton:hover { background-color: #e6e6e6; "
                              "transform: scale(1.05); }");
  connect(createButton, &QPushButton::clicked, this, &QDialog::accept);

  buttonLayout->addWidget(cancelButton);
  buttonLayout->addSpacing(10);
  buttonLayout->addWidget(createButton);

  layout->addStretch();
  layout->addLayout(buttonLayout);
}

QString CreateAlbumDialog::getAlbumName() const { return nameEdit->text(); }

QString CreateAlbumDialog::getCoverPath() const { return selectedCoverPath; }

void CreateAlbumDialog::onSelectCoverClicked() {
  QString path = QFileDialog::getOpenFileName(this, "Select Cover Image", "",
                                              "Images (*.png *.jpg *.jpeg)");
  if (!path.isEmpty()) {
    selectedCoverPath = path;
    QPixmap pixmap(path);
    if (!pixmap.isNull()) {
      coverPreview->setPixmap(
          pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      coverPreview->setText("");
      selectCoverButton->setText("Change Cover");
    }
  }
}
