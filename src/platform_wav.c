#include "platform_wav.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int platform_wav_init_with_fp(platform_wav_t *this, FILE *fp) {
  this->fp = fp;
  static platform_wav_header_t head = {
      .chRIFF = "RIFF",
      .chWAVE = "WAVE",
      .chFMT = "fmt ",
      .FMTLen = 16,
      .fmt_pcm = 1,
      .channels = 1,
      .fmt_sampingRate = 8000,
      .fmt_bytepsec = 8000 * 2,
      .fmt_bytesample = 2,
      .fmt_bitpsample = 16,
      .chDATA = "data",
  };
  fwrite(&head, sizeof(head), 1, fp);
  return 0;
}

void platform_wav_finish(platform_wav_t *this) {
  /*write wav head to the file*/
  fseek(this->fp, 0, SEEK_END);
  uint32_t len =
      ftell(this->fp) - (uint32_t) & ((platform_wav_header_t *)0)->chWAVE;
  uint32_t datalen = len - sizeof(platform_wav_header_t) + (uint32_t) &
                     ((platform_wav_header_t *)0)->chWAVE;

  fseek(this->fp, (uint32_t) & ((platform_wav_header_t *)0)->total_Len,
        SEEK_SET);
  fwrite(&len, sizeof(len), 1, this->fp);
  fseek(this->fp, (uint32_t) & ((platform_wav_header_t *)0)->DATALen, SEEK_SET);
  fwrite(&datalen, sizeof(datalen), 1, this->fp);
  this->fp = NULL;
}