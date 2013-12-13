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
  unsigned char tamp[2048];
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

  // now try to bruteforce a valid message by continuing the existing mac.
  //
  // valid mac is based on sha1(key || msg) which is based on (key || msg || padding0)

  // we want sha1(key || msg || padding0 || tampering || padding1) to be valid, 
  // for that we need to guess key length and append tampering and continue
  // the sha1 hash function over padding and tampering.

  // 
  
  // padding = | 0x80 0000 ... 0000 LEN | where 
  // 

  // tampered mac

  unsigned char tampmac[20];

  int msglen = strlen(data);
  int keylen = secret_key_len; // start with a zero length key  
  int valid;

  do {

  // the sha1 state we will continue on
  unsigned int h[5];
  int i;
  for(i = 0; i < 5; i++) {
    h[i] = (mac[i*4] << 24) | (mac[i*4+1] << 16) | (mac[i*4+2] << 8) | mac[i*4+3];
    //    printf("h[%d] = %08X\n", i, h[i]);
  } 


  strcpy(tamp, ";admin=true");

  int key_data_padding0_len =  ((keylen + msglen + 9) + 64 - (keylen + msglen + 9) % 64);
  int tamp_len = strlen(tamp);

  // the length of the tampered message, that is msglen + keylen + padding + tamp

  // calculate tampered mac
  printf("-----------------------------------\ngenerating tampered mac using total length: %d, key_data_padding: %d\n", key_data_padding0_len + tamp_len, key_data_padding0_len);

  sha1_finish(tamp, key_data_padding0_len + tamp_len, h, tampmac);
  printf("tampered mac:\n");
  hexdump(tampmac, 20);
  
  // generate tampered message, that is, original message + padding0 + tampered data
  strcpy(tamp, data);

  // append padding0 after original message
  
  unsigned char *p = tamp+msglen;

  *p++ = 0x80;
  // nullpadd until 8 bytes from block boundary
  while( (p - (unsigned char *)tamp) % 64 != 56) {
    *p++ = 0;
  }

  // first 32 bits of 64 bit length indicator is zero
  *p++ = 0;
  *p++ = 0;
  *p++ = 0;
  *p++ = 0;
  
  unsigned int bits = 8*(msglen + keylen);

  // append the 32 bit length field
  *p++ = bits >> 24;
  *p++ = bits >> 16;
  *p++ = bits >> 8;
  *p++ = bits & 0xff;

  // append tampering
  strcpy(p, ";admin=true");
  p += strlen(p);

  int tampered_msg_len = p - tamp;

  printf("Validate mac using length: %d\n", tampered_msg_len);
  hexdump(tamp, tampered_msg_len);

  printf("-----------------------------------\nValidate tampered mac with length: %d\n", tampered_msg_len);
  valid = validate_mac(tamp, tampered_msg_len, secret_key, secret_key_len, tampmac);

  if(!valid) {
    keylen++;
    printf("Message not accepted, try larger keylength %d\n", keylen);
  } else {
    printf("tampered message accepted, guessed keylength: %d\n", keylen);
  }

  break;

  } while(!valid && keylen < 2);  


  if(!valid) {
    printf("Failed to get tampered message accepted\n");
  }

  return 0;
}
