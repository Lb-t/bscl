#include "platform_audio.h"
#include "platform_IODevice.h"
#include "platform_queue.h"
#include "portaudio.h"
#include <limits.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLATFORM_AUDIO_BUF_NUM 2

typedef struct {
  platform_IODevice_t dev;
  PaStream *stream;
  uint16_t sampe_rate;
  uint16_t frame_size;
  void *readBuf[2];
  void *writeBuf[2];
  bool readBufStatus[2];
  bool writeBufStatus[2];
  sem_t *readable;
  sem_t *writable;
} platform_audioIO_t;

void platform_audio_onError(PaError err) {
  Pa_Terminate();
  fprintf(stderr, "An error occured while using the portaudio stream\n");
  fprintf(stderr, "Error number: %d\n", err);
  fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
}

void platform_audio_speakerEnable(bool action) {}
// enable or disable micphone
void platform_micphoneEnable(bool action) {
  // todo:
}
/*pause audio player*/
void platform_audio_pausePlayer(platform_audioIO_t *this) {}
/*resume audio player*/
void platform_audio_resumePlayer(platform_audioIO_t *this) {}

/*resume audio recorder*/
void platform_audio_resumeRecorder(platform_audioIO_t *this) {}

/*pause audio recorder*/
void platform_audio_pauseRecorder(platform_audioIO_t *this) {}

size_t platform_audio_write(platform_IODevice_t *dev, void *data, size_t len) {
  platform_audioIO_t *this = (platform_audioIO_t *)dev;
  size_t writeLen = len < this->frame_size * sizeof(int16_t)
                        ? len
                        : this->frame_size * sizeof(int16_t);
  static uint8_t index = 0;
  while (1) {
    sem_wait(this->writable);
    if (!this->writeBufStatus[index]) {
      memcpy(this->writeBuf[index], data, writeLen);
      this->writeBufStatus[index] = true;
      index = (index + 1) % PLATFORM_AUDIO_BUF_NUM;
      return writeLen;
    }
  }
  return 0;
}

int platform_audio_Callback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags, void *userData) {
  platform_audioIO_t *audioIO = (platform_audioIO_t *)userData;
  size_t len = framesPerBuffer < audioIO->frame_size
                   ? framesPerBuffer * sizeof(int16_t)
                   : audioIO->frame_size * sizeof(int16_t);
  static uint8_t readIndex = 0;
  static uint8_t writeIndex = 0;

  if (audioIO->writeBufStatus[writeIndex]) {
    memcpy(outputBuffer, audioIO->writeBuf[writeIndex], len);
    audioIO->writeBufStatus[writeIndex] = false;
    writeIndex = (writeIndex + 1) % PLATFORM_AUDIO_BUF_NUM;
    sem_post(audioIO->writable);
  } else {
    memset(outputBuffer, 0, len);
    // platform_abort();
  }

  if (!audioIO->readBufStatus[readIndex]) {
    memcpy(audioIO->readBuf[readIndex], inputBuffer, len);
    audioIO->readBufStatus[readIndex] = true;
    readIndex = (readIndex + 1) % PLATFORM_AUDIO_BUF_NUM;
    sem_post(audioIO->readable);
  } else {
    // pause
    // platform_abort();
    puts("audio record failed on callback.");
  }
  return 0;
}

// read audio data
size_t platform_audio_read(platform_IODevice_t *dev, void *data, size_t len) {
  static uint8_t prevReadBufIndex = 1;
  platform_audioIO_t *this = (platform_audioIO_t *)dev;
  size_t readout = 0;
  sem_wait(this->readable);
  prevReadBufIndex = (prevReadBufIndex + 1) % PLATFORM_AUDIO_BUF_NUM;
  if (this->readBufStatus[prevReadBufIndex]) {
    this->readBufStatus[prevReadBufIndex] = false;
    readout = len < this->frame_size * sizeof(int16_t)
                  ? len
                  : this->frame_size * sizeof(int16_t);
    memcpy(data, this->readBuf[prevReadBufIndex], readout);
  } else {
    // platform_abort();
	  puts("audio read failed.");
  }
  return readout;
}

/*initial audio IO*/
platform_IODevice_t *platform_audioIO_init(uint16_t sampe_rate,
                                       uint16_t frame_size) {
  static platform_audioIO_t audioIO;
  static sem_t readable_sem;
  static sem_t writable_sem;
  static bool isInit = false;

  // alow only one instance
  if (isInit)
    return NULL;

  audioIO.dev.read = platform_audio_read;
  audioIO.dev.write = platform_audio_write;
  audioIO.readable = &readable_sem;
  audioIO.writable = &writable_sem;
  audioIO.frame_size = frame_size;
  PaError err;
  /* Initialize our data for use by callback. */
  /* Initialize library before making any other calls. */
  err = Pa_Initialize();
  if (err != paNoError) {
    printf("portaudio init error.\n");
    platform_audio_onError(err);
  }
  /* Open an audio I/O stream. */
  err = Pa_OpenDefaultStream(&audioIO.stream, 1, /* no input channels */
                             1,                  /* stereo output */
                             paInt16, /* 32 bit floating point output */
                             sampe_rate, frame_size, /* frames per buffer */
                             platform_audio_Callback, &audioIO);
  if (err != paNoError) {
    printf("portaudio open stream error.\n");
    platform_audio_onError(err);
  }
  err = Pa_StartStream(audioIO.stream);
  if (err != paNoError)
    platform_audio_onError(err);
  int res = sem_init(audioIO.readable, 0, 0);
  assert(res == 0);
  res = sem_init(audioIO.writable, 0, 2);
  assert(res == 0);
  audioIO.readBuf[0] = malloc(frame_size * sizeof(int16_t));
  assert(audioIO.readBuf[0]);
  audioIO.readBuf[1] = malloc(frame_size * sizeof(int16_t));
  assert(audioIO.readBuf[1]);
  audioIO.writeBuf[0] = malloc(frame_size * sizeof(int16_t));
  assert(audioIO.writeBuf[0]);
  audioIO.writeBuf[1] = malloc(frame_size * sizeof(int16_t));
  assert(audioIO.writeBuf[1]);
  audioIO.readBufStatus[0] = false;
  audioIO.readBufStatus[1] = false;
  audioIO.writeBufStatus[0] = false;
  audioIO.writeBufStatus[1] = false;

  isInit = true;
  return (platform_IODevice_t *)&audioIO;
}

void platform_audio_deinit(platform_IODevice_t *dev) {
  platform_audioIO_t *audioIO = (platform_audioIO_t *)dev;
  PaError err;
  err = Pa_StopStream(audioIO->stream);
  if (err != paNoError) {
    platform_audio_onError(err);
    return;
  }
  err = Pa_CloseStream(audioIO->stream);
  if (err != paNoError) {
    platform_audio_onError(err);
    return;
  }
  Pa_Terminate();
}
