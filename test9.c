#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  char indata[32768];
  unsigned char data[32768];
  char hex[256];
  int len;
  unsigned char key[] = { 'Y', 'E', 'L', 'L', 'O', 'W', ' ', 'S', 'U', 'B', 'M', 'A', 'R', 'I', 'N', 'E' };

  fgets(indata, sizeof(indata), stdin);
  
  len = base64decode(indata, strlen(indata), data);

  aes_ecb_decrypt(data, len, key, sizeof(key));

  //remove padding
  strip_padding(data, len);

  printf("message: %s\n", data);
  hexdump(data, len);

  return 0;
}
