#include "bscl_md5.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage:\n%s [file]\n",argv[0]);
  }

  FILE *fp=fopen(argv[1],"rb");
  if (!fp) {
    perror("open fail.");
  }
  uint8_t buf[1024];
  bscl_md5_context_t ctx;
  bscl_md5_digest_t digest;
  bscl_md5_init(&ctx);
  while (!feof(fp)) {
    int len=fread(buf,1,sizeof(buf),fp);
    if (len > 0) {
      bscl_md5_accumulate(&ctx, buf, len) ;
    }
  }
  fclose(fp);
  bscl_md5_finish(&ctx, &digest);

  for (int i=0; i < sizeof(digest); ++i) {
    printf("%02x ",digest.digest[i]);
  }
  printf("\n");
  return 0;
}
