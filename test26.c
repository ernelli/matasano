#include<stdio.h>
#include<string.h>
#include"tools.h"

static unsigned char ud_key[16];
static unsigned char ud_nonce[8];
static int ud_init = 0;


int add_userdata_ctr(const char *userdata, unsigned char *ciphertext, int bufflen) {
  char *prefix = "comment1=cooking%20MCs;userdata=";
  char *postfix = ";comment2=%20like%20a%20pound%20of%20bacon";

  const char *s;
  char *d;

  int len;

  if(!ud_init) {
    random_bytes(ud_key, 16);
    random_bytes(ud_nonce, 8);
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

  aes_ctr_encrypt(ciphertext, len, ud_key, sizeof(ud_key), ud_nonce, 0, 0);
  
  return len;
}

int is_user_admin_ctr(unsigned char *data, int len) {
  unsigned char plaintext[1024];

  memcpy(plaintext, data, len);

  aes_ctr_decrypt(plaintext, len, ud_key, sizeof(ud_key), ud_nonce, 0, 0);

  if(validate_padding(plaintext, len)) {
    strip_padding(plaintext, len);
    if(strstr((char *)plaintext, ";admin=true;")) {
      return 1;
    }
  }

  return 0;
}


// prepare attackstring where plaintext 'U' and 'S' becomes ; and & after xor with 0x68
       
// = 0x3d  0011 1101
// ; 0x3b  0011 1011
//          ||  |
// U 0x55  0101 0101 
// S 0x53  0101 0011
 
//   0x68  0110 1000

// |               |               |               |               |               |               
// comment1=cooking%20MCs;userdata=;comment2=%20like%20a%20pound%20of%20bacon

// |               |               |               |               |               |               |         pppppp
// comment1=cooking%20MCs;userdata=mesostrashymydatfoooa;admin=true;comment2=%20like%20a%20pound%20of%20bacon


int main(int argc, char *argv[]) {
  char plaintext[256];
  unsigned char *dst, data[32768];
  char hex[256];
  int add, len;

  // I'm lazy, I reuse my old attack data even though ctr mode bit flipping can be done wihout
  // block aligned attack data or leading trash block
  char *userdata = "mesostrashymydatfoooaSadminUtrue";

  len = add_userdata_ctr("data;admin=true", data, sizeof(data));
  printf("simple attack, user_is_admin: %s\n", is_user_admin_ctr(data, len) ? "true" : "false");

  len = add_userdata_ctr(userdata, data, sizeof(data));

  // in ctr mode, flip the bits at the real offset, in cbc mode the bits was flipped in previous block
  // e.g the block being xor'ed with the target block being decrypted.
  data[16+32+5] ^= 0x68;
  data[16+32+11] ^= 0x68;

  printf("real attack, user_is_admin: %s\n", is_user_admin_ctr(data, len) ? "true" : "false");

  return 0;
}
