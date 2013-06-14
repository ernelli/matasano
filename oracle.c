#include <stdio.h>
#include <string.h>
#include "tools.h"

int encryption_oracle(const unsigned char *in, int len, unsigned char *outbuff, int outlen) {
  unsigned char prepend[11];
  unsigned char append[11];
  unsigned char key[16];
  unsigned char iv[16];
  int i, j, I;

  if(len + 20 > outlen) {
    return 0;
  }
  random_bytes(prepend, 11);
  random_bytes(append, 11);
  random_bytes(key, 16);
  random_bytes(iv, 16);

  for(i = 0; i < 5; i++) {
    outbuff[i] = prepend[i];
  }
  I = 5 + prepend[10] % 6;
  while(i < I) {
    outbuff[i] = prepend[i];
    i++;
  }
  memcpy(outbuff + i, in, len);
  j = i + len;
  
  for(i = 0; i < 5; i++) {
    outbuff[j++] = append[i];
  }
  I = 5 + append[10] % 6;
  while(i < I) {
    outbuff[j++] = append[i++];
  }

  len = add_padding(outbuff, j, 16);
  
  if(prepend[0] & 1) {
    aes_ecb_encrypt(outbuff, len, key, 16);
  } else {
    aes_cbc_encrypt(outbuff, len, key, 16, iv);
  }
  return len;
}
