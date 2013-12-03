#include<stdio.h>
#include<string.h>

#include"tools.h"

unsigned char secret_key[128];
int secret_key_len = 0;

void init_key() {
  unsigned char binkey[64];
  random_bytes((unsigned char *)&secret_key_len, sizeof(secret_key_len));
  secret_key_len &= 0x7fffffff;
  secret_key_len = (16 + secret_key_len % 16);
  
  random_bytes(binkey, secret_key_len);
  // secret_key is null terminated
  hexencode(binkey, secret_key_len, secret_key);
  //  printf("key_len: %d\n", secret_key_len);
  //printf("%s\n", secret_key);
}

/*
void generate_mac(const char *data, const char *key, unsigned char *mac) {
  unsigned int h[5];
  sha1_init(h);
  sha1_update(key, h);
  sha1_update(data);
  sha1_finish(mac);

}
*/
int main(int argc, char *argv[]) {
  unsigned char mac[20];
  char buff[128];
  char tamp[256];
  const char *data = "comment1=cooking%20MCs;userdata=foo;comment2=%20like%20a%20pound%20of%20bacon";

  init_key();


  generate_mac(data, strlen(data), secret_key, secret_key_len, mac);
  hexencode(mac, 20, buff);
  buff[40] = '\0';
  printf("digest: %s\n", buff);
  printf("mac valid: %d\n", validate_mac(data, strlen(data), secret_key, secret_key_len, mac));
  strcpy(tamp, data);
  strcat(tamp, ";admin=true");
  printf("tampered msg mac valid: %d\n", validate_mac(tamp, strlen(tamp), secret_key, secret_key_len, mac));
  
  return 0;
}
