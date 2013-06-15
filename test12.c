#include<stdio.h>
#include<string.h>
#include"tools.h"

int encryption_oracle_ecb(const unsigned char *in, int len, unsigned char *outbuff, int outlen);

int main(int argc, char *argv[]) {
  char *testdata = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  unsigned char testbuff[256];

  unsigned char ciphertext[256];
  unsigned char plaintext[256];

  unsigned char testblock0[64];
  unsigned char testblock1[64];

  char hex0[128];
  char hex1[128];

  int len, blocklen;

  int i, j, k;

  for(i = 3; i < 32; i++) {
    len = encryption_oracle_ecb(testdata, i, ciphertext, sizeof(ciphertext));
    memcpy(testblock1, testblock0, sizeof(testblock1));
    memcpy(testblock0, ciphertext, sizeof(testblock1));

    /*
    printf("Testblocks at len: %d\n", i);
    hexencode(testblock0, 32, hex0);
    hexencode(testblock1, 32, hex1);
    hex0[64] = '\0';
    hex1[64] = '\0';
    printf("testblock0: %s\ntestblock1: %s\n", hex0, hex1);
    */

    if(!memcmp(testblock0, testblock1, i-1)) {
      blocklen = i - 1;
      printf("blocksize: %d\n", blocklen);
    }
  }

  len = strlen(testdata);

  len = encryption_oracle_ecb(testdata, len, ciphertext, sizeof(ciphertext));

  if(!detect_ecb(ciphertext, len, blocklen)) {
    printf("Cannot decrypt non ECB encoded data\n");
    return 0;
  }

  printf("ECB detected, decrypting secret data...\n");

  k = 0;
  for(i = 15; i >=0; i--) {
    // encode first ECB block with known data + part of ciphertext
    len = encryption_oracle_ecb(testdata, i, ciphertext, sizeof(ciphertext));

    memcpy(testblock0, ciphertext, 16);

    //printf("testblock0:\n");
    //hexdump(testblock0, 16);

    memcpy(testblock1, testdata, i);
    memcpy(testblock1+i, plaintext, k);
    
    //find the next character
    for(j = 0; j < 256; j++) {
      testblock1[15] = j;
      
      encryption_oracle_ecb(testblock1, 16, ciphertext, sizeof(ciphertext));
      if(!memcmp(testblock0, ciphertext, 16)) { // found next plaintext
        plaintext[k++] = j;
        break;
      }
    }
    if(j == 256) {
      printf("Failed to find plaintext character at index: %d!\n", k);
      return 1;
    }
  }

  printf("found plaintext: %.16s\n", plaintext);

  return 0;
}
