#ifndef RATE_H
#define RATE_H

double rate_text(const unsigned char *s, int len, int dump);
double rate_text_key(const unsigned char *s, int len, unsigned char key);
unsigned char find_xor_key(unsigned char *s, int len, double *best_rate);
void xor_encrypt(unsigned char *data, const unsigned char *key, int data_len, int key_len);
void strip_terminate(char *data, int len);

#endif
