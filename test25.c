#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  unsigned char *key = "YELLOW SUBMARINE";
  unsigned char ciphertext[256], testbuff[256];
  unsigned char nonce[8];
  
  memset(nonce, 0, sizeof(nonce));

  memset(testbuff, 'A', sizeof(testbuff));
  memcpy(ciphertext, testbuff, sizeof(testbuff));
  aes_ctr_encrypt(ciphertext, sizeof(ciphertext), key, 16, nonce, 0, 0);

  hexdump(ciphertext, 256);

  int offset, len;

  unsigned char val;

  random_bytes(&val, 1);
  offset = val; 
  random_bytes(&val, 1);
  len = val;
  if(offset+len > sizeof(testbuff) - 1) {
    len = sizeof(testbuff) - offset - 1;
  }
  //printf("edit at %d len %d\n", offset, len);
  memset(testbuff, 'A', len);
  testbuff[len] = '\0';
  aes_ctr_edit(ciphertext, key, offset, testbuff);
  hexdump(ciphertext, 256);
}
