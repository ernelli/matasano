#include<stdio.h>

#include"tools.h"

int main(int argc, char *argv[]) {
  unsigned char data[1024*1024];
  unsigned char digest[20];
  int len;


  len = fread(data, 1, sizeof(data), stdin);

  sha1(data, len, digest);

  hexdump(digest, 20);

  return 0;
}
