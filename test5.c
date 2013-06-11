#include<stdio.h>
#include<string.h>

#include "base64.h"

int main(int argc, char *argv[]) {
  char data[256], outbuf[513];
  int len;
  unsigned char key[3] = { 'I', 'C', 'E' };

  len = fread(data, 1, sizeof(data), stdin);
  //printf("got %d bytes\n", len);
  xor_encrypt(data, key, len, 3);
  hexencode(data, outbuf, len);
  outbuf[2*len] = '\0';
  printf("%*s\n", len, outbuf);
  return 0;
}
