#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  const char indata[] = "MyBloodyValentinMyBloodyValentin";
  unsigned char data[32768];
  char hex[256], base64[256];
  int len;

  unsigned char iv[] = { 'Y', 'E', 'L', 'L', 'O', 'W', ' ', 'S', 'U', 'B', 'M', 'A', 'R', 'I', 'N', 'E' };
  unsigned char key[] = { 'Y', 'E', 'L', 'L', 'O', 'W', ' ', 'S', 'U', 'B', 'M', 'A', 'R', 'I', 'N', 'E' };

  len = strlen(indata);
  printf("// testing aes-128-ecb\n");

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

  printf("// testing aes-128-cbc\n");

  printf("Plaintext:\n");
  hexdump(indata, len);
  
  memcpy(data, indata, len);

  aes_cbc_encrypt(data, len, key, sizeof(key), iv);
  printf("Ciphertext:\n");
  hexdump(data, 16);

  base64[base64encode(data, len, base64)] = '\0';
  printf("base64: %s\n", base64);

  aes_cbc_decrypt(data, len, key, sizeof(key), iv);
  data[len] = '\0';

  printf("Decrypted plaintext: %s\n", data);
  hexdump(data, len);



  return 0;
}
