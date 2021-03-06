#include<stdio.h>
#include<string.h>

#include"tools.h"

#define TEST_SHA1
//#define TEST_MAC

void test_sha1() {
  unsigned char digest[20];
  const char *s;

  s = "The quick brown fox jumps over the lazy dog";

  sha1(s, strlen(s), digest); 
  printf("digest\n");
  hexdump(digest, 20);
  printf("ref\n");
  printf("0000: 2f d4 e1 c6 7a 2d 28 fc ed 84 9e e1 bb 76 e7 39 1b 93 eb 12\n");
  //SHA1("The quick brown fox jumps over the lazy cog")
  //= de9f2c7f d25e1b3a fad3e85a 0bd17d9b 100db4b3
  //The hash of the zero-length string is:

  sha1("", 0, digest); hexdump(digest, 20);
  printf("0000: da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 90 af d8 07 09\n");

  //  const char *s0 = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    const char *s0 = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
  //const char *s0 = "AA";
  sha1(s0, strlen(s0), digest); printf("63A\n"); hexdump(digest, 20);
}

int main(int argc, char *argv[]) {
  unsigned char data[1024*1024];
  unsigned char digest[20];
  int len;

#ifdef TEST_SHA1
  test_sha1();
#endif

#ifdef TEST_MAC
  len = fread(data, 1, sizeof(data), stdin);
  sha1(data, len, digest);
  //hexdump(digest, 20);
 
   char msg[200];
  memset(msg, 'A', 199);
  msg[199] = '\0';
  char *key = "YELLOW SUBMARINEYELLOW SUBMARINEYELLOW SUBMARINEYELLOW SUBMARINEYELLOW SUBMARINE";



  printf("mac valid: %d\n", validate_mac(msg, strlen(msg), key, strlen(key), digest));
#endif



  return 0;
}
