#include<stdio.h>
#include<string.h>
#include"tools.h"

int encryption_oracle(const unsigned char *in, int len, unsigned char *outbuff, int outlen);

int main(int argc, char *argv[]) {
  char *testdata = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  unsigned char ciphertext[256];

  int len;

  len = strlen(testdata);

  len = encryption_oracle(testdata, len, ciphertext, sizeof(ciphertext));

  if(detect_ecb(ciphertext, len, 16)) {
    printf("ECB\n");
  } else {
    printf("CBC\n");
  }

  return 0;
}
