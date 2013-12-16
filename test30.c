#include<stdio.h>
#include<string.h>

#include"tools.h"

int main(int argc, char *argv[]) {
  unsigned char data[1024*1024];
  unsigned char digest[16];
  int len;

  _md4(data, 0, digest);

  return 0;
}
