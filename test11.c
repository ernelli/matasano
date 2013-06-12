#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  char inbuf[256];
  unsigned char *dst, data[32768];
  char hex[256];
  int add, len;
  const char *seed = "jallapelle123skcsdkfsrdvh dshvrkgkvhsdk v d";

  //seed_random(seed, strlen(seed));

  random_bytes(data, 100);

  hexencode(data, 100, hex);
  hex[200] = '\0';
  printf("%s\n", hex);

  return 0;
}
