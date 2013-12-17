#include<stdio.h>
#include<string.h>

#include"tools.h"

void test_md4()  {
  unsigned char digest[16];

  //MD4 ("") = 31d6cfe0d16ae931b73c59d7e0c089c0
  _md4("", 0, digest);
  hexdump(digest, 16);

  //MD4 ("a") = bde52cb31de33e46245e05fbdbd6fb24
  _md4("a", 1, digest);
  hexdump(digest, 16);
  
  //MD4 ("abc") = a448017aaf21d8525fc10ae87aa6729d
  _md4("abc", 3, digest);
  hexdump(digest, 16);

}

int main(int argc, char *argv[]) {
  unsigned char data[1024*1024];
  unsigned char digest[16];
  int len;

  test_md4();

  return 0;
}
