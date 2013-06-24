#include<stdio.h>
#include<string.h>

#include "tools.h"

int main(int argc, char *argv[]) {
  char inbuf[256];
  char b64buf[256];
  char hexbuf[256];

  int len;

  unsigned char binbuf[256];

  fgets(inbuf, sizeof(inbuf), stdin);
  len = hexdecode(inbuf, binbuf);
  len = base64encode(binbuf, len, b64buf);
  b64buf[len] = '\0';
  printf("%s\n", b64buf);
  len = base64decode(b64buf, len, binbuf);
  hexencode(binbuf, len,  hexbuf);
  hexbuf[2*len] = '\0';
  printf("%s\n", hexbuf);

  return 0;
}
