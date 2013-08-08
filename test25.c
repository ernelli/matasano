#include<stdio.h>
#include<string.h>
#include"tools.h"

void test_aes_ctr_edit() {
  unsigned char *key = "YELLOW SUBMARINE";
  unsigned char ciphertext[256], testbuff[256];
  unsigned char nonce[8];
  
  memset(nonce, 0, sizeof(nonce));

  memset(testbuff, 'A', sizeof(testbuff));
  memcpy(ciphertext, testbuff, sizeof(testbuff));
  aes_ctr_encrypt(ciphertext, sizeof(ciphertext), key, 16, nonce, 0, 0);

  hexdump(ciphertext, 256);

  int offset, len;

  unsigned char val;

  random_bytes(&val, 1);
  offset = val; 
  random_bytes(&val, 1);
  len = val;
  if(offset+len > sizeof(testbuff) - 1) {
    len = sizeof(testbuff) - offset - 1;
  }
  //printf("edit at %d len %d\n", offset, len);
  memset(testbuff, 'A', len);
  testbuff[len] = '\0';
  aes_ctr_edit(ciphertext, key, offset, testbuff);
  hexdump(ciphertext, 256);
}

int main(int argc, char *argv[]) {
  char inbuf[256];
  unsigned char data[32768], *dst;
  unsigned char testbuf[32768];
  int i, j, len, add;

  //test_aes_ctr_edit();

  dst = data;
  
  while(fgets(inbuf, sizeof(inbuf), stdin)) {
    len = strlen(inbuf);
    if(inbuf[len-1] == '\n') {
      len--;
    }
    dst += (add = base64decode(inbuf, len, dst));
  }

  len = dst - data;

  aes_ecb_decrypt(data, len, "YELLOW SUBMARINE", 16);

  if(!validate_padding(data, len)) {
    fprintf(stderr, "invalid padding\n");
    return 1;
  }

  len = strip_padding(data, len);
  
  data[len] = '\0';

  // recovered plaintext
  //printf("%s", data);

  // generate random ctr key
  unsigned char key[16];
  random_bytes(key, sizeof(key));

  unsigned char nonce[8];
  memset(nonce, 0, sizeof(nonce));

  aes_ctr_encrypt(data, len, key, sizeof(key), nonce, 0, 0);

  //printf("encrypted plaintext\n");
  //hexdump(data, len);

  // now break this cipher using aes_ctr_edit...  a walk in the park.

  char plaintext[4096];

  unsigned char buffer[64];  
  unsigned char *src;

  const char *edit = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

  int n;

  src = data; 
  dst = plaintext;

  // decrypt 64 bytes a time
  while(len) {
    n = len > 64 ? 64 : len;

    // save ciphertext ( plaintext XOR stream ) = ciphertext
    memcpy(buffer, src, n);

    // edit in place with ( edit XOR stream ), edit is known data
    aes_ctr_edit(data, key, src-data, edit);

    // extract stream ( edit XOR stream XOR edit ) = stream
    xor_encrypt(src, edit, n, n);

    // decrypt plaintext ( ciphertext XOR stream ) = plaintext
    xor_encrypt(buffer, src, n, n);
    
    memcpy(dst, buffer, n);
    
    src += n;
    dst += n;
    
    len -= n;
  }

  *dst = '\0';
  
  printf("plaintext:\n%s\n", plaintext);
  
  return 0;
}
