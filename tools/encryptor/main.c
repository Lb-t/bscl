#include <stdio.h>
#include <stdint.h>
#include "bscl_tea.h"

#pragma pack(push,1)
typedef struct {
  uint8_t sha256[32];
  uint8_t sign[32];
  unsigned char data[0]; 
}file_head_t;
#pragma pack(pop)

// encrypt file -k key -m method 
int main(int argc,char *argv[])
{
  if(argc<2){
    return -1;
  }
  char *iname=argv[1];
  char *oname=argv[2];

  FILE *fpi=fopen(iname,"rb");
  if (!fpi) {return -1;
  }
  FILE *fpo=fopen(iname,"wb");
  if (!fpo) {
    return -1;
  }

  static unsigned char buf[1024];
  file_head_t head;

  //todo:cacu sha 
  fwrite(&head,sizeof(head),1,fpo);
  while (!feof(fpi)) {
    int len =fread(buf,1,sizeof(buf),fpi);
    if (len > 0) {
      bscl_encryptData(buf, len);
      fwrite(buf,1,len,fpo);
    }
  }
  fclose(fpi);
  fclose(fpo);
}
