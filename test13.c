#include<stdio.h>
#include<string.h>
#include"tools.h"

int encryption_oracle_ecb(const unsigned char *in, int len, unsigned char *outbuff, int outlen);

int main(int argc, char *argv[]) {
  unsigned char *testdata = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  unsigned char testbuff[256];

  unsigned char ciphertext[256];
  unsigned char plaintext[256];

  unsigned char testblock0[64];
  unsigned char testblock1[64];
  
  int len;
  
  //kv_parse("foo=bar&baz=qux&zap=zazzle&zip=10&xap=11&jalla=foo", testbuff, sizeof(testbuff));
  //printf("JSON:\n\n%s", testbuff);

  len = encrypt_profile("foo@bar.com", ciphertext, sizeof(ciphertext));
  decrypt_profile(ciphertext, len, plaintext, sizeof(plaintext));
  printf("profile:\n%s\n", plaintext);
}
