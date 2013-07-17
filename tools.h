#ifndef TOOLS_H
#define TOOLS_64_H

int base64encode(const unsigned char *s, int len, char *d);
int base64decode(const char *s, int len, unsigned char *d);
int  hexdecode(const char *s, unsigned char *d);
void hexencode(const unsigned char *s, int len, char *d);

const int *get_letter_test_table();
double rate_text(const unsigned char *s, int len, int dump);
double rate_text_key(const unsigned char *s, int len, unsigned char key, int table);

unsigned char find_xor_key(unsigned char *s, int len, double *best_rate, int table);

void xor_encrypt(unsigned char *data, const unsigned char *key, int data_len, int key_len);

void strip_terminate(char *data, int len);
void hexdump(const unsigned char *data, int len);

void aes_ecb_encrypt(unsigned char *block, int len, unsigned char *key, int key_len);
void aes_ecb_decrypt(unsigned char *block, int len, unsigned char *key, int key_len);
void aes_cbc_encrypt(unsigned char *block, int len, unsigned char *key, int key_len, unsigned char *iv);
void aes_cbc_decrypt(unsigned char *block, int len, unsigned char *key, int key_len, unsigned char *iv);

void mt19937_encrypt(unsigned char *data, int len, unsigned short key);
void mt19937_decrypt(unsigned char *data, int len, unsigned short key);

void seed_random(const unsigned char *seed, int len);
void random_bytes(unsigned char *data, int len);
int detect_ecb(unsigned char *indata, int len, int blocklen);

struct MT_generator {
  unsigned int state[624];
  int index;
} MT;

void initialize_generator(struct MT_generator *gen, unsigned int seed);
void MT_initialize_generator(unsigned int seed);
unsigned int extract_number(struct MT_generator *gen);
unsigned int MT_extract_number();

#endif
