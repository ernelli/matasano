#ifndef BASE_64_H
#define BASE_64_H

void base64encode(const unsigned char *s, int len, char *d);
int base64decode(const char *s, int len, unsigned char *d);
void hexdecode(const char *s, unsigned char *d);
void hexencode(const unsigned char *s, char *d, int len);

#endif
