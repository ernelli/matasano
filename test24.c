#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  unsigned short key;
  unsigned char data[256], testbuff[256];
  const char *plaintext = "AAAAAAAAAAAAAA";
  int i, len;

  random_bytes((unsigned char *)&len, sizeof(len));

  len &= 0x7fffffff;

  len = 8 + len % 56;
  random_bytes(data, len);
  strcpy(data+len, plaintext);
  len += strlen(plaintext);

  random_bytes((unsigned char *)&key, sizeof(key));
  
  mt19937_encrypt(data, len, key);

  // brute force key 
  for(i = 0; i < 65536; i++) {
    memcpy(testbuff, data, len);
    mt19937_decrypt(testbuff, len, i);
    if(memcmp(testbuff + len - 14, plaintext, 14) == 0) {
      printf("found key: %d\n", i);
      break;
    }
  }
}
