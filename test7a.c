#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  char indata[] = "MyBloodyValentin";
  unsigned char data[32768];
  char hex[256], base64[256];
  int len;
  unsigned char key[] = { 'Y', 'E', 'L', 'L', 'O', 'W', ' ', 'S', 'U', 'B', 'M', 'A', 'R', 'I', 'N', 'E' };

  len = strlen(indata);

  printf("Plaintext:\n");
  hexdump(indata, len);
  
  memcpy(data, indata, len);

  aes_ecb_encrypt(data, len, key, sizeof(key));
  printf("Ciphertext:\n");
  hexdump(data, 16);

  base64[base64encode(data, len, base64)] = '\0';
  printf("base64: %s\n", base64);

  aes_ecb_decrypt(data, len, key, sizeof(key));
  data[len] = '\0';

  printf("Decrypted plaintext: %s\n", data);
  hexdump(data, len);

  return 0;
}
