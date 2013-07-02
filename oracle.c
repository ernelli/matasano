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

static char *po_plaintext[] =
  {"MDAwMDAwTm93IHRoYXQgdGhlIHBhcnR5IGlzIGp1bXBpbmc=",
   "MDAwMDAxV2l0aCB0aGUgYmFzcyBraWNrZWQgaW4gYW5kIHRoZSBWZWdhJ3MgYXJlIHB1bXBpbic=",
   "MDAwMDAyUXVpY2sgdG8gdGhlIHBvaW50LCB0byB0aGUgcG9pbnQsIG5vIGZha2luZw==",
   "MDAwMDAzQ29va2luZyBNQydzIGxpa2UgYSBwb3VuZCBvZiBiYWNvbg==",
   "MDAwMDA0QnVybmluZyAnZW0sIGlmIHlvdSBhaW4ndCBxdWljayBhbmQgbmltYmxl",
   "MDAwMDA1SSBnbyBjcmF6eSB3aGVuIEkgaGVhciBhIGN5bWJhbA==",
   "MDAwMDA2QW5kIGEgaGlnaCBoYXQgd2l0aCBhIHNvdXBlZCB1cCB0ZW1wbw==",
   "MDAwMDA3SSdtIG9uIGEgcm9sbCwgaXQncyB0aW1lIHRvIGdvIHNvbG8=",
   "MDAwMDA4b2xsaW4nIGluIG15IGZpdmUgcG9pbnQgb2g=",
   "MDAwMDA5aXRoIG15IHJhZy10b3AgZG93biBzbyBteSBoYWlyIGNhbiBibG93"
  };

static int po_size = sizeof(po_plaintext)/sizeof(po_plaintext[0]);

static int po_init = 0;
static unsigned char po_key[16];
static unsigned char po_iv[16];

int cbc_padding_oracle(unsigned char *data, int size, unsigned char *iv) {
  unsigned int index;
  int len;

  if(!po_init) {
    random_bytes(po_key, 16);
    random_bytes(po_iv, 16);
    po_init = 1;
  }

  random_bytes((unsigned char *)&index, sizeof(index));
  index %= po_size;

  len = strlen(po_plaintext[index]);
  if(len + 16 & ~0xf > size) {
    fprintf(stderr, "supplied buffer too small (%d) %d bytes needed\n", size, len +16 & ~0xf);
    exit(1);
  }
  //printf("index; %d\n", index);

  len = base64decode(po_plaintext[index], strlen(po_plaintext[index]), data);
  //  memcpy(data, po_plaintext[index], len);
  len = add_padding(data, len, 16);
  
  aes_cbc_encrypt(data, len, po_key, sizeof(po_key), po_iv);

  // return IV to caller, seems odd, but the challenge instructions says so:
  //
  // "generate a random AES key (which it should save for all future
  // encryptions), pad the string out to the 16-byte AES block size and
  // CBC-encrypt it under that key, providing the caller the ciphertext and
  // IV."
  
  memcpy(iv, po_iv, 16);

  // unless IV is known by caller, first block can never be deciphered

  return len;
}

int cbc_padding_oracle_validate(const unsigned char *ciphertext, int len) {
  unsigned char plaintext[1024];

  if(len > sizeof(plaintext)) {
    fprintf(stderr, "ciphertext too large (%d) max %d bytes can be validated\n", len, sizeof(plaintext));
    exit(1);    
  }

  memcpy(plaintext, ciphertext, len);

  aes_cbc_decrypt(plaintext, len, po_key, sizeof(po_key), po_iv);

  return validate_padding(plaintext, len);
}
