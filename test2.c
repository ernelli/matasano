#include<stdio.h>
#include<string.h>

#include "base64.h"

int main(int argc, char *argv[]) {
  char inbuf1[256];
  char inbuf2[256];
  int i, len;

  unsigned char bin1[256];
  unsigned char bin2[256];
  unsigned char res[256];
  char outbuf[256];

  if(!fgets(inbuf1, sizeof(inbuf1), stdin))  {
    fprintf(stderr, "failed to read data");
    return 1;
  }
  if(!fgets(inbuf2, sizeof(inbuf2), stdin)) {
    fprintf(stderr, "failed to read data");
    return 1;
  }

  len = strlen(inbuf1);
  if(len != strlen(inbuf2)) {
    fprintf(stderr, "invalid input");
    return 2;
  }

  len--;

  if(inbuf1[len] == '\n') {
    inbuf1[len] = '\0';
  } 

  if(inbuf2[len] == '\n') {
    inbuf2[len] = '\0';
  }

  
  if(len & 1) {
    fprintf(stderr, "invalid input, odd length: %d", len);
    return 2;
  }
  
  len /= 2;

  hexdecode(inbuf1, bin1);
  hexdecode(inbuf2, bin2);
  
  for(i = 0; i < len; i++) {
    res[i] = bin1[i] ^ bin2[i];
  }
  
  hexencode(res, outbuf, len);
  outbuf[2*len] = '\0';
  printf("%s\n", outbuf);
}
