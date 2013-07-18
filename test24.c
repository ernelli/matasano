#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  unsigned short key;
  unsigned char data[256], testbuff[256];
  const char *plaintext = "AAAAAAAAAAAAAA";
  int i, len;

  // generate a 31 bit positive random integer
  random_bytes((unsigned char *)&len, sizeof(len));
  len &= 0x7fffffff;

  // make it a random integer between 8 and 64 bytes (biased)
  len = 8 + len % 56;

  // produce ranom length bytes
  random_bytes(data, len);

  // append plaintext
  strcpy(data+len, plaintext);
  len += strlen(plaintext);

  // generate a random "key" using a 16 bit random seed
  random_bytes((unsigned char *)&key, sizeof(key));
  mt19937_encrypt(data, len, key);

  // brute force key 
  for(i = 0; i < 65536; i++) {
    memcpy(testbuff, data, len);
    mt19937_decrypt(testbuff, len, i);
    // when the known plaintext is detected at the end of the buffer, the "key" is found
    if(memcmp(testbuff + len - 14, plaintext, 14) == 0) {
      printf("found key: %d\n", i);
      break;
    }
  }
  return 0;
}
