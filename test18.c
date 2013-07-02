#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  unsigned char data[1024], testblock[1024], plaintext[1024], nonce[16];
  int len;

  char *ciphertext = "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==";

  len = base64decode(ciphertext, strlen(ciphertext), data);

  memset(nonce, 0, 16);

  aes_ctr_decrypt(data, len, "YELLOW SUBMARINE", 16, nonce, 0, 0);

  data[len] = '\0';

  //hexdump(data, len);
  printf("%s\n", data);

  return 0;
}
