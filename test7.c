#include <stdio.h>
#include <string.h>

#include "tools.h"

//#define USE_SSL

#ifdef USE_SSL

#include <openssl/conf.h>
#include <openssl/evp.h>

void handleErrors(const char *operation) {
  printf("decryption failed during: %s\n", operation);
  ERR_print_errors_fp(stdout);
  exit(1);
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
  unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int plaintext_len;

  /* Load the human readable error strings for libcrypto */
  ERR_load_crypto_strings();

  /* Load all digest and cipher algorithms */
  OpenSSL_add_all_algorithms();

  /* Load config file, and other important initialisation */
  OPENSSL_config(NULL);


  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors("EVP_CIPHER_CTX_new");

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL))
    handleErrors("EVP_DecryptInit_ex");

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors("EVP_DecryptUpdate");

  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors("EVP_DecryptFinal_ex");
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  /* Clean up */

  /* Removes all digests and ciphers */
  EVP_cleanup();

  /* Remove error strings */
  ERR_free_strings();

  return plaintext_len;
}
#else
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *plaintext) {
  memcpy(plaintext, ciphertext, ciphertext_len);
  aes_ecb_decrypt(plaintext, ciphertext_len, key, 16);
  return ciphertext_len;
}
#endif

int main(int argc, char *argv[]) {
  char inbuf[256];
  unsigned char data[32768], *dst;
  unsigned char testbuf[32768];
  int i, j, len, add;

  dst = data;
  
  while(fgets(inbuf, sizeof(inbuf), stdin)) {
    len = strlen(inbuf);
    if(inbuf[len-1] == '\n') {
      len--;
    }
    dst += (add = base64decode(inbuf, len, dst));
  }

  len = dst - data;

  /* ... Do some crypto stuff here ... */
  unsigned char *key = "YELLOW SUBMARINE";

  len = decrypt(data, len, key, testbuf);
  
  strip_padding(testbuf, len);
  fputs(testbuf, stdout);

  return 0;
}
