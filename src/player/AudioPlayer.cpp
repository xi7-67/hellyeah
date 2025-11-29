#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "AudioPlayer.hpp"
#include <QDebug>
#include <QDir>
#include <QTimer>

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent), currentReply(nullptr), tempFile(nullptr),
      engine(nullptr), sound(nullptr), isEngineInitialized(false),
      isSoundInitialized(false), m_volume(1.0f) {

  manager = new QNetworkAccessManager(this);
  positionTimer = new QTimer(this);
  positionTimer->setInterval(100); // Update every 100ms
  connect(positionTimer, &QTimer::timeout, this, &AudioPlayer::onPositionTimer);

  initMiniaudio();
}

AudioPlayer::~AudioPlayer() {
  cleanupMiniaudio();
  if (tempFile) {
    QString path = tempFile->fileName();
    delete tempFile;
    QFile::remove(path);
  }
}

void AudioPlayer::initMiniaudio() {
  engine = new ma_engine;
  ma_result result = ma_engine_init(NULL, engine);
  if (result != MA_SUCCESS) {
    qCritical() << "Failed to initialize audio engine.";
    return;
  }
  isEngineInitialized = true;

  sound = new ma_sound;
}

void AudioPlayer::cleanupMiniaudio() {
  if (isSoundInitialized) {
    ma_sound_uninit(sound);
    isSoundInitialized = false;
  }

  if (isEngineInitialized) {
    ma_engine_uninit(engine);
    delete engine;
    isEngineInitialized = false;
  }

  delete sound;
}

void AudioPlayer::playUrl(const QString &url) {
  stop(); // Stop current playback

  if (currentReply) {
    currentReply->abort();
    currentReply->deleteLater();
    currentReply = nullptr;
  }

  if (tempFile) {
    delete tempFile;
    tempFile = nullptr;
  }

  // Create a temp file
  tempFile = new QTemporaryFile(this);
  if (!tempFile->open()) {
    emit errorOccurred("Could not create temporary file.");
    return;
  }

  qDebug() << "Downloading to temp file:" << tempFile->fileName();

  QNetworkRequest request((QUrl(url)));
  currentReply = manager->get(request);

  connect(currentReply, &QNetworkReply::finished, this,
          &AudioPlayer::onDownloadFinished);
  connect(currentReply, &QNetworkReply::downloadProgress, this,
          &AudioPlayer::onDownloadProgress);
}

void AudioPlayer::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
  if (tempFile && currentReply) {
    QByteArray data = currentReply->readAll();
    tempFile->write(data);
  }
}

void AudioPlayer::onDownloadFinished() {
  if (!currentReply)
    return;

  if (currentReply->error() == QNetworkReply::NoError) {
    // Write any remaining data
    QByteArray data = currentReply->readAll();
    tempFile->write(data);
    tempFile->flush();
    tempFile->close(); // Close to flush to disk, but QTemporaryFile keeps it
                       // until deleted

    qDebug() << "Download finished. Playing...";

    if (isSoundInitialized) {
      ma_sound_uninit(sound);
      isSoundInitialized = false;
    }

    // Load sound from file
    // Note: QTemporaryFile might need to be kept open or name used.
    // ma_engine_play_sound_from_file takes a path.

    ma_result result = ma_sound_init_from_file(
        engine, tempFile->fileName().toUtf8().constData(), 0, NULL, NULL,
        sound);
    if (result != MA_SUCCESS) {
      emit errorOccurred("Failed to load sound file.");
      return;
    }

    isSoundInitialized = true;
    ma_sound_set_volume(sound, m_volume); // Apply stored volume
    ma_sound_start(sound);
    positionTimer->start();
    emit durationChanged(duration());

  } else {
    emit errorOccurred("Download error: " + currentReply->errorString());
  }

  currentReply->deleteLater();
  currentReply = nullptr;
}

void AudioPlayer::play() {
  if (isSoundInitialized) {
    ma_sound_start(sound);
    positionTimer->start();
  }
}

void AudioPlayer::pause() {
  if (isSoundInitialized) {
    ma_sound_stop(sound);
    positionTimer->stop();
  }
}

void AudioPlayer::stop() {
  if (isSoundInitialized) {
    ma_sound_stop(sound);
    ma_sound_seek_to_pcm_frame(sound, 0);
    positionTimer->stop();
    emit positionChanged(0);
  }
}

void AudioPlayer::seek(qint64 positionMs) {
  if (isSoundInitialized) {
    ma_uint32 sampleRate;
    ma_sound_get_data_format(sound, NULL, NULL, &sampleRate, NULL, 0);
    ma_uint64 frameIndex = (positionMs * sampleRate) / 1000;
    ma_sound_seek_to_pcm_frame(sound, frameIndex);
    emit positionChanged(positionMs);
  }
}

qint64 AudioPlayer::position() const {
  if (isSoundInitialized) {
    ma_uint64 frameIndex;
    ma_sound_get_cursor_in_pcm_frames(sound, &frameIndex);

    ma_uint32 sampleRate;
    ma_sound_get_data_format(sound, NULL, NULL, &sampleRate, NULL, 0);

    if (sampleRate > 0) {
      return (frameIndex * 1000) / sampleRate;
    }
  }
  return 0;
}

qint64 AudioPlayer::duration() const {
  if (isSoundInitialized) {
    ma_uint64 lengthInFrames;
    ma_sound_get_length_in_pcm_frames(sound, &lengthInFrames);

    ma_uint32 sampleRate;
    ma_sound_get_data_format(sound, NULL, NULL, &sampleRate, NULL, 0);

    if (sampleRate > 0) {
      return (lengthInFrames * 1000) / sampleRate;
    }
  }
  return 0;
}

void AudioPlayer::onPositionTimer() {
  if (isSoundInitialized && ma_sound_is_playing(sound)) {
    emit positionChanged(position());
  }
}

void AudioPlayer::setVolume(float volume) {
  m_volume = volume; // Store it
  if (isSoundInitialized) {
    ma_sound_set_volume(sound, volume);
  }
}
