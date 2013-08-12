
void main() {
  unsigned char digest[20];

  memset(digest, 0, 20);

  SHA1("hello i love you", 16, digest);

  hexdump(digest, 20);
}
