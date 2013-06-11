#ifndef TOOLS_H
#define TOOLS_64_H

int base64encode(const unsigned char *s, int len, char *d);
int base64decode(const char *s, int len, unsigned char *d);
int  hexdecode(const char *s, unsigned char *d);
void hexencode(const unsigned char *s, int len, char *d);

double rate_text(const unsigned char *s, int len, int dump);
double rate_text_key(const unsigned char *s, int len, unsigned char key);

unsigned char find_xor_key(unsigned char *s, int len, double *best_rate);

void xor_encrypt(unsigned char *data, const unsigned char *key, int data_len, int key_len);

void strip_terminate(char *data, int len);
void hexdump(const unsigned char *data, int len);

#endif