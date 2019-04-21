#include "bscl_wav.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
int bscl_wav_init_with_fp(bscl_wav_t *this, FILE *fp) {
  this->fp = fp;
  static bscl_wav_header_t head = {
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

void bscl_wav_finish(bscl_wav_t *this) {
  /*write wav head to the file*/
  fseek(this->fp, 0, SEEK_END);
  uint32_t len =
      ftell(this->fp) - offsetof(bscl_wav_header_t,chWAVE);
  uint32_t datalen = len - sizeof(bscl_wav_header_t) + offsetof(bscl_wav_header_t,chWAVE);

  fseek(this->fp, offsetof(bscl_wav_header_t,total_Len),
        SEEK_SET);
  fwrite(&len, sizeof(len), 1, this->fp);
  fseek(this->fp, offsetof(bscl_wav_header_t,DATALen), SEEK_SET);
  fwrite(&datalen, sizeof(datalen), 1, this->fp);
  this->fp = NULL;
}
