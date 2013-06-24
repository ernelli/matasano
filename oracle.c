#include <stdlib.h>
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

static int o_init = 0;

const char *o_data_b64 =
  "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkg"
  "aGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBq"
  "dXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUg"
  "YnkK";

static unsigned char o_key[16];
static unsigned char o_data[256];
static int o_data_len = 0;

int encryption_oracle_ecb(const unsigned char *in, int len, unsigned char *outbuff, int outlen) {
  int i, j, I;

  if(len + o_data_len > outlen) {
    fprintf(stderr, "encryption_oracle_ecb failed, outbuff size (%d) too small, %d bytes needed\n", outlen, len+o_data_len);
    exit(1);
  }
  
  if(!o_init) {
    random_bytes(o_key, 16);
    o_data_len = base64decode(o_data_b64, strlen(o_data_b64), o_data);
    o_init = 1;
  }
  
  memcpy(outbuff, in, len);
  memcpy(outbuff+len, o_data, o_data_len);

  len +=o_data_len;

  len = add_padding(outbuff, len, 16);

  aes_ecb_encrypt(outbuff, len, o_key, 16);

  return len;
}

int encryption_oracle_ecb_random_prefix(const unsigned char *in, int len, unsigned char *outbuff, int outlen) {
  int i, j, I;

  unsigned char prefix_len_s[1];
  int prefix_len;
  
  if(len + o_data_len > outlen) {
    fprintf(stderr, "encryption_oracle_ecb failed, outbuff size (%d) too small, %d bytes needed\n", outlen, len+o_data_len);
    exit(1);
  }
  
  if(!o_init) {
    random_bytes(o_key, 16);
    o_data_len = base64decode(o_data_b64, strlen(o_data_b64), o_data);
    o_init = 1;
  }
  
  random_bytes(prefix_len_s, 1);
  prefix_len = (int)prefix_len_s[0];

  // test the performance using a fixed sized prefix
  //  prefix_len = 5;

  if(len + prefix_len + o_data_len > outlen) {
    fprintf(stderr, "encryption_oracle_ecb failed, outbuff size (%d) too small, %d bytes needed\n", outlen, len+prefix_len+o_data_len);
    exit(1);
  }

  random_bytes(outbuff, prefix_len);
  memcpy(outbuff+prefix_len, in, len);
  len += prefix_len;
  memcpy(outbuff+len, o_data, o_data_len);

  len +=o_data_len;

  len = add_padding(outbuff, len, 16);
  
  aes_ecb_encrypt(outbuff, len, o_key, 16);

  return len;
}
