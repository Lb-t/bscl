#ifndef INCLUDE_PLATFORM_WAV_H_
#define INCLUDE_PLATFORM_WAV_H_
#include <stdint.h>
#include <stdio.h>

typedef struct {
  char chRIFF[4];           // "RIFF"
  uint32_t total_Len;       // file len
  char chWAVE[4];           // "WAVE"
  char chFMT[4];            // "fmt"
  uint32_t FMTLen;          // 0x10
  uint16_t fmt_pcm;         // 1 for pcm
  uint16_t channels;        // 1
  uint32_t fmt_sampingRate; // 8000
  uint32_t fmt_bytepsec;    // 1*8000*16/8
  uint16_t fmt_bytesample;  // 16
  uint16_t fmt_bitpsample;  // 16
  char chDATA[4];           // ＂data ＂
  uint32_t DATALen;         //
} platform_wav_head_t;

typedef struct { platform_wav_head_t *head; } platform_wav_t;
platform_wav_t *platform_wav_new(char *file_name);
void platform_wav_flush(platform_wav_t *this);
void platform_wav_delete(platform_wav_t *this);
void platform_wav_write(void *buf, size_t len);
uint16_t platform_wav_get_samping_rate(platform_wav_t *this);

#endif
