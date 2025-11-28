#include "api/HifiClient.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  HifiClient client;

  QObject::connect(
      &client, &HifiClient::searchResults, [&](const QJsonArray &tracks) {
        qDebug() << "Search successful! Found" << tracks.size() << "tracks.";
        if (!tracks.isEmpty()) {
          QJsonObject first = tracks[0].toObject();
          qDebug() << "First track:" << first["title"].toString();
          // Try to get stream for first track
          int id = first["id"].toInt();
          qDebug() << "Fetching stream for track" << id;
          client.getTrackStream(id);
        } else {
          app.quit();
        }
      });

  QObject::connect(&client, &HifiClient::trackStreamUrl,
                   [&](const QString &url) {
                     qDebug() << "Stream URL retrieved:" << url;
                     app.quit();
                   });

  QObject::connect(&client, &HifiClient::errorOccurred,
                   [&](const QString &msg) {
                     qDebug() << "Error:" << msg;
                     // Don't quit immediately on error, client might be
                     // rotating endpoints
                   });

  // Timeout after 10 seconds
  QTimer::singleShot(10000, &app, [&]() {
    qDebug() << "Timeout reached.";
    app.quit();
  });

  qDebug() << "Starting search for 'The Weeknd'...";
  client.searchTracks("The Weeknd");

  return app.exec();
}
