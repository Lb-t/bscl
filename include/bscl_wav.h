#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#pragma pack(push,1)
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
  char chDATA[4];           // data
  uint32_t DATALen;         //
} bscl_wav_header_t;

#pragma pack(pop)

typedef struct {
  FILE *fp;
  uint32_t numChannels;
  uint32_t totalSamples;
  uint16_t channels;
  uint16_t bitsPerSample;
} bscl_wav_t;

#define bscl_wav_seek(this, len)                                           \
  fseek((this)->fp, sizeof(WaveHeader) + (len), SEEK_SET)
#define bscl_wav_write(this, buf, len) fwrite((buf), 1, (len), (this)->fp)
#define bscl_wav_read(this, buf, len) fread((buf), 1, (len), (this)->fp)

int bscl_wav_init_with_fp(bscl_wav_t *this, FILE *fp);
void bscl_wav_finish(bscl_wav_t *this);
