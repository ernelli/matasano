#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  char inbuf[256];
  unsigned char *dst, data[32768];
  char hex[256];
  int add, len;

  unsigned char iv[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  unsigned char key[] = { 'Y', 'E', 'L', 'L', 'O', 'W', ' ', 'S', 'U', 'B', 'M', 'A', 'R', 'I', 'N', 'E' };

  dst = data;

  while(fgets(inbuf, sizeof(inbuf), stdin)) {
    len = strlen(inbuf);
    if(inbuf[len-1] == '\n') {
      len--;
    }
    dst += (add = base64decode(inbuf, len, dst));
  }

  len = dst - data;

  //printf("indata len: %d\n", len);
  //hexdump(data, len);

  aes_cbc_decrypt(data, len, key, sizeof(key), iv);

  //remove padding
  strip_padding(data, len);

  printf("message: %s\n", data);
  //  hexdump(data, len);

  return 0;
}
