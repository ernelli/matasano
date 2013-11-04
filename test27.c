#include<stdio.h>
#include<string.h>
#include"tools.h"

static unsigned char ud_key[16];
static int ud_init = 0;


int add_userdata_iv(const char *userdata, unsigned char *ciphertext, int bufflen) {
  char *prefix = "comment1=cooking%20MCs;userdata=";
  char *postfix = ";comment2=%20like%20a%20pound%20of%20bacon";

  const char *s;
  char *d;

  int len;

  if(!ud_init) {
    random_bytes(ud_key, 16);
    ud_init = 1;
  }

  strcpy((char *)ciphertext, prefix);
  
  s = userdata;
  d = (char *)ciphertext + strlen((char *)ciphertext);

  while(*s) {
    if(*s == ';' || *s == '=') {
      d += sprintf(d, "%%%02x", *s);
    } else {
      *d++ = *s;
    }
    s++;
  }

  *d++ = '\0';
  
  // strcat(ciphertext, userdata);
  strcat((char *)ciphertext, postfix);

  len = strlen((char *)ciphertext);
  len = add_padding(ciphertext, len, 16);

  aes_cbc_encrypt(ciphertext, len, ud_key, sizeof(ud_key), ud_key);
  
  return len;
}

int validate_url(unsigned char *data, int len, char *error_msg, int size) {
  unsigned char plaintext[1024];
  int i;

  memcpy(plaintext, data, len);

  aes_cbc_decrypt(plaintext, len, ud_key, sizeof(ud_key), ud_key);
  for(i = 0; i < len; i++) {
    if(plaintext[i] > 127) {
      if(error_msg) {
        snprintf(error_msg, size, "Invalid url: %.*s", len, plaintext);
      }
      return 0;
    }
  }

  return 1;
}

int main(int argc, char *argv[]) {
  unsigned char ciphertext[4096];

  unsigned char attack[256];
  unsigned char plaintext[256];

  char msg[256];
  int len;

  len = add_userdata_iv("some testdata to get started", ciphertext, sizeof(ciphertext));
  
  printf("ciphertext\n");
  hexdump(ciphertext, len);

  printf("url data is valid: %d\n", validate_url(ciphertext, len, NULL, 0));


  memcpy(attack, ciphertext, 16);
  memset(attack+16, 0, 16);
  memcpy(attack+32, ciphertext, 16);

  unsigned char revealed_key[16];

  if(!validate_url(attack, 48, msg, sizeof(msg))) {
    
    if(strlen(msg) > sizeof(plaintext)) {
      printf("Error message too large %d\n", strlen(msg));
      return 1;
    }

    if(strlen(msg) < (13+48) ) {
      printf("Error message too small: %d, possible null char in trashed block\n", strlen(msg));
      return 1;
    }

    printf("error: %s\n", msg);
    memcpy(plaintext, msg+13, 48);

    // copy p1
    memcpy(revealed_key, plaintext, 16);
    // xor with p3
    xor_encrypt(revealed_key, plaintext+32, 16, 16);

    printf("revealed key: ");
    hexdump(revealed_key, 16);
    printf("\n\n\nplaintext\n");

    aes_cbc_decrypt(ciphertext, len, revealed_key, 16, revealed_key);

    hexdump(ciphertext, len);
  }

 

  return 0;
}
