#ifndef BASE_64_H
#define BASE_64_H

int base64encode(const unsigned char *s, int len, char *d);
int base64decode(const char *s, int len, unsigned char *d);
int  hexdecode(const char *s, unsigned char *d);
// TODO put len after s
void hexencode(const unsigned char *s, char *d, int len);

#endif
