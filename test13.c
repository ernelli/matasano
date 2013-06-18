#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  unsigned char *testdata = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  unsigned char testbuff[256];

  unsigned char ciphertext[256];
  unsigned char plaintext[256];

  unsigned char testblock0[64];
  unsigned char testblock1[64];
  
  unsigned char admin_block[16];
  
  int len;
  
  // test kv_parse, note that numbers are not quoted in JSON
  //kv_parse("foo=bar&baz=qux&zap=zazzle&zip=10&xap=11&jalla=foo", testbuff, sizeof(testbuff));
  //printf("JSON:\n\n%s", testbuff);

  
  // test the profile code using normal email

  //len = encrypt_profile("foo@bar.com", ciphertext, sizeof(ciphertext));
  //decrypt_profile(ciphertext, len, plaintext, sizeof(plaintext));

  // format of ciphertext
  // email=XXXX&uid=10&role=user

  // need block containing |admin\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb
  // put admin block aligned on 16 byte boundary

  //|               |               |
  //email=aaaaaaaaaaadmin...........&uid=10&role=user

  strcpy(testbuff, "admin");
  add_padding(testbuff, 5, 16);
  //hexdump(testbuff, 16);
  memmove(testbuff+10, testbuff, 16);
  memcpy(testbuff, testdata, 10);
  testbuff[26] = '\0';
  //hexdump(testbuff, strlen(testbuff));

  // attack and get admin encrypted
  len = encrypt_profile(testbuff, ciphertext, sizeof(ciphertext));
  // save admin with padding
  memcpy(admin_block, ciphertext+16, 16);

// attack step 2, create username so role=user alings user with last block
  //|               |               |               |
  //email=foooo@bar.com&uid=10&role=user

  len = encrypt_profile("foooo@bar.com", ciphertext, sizeof(ciphertext));
// replace user with admin
  memcpy(ciphertext+32, admin_block, 16);

  decrypt_profile(ciphertext, len, plaintext, sizeof(plaintext));
  printf("profile:\n%s\n", plaintext);
}
