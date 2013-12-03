#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<sys/time.h>
#include<time.h>
#include"tools.h"

static char inttob64[64];
static unsigned char b64toint[256];

static int b64_init_done = 0;
static void b64_init() {
  int i;
  
  if(b64_init_done) {
    return;
  }

  for(i = 0; i < 10; i++) {
    inttob64[i] = 'A' + i;
    inttob64[i+26] = 'a' + i;
    inttob64[i+52] = '0' + i;
  }

  for(; i < 26; i++) {
    inttob64[i] = 'A' + i;
    inttob64[i+26] = 'a' + i;
  }

  inttob64[62] = '+';
  inttob64[63] = '/';

  for(i = 0; i < 64; i++) {
    b64toint[(int)inttob64[i]] = i;
  }
  
  b64_init_done = 1;
}


//   s0       s1       s2    
// 76543210 76543210 76543210

//   s0     s0  s1     s1  s2     s2
// 765432 | 10 7654 | 3210 76 | 543210

int base64encode(const unsigned char *s, int len, char *d) {
  b64_init();
  char *d0 = d;
  
  while(len >= 3) {
    d[0] = inttob64[s[0] >> 2];
    d[1] = inttob64[( (s[0] << 4) | (s[1] >> 4) ) & 0x3f];
    d[2] = inttob64[( (s[1] << 2) | (s[2] >> 6) ) & 0x3f];
    d[3] = inttob64[ s[2] & 0x3f];
   
    len -= 3;
    s += 3;
    d += 4;
  }

  if(len == 2) {
    d[0] = inttob64[s[0] >> 2];
    d[1] = inttob64[( (s[0] << 4) | (s[1] >> 4) ) & 0x3f];
    d[2] = inttob64[(s[1] << 2) &0x3f];
    d[3] = '=';
    d += 4;
  } else if(len == 1) {
    d[0] = inttob64[s[0] >> 2];
    d[1] = inttob64[(s[0] << 4) & 0x3f];
    d[2] = '=';
    d[3] = '=';
    d += 4;
  }

  return d-d0;
}

//   s0     s1     s2     s3
// 543210 543210 543210 543210

//   s0   s1   s1   s2    s2   s3
// 543210 54| 3210 5432 | 10 543210


int base64decode(const char *s, int len, unsigned char *d) {
  unsigned char *d0 = d;
  b64_init();

  while(len >= 4) {

    d[0] = (b64toint[(int)s[0]] << 2) | (b64toint[(int)s[1]] >> 4);
    d[1] = (b64toint[(int)s[1]] << 4) | (b64toint[(int)s[2]] >> 2);
    d[2] = (b64toint[(int)s[2]] << 6) | (b64toint[(int)s[3]]);
    
    d += 3 - (s[2] == '=' ? 2 : (s[3] == '=' ? 1 : 0));
    s += 4;
    len -= 4;
  }

  return d - d0;
}

#define hex2int(c) ((c >= '0' && c <= '9') ? c - '0' : (c >= 'A' && c <= 'F') ? c - 'A' + 10 : (c >= 'a' && c <= 'f') ? c - 'a' + 10: 0)

int hexdecode(const char *s, unsigned char *d) {
  unsigned char *d0 = d;
  
  while(s[0] && s[1]) {
    *d++ = 16*hex2int(s[0]) + hex2int(s[1]);
    s+=2;
  }
  return d-d0;
}

#define int2hex(n) ( (n) >= 10 ? 'a' + (n) - 10 : '0' + (n))

void hexencode(const unsigned char *s, int len, char *d) {
  while(len--) {
    *d++ = int2hex(*s >> 4);
    *d++ = int2hex(*s & 0xf);
    s++;
  }
}


struct freq_table_s {
  int letter;
  double f;
} letter_freq_table[] = {
  {'a', 8.167},
  {'b', 1.492},
  {'c', 2.782},
  {'d', 4.253},
  {'e', 12.702},
  {'f', 2.228},
  {'g', 2.015},
  {'h', 6.094},
  {'i', 6.966},
  {'j', 0.153},
  {'k', 0.772},
  {'l', 4.025},
  {'m', 2.406},
  {'n', 6.749},
  {'o', 7.507},
  {'p', 1.929},
  {'q', 0.095},
  {'r', 5.987},
  {'s', 6.327},
  {'t', 9.056},
  {'u', 2.758},
  {'v', 0.978},
  {'w', 2.360},
  {'x', 0.150},
  {'y', 1.974},
  {'z', 0.074}
};

double freq_table[] = {
  8.167,
  1.492,
  2.782,
  4.253,
  12.702,
  2.228,
  2.015,
  6.094,
  6.966,
  0.153,
  0.772,
  4.025,
  2.406,
  6.749,
  7.507,
  1.929,
  0.095,
  5.987,
  6.327,
  9.056,
  2.758,
  0.978,
  2.360,
  0.150,
  1.974,
  0.074,
};

double first_letter_freq_table[] = {
  11.602,	
  4.702,	
  3.511,	
  2.670,	
  2.007,	
  3.779,	
  1.950,	
  7.232,	
  6.286,	
  0.597,	
  0.590,	
  2.705,	
  4.374,	
  2.365,	
  6.264,	
  2.545,	
  0.173,	
  1.653,	
  7.755,	
  16.671,	
  1.487,	
  0.649,	
  6.753,	
  0.037,	
  1.620,	
  0.034
};

int letter_test_table[256];

static int freq_table_size = sizeof(freq_table)/sizeof(freq_table[0]);

static int f_init_done = 0;

int comp_freq_table(const void *_a, const void *_b) {

  const struct freq_table_s *a = (const struct freq_table_s *)_a;
  const struct freq_table_s *b = (const struct freq_table_s *)_b;

  return a->f > b->f ? -1 : 1;
}

static void f_init() {
  int i;
  for(i = 0; i < freq_table_size; i++) {
    freq_table[i] /= 100;
  }
}

const int *get_letter_test_table() {
  int i, N, n;

  int marker[256];

  static int init_done = 0;


  if(!init_done) {
    N = sizeof(letter_freq_table)/sizeof(struct freq_table_s);
    
    qsort(letter_freq_table, N, sizeof(struct freq_table_s), comp_freq_table);
    
    for(i = 0; i < N; i++) {
      letter_test_table[i] = letter_freq_table[i].letter;
      letter_test_table[i+N] = letter_freq_table[i].letter - 'a' + 'A';
    }
    
    i = 2*N;
    
    letter_test_table[i++] = ' ';
    letter_test_table[i++] = '.';
    letter_test_table[i++] = '\n';
    
    for(n = 0; n < 10; n++) {
      letter_test_table[i++] = '0' + n;
    }
    
    memset(marker, 0, sizeof(marker));
    
    for(n = 0; n < i; n++) {
      marker[letter_test_table[n]] = 1;
    }

    for(n = 0; n < 256; n++) {
      if(!marker[n]) {
        if(i >= 256) {
          fprintf(stderr, "internal error generating letter_test_table\n");
          exit(1);
        }
        letter_test_table[i++] = n;
      }
    }
    
    if(i != 256) {
      fprintf(stderr, "internal error generating letter_test_table\n");
      exit(1);
    }

    init_done = 1;
  }

  return letter_test_table;
}

double xcorr(const double *X, const double *Y, int N) {
  int i;
  double s = 0;

  for(i = 0; i < N; i++) {
    s += X[i] * Y[i];
  }

  return s;
}

double rate_text_key(const unsigned char *s, int len, unsigned char key, int table) {
  int i, N = 0, n = 0, v = 0, c, freq[26];
  double rel_freq[26];

  if(!f_init_done) {
    f_init();
    f_init_done = 1;
  }
  
  memset(freq, 0, sizeof(freq));
  while(N < len) {
    N++;
    
    c = *s ^ key;

    if(isalpha(c)) {
      c = tolower(c);

      if(c >= 'a' && c <= 'z') {
        freq[c - 'a']++;
        n++;
        v++;
      }
    } else {
      if(c == ' ') {
        v++;
      }
    }

    s++;
  }

  if(n) {
    for(i = 0; i < 26; i++) {
      rel_freq[i] = (double)freq[i] / (double)n;
    }
  } else {
    memset(rel_freq, 0, sizeof(rel_freq));
  }

  //  printf("xcorr: %f\n", xcorr(rel_freq, freq_table, 26));
  //  printf("ref1: %f\n", xcorr(freq_table, freq_table, 26));
  //  printf("ref2: %f\n", xcorr(rel_freq, rel_freq, 26));

  return v*xcorr(rel_freq, table ? first_letter_freq_table : freq_table, 26);
}


double rate_text(const unsigned char *s, int len, int dump) {
  return rate_text_key(s, len, 0, 0);
  /*
  int i, N = 0, n = 0, v = 0, c, freq[26];
  double rel_freq[26];

  if(!f_init_done) {
    f_init();
    f_init_done = 1;
  }
  
  memset(freq, 0, sizeof(freq));
  while(N < len) {
    N++;
    if(isalpha(*s)) {
      c = tolower(*s);

      if(c >= 'a' && c <= 'z') {
        freq[c - 'a']++;
        n++;
        v++;
      }
    } else {
      if(*s == ' ') {
        v++;
      }
    }


    s++;
  }

  if(dump)
    printf("found %d valid letters of %d total\n", v, N);

  if(n) {
    for(i = 0; i < 26; i++) {
      rel_freq[i] = (double)freq[i] / (double)n;
      if(dump)
        printf("%c rf %.5f, ref: %.5f\n", 'a'+i, rel_freq[i], freq_table[i]);
    }
  } else {
    memset(rel_freq, 0, sizeof(rel_freq));
  }

  //  printf("xcorr: %f\n", xcorr(rel_freq, freq_table, 26));
  //  printf("ref1: %f\n", xcorr(freq_table, freq_table, 26));
  //  printf("ref2: %f\n", xcorr(rel_freq, rel_freq, 26));

  return v*xcorr(rel_freq, freq_table, 26);
  */
}


unsigned char find_xor_key(unsigned char *s, int len, double *best_rate, int table) {
  int i;
  double score[256];

  memset(score, 0, sizeof(score));

  for(i = 0; i < 256; i++) {
    score[i] = rate_text_key(s, len, i, table);
  }

  double maxs = score[0];
  int xor = 0;
  
  for(i = 0; i < 256; i++) {
    if(score[i] > maxs) {
      maxs = score[i];
      xor = i;
    }
  }

  if(best_rate) {
    *best_rate = maxs;
  }

  return xor;
}

void xor_encrypt(unsigned char *data, const unsigned char *key, int data_len, int key_len) {
  int i = 0;

  while(data_len--) {
    *data++ ^= key[i++];
    if(i >= key_len) {
      i = 0;
    }
  }
}

int add_padding(unsigned char *data, int len, int blocklen) {
  int add;
  unsigned char *d, *e;

  add = blocklen - len % blocklen;

  d = data + len;
  e = d + add;


  while(d < e) {
    *d++ = add;
  }

  return d - data;
}

void strip_terminate(char *data, int len) {
  int i;

  data[len] = '\0';
  for(i = 0; i < len; i++) {
    if(!isprint(data[i])) {
      data[i] = '.';
    }
  }
}

int strip_padding(unsigned char *data, int len) {
  int p;

  p = data[len-1];
  if(p < 1 || p > 16) {
    return len;
  }

  if(p > len) {
    return len;
  }
  
  data[len-p] = '\0';

  return len - p;
}


void hexdump(const unsigned char *data, int len) {
  int i;
  char line[60], *d;
  int m = 0;
  char ascii[32];

  d = line;
  
  d = line + sprintf(line, "0000: ");

  for(i = 0; i < len; i++) {
    m = i & 0xf;

    if(i) {
      if(m == 0) {
        ascii[16] = '\0';
        printf("%s %s\n", line, ascii);
        d = line + sprintf(line, "%04x: ", i);
      } else if(m == 8) {
        d += sprintf(d, "-");
      } else {
        d += sprintf(d, " ");
      }
    }    

    d += sprintf(d, "%02x", data[i]);

    if(isprint(data[i])) {
      ascii[m] = data[i];
    } else {
      ascii[m] = '.';
    }
  }
  ascii[m+1] = '\0';
  printf("%-53s %s\n", line, ascii);
}

/////////////////
//
// AES-128,192,256 block cipher implementation

/*
 *  jsaes version 0.1  -  Copyright 2006 B. Poettering
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307 USA
 */

// Port to C, Mattias Ernelli

/* 
   AES_Init: initialize the tables needed at runtime. Call this function
   before the (first) key expansion.
*/

/******************************************************************************/

/* The following lookup tables and functions are for internal use only! */

static int AES_Sbox[] = {
  99,124,119,123,242,107,111,197,48,1,103,43,254,215,171,
  118,202,130,201,125,250,89,71,240,173,212,162,175,156,164,114,192,183,253,
  147,38,54,63,247,204,52,165,229,241,113,216,49,21,4,199,35,195,24,150,5,154,
  7,18,128,226,235,39,178,117,9,131,44,26,27,110,90,160,82,59,214,179,41,227,
  47,132,83,209,0,237,32,252,177,91,106,203,190,57,74,76,88,207,208,239,170,
  251,67,77,51,133,69,249,2,127,80,60,159,168,81,163,64,143,146,157,56,245,
  188,182,218,33,16,255,243,210,205,12,19,236,95,151,68,23,196,167,126,61,
  100,93,25,115,96,129,79,220,34,42,144,136,70,238,184,20,222,94,11,219,224,
  50,58,10,73,6,36,92,194,211,172,98,145,149,228,121,231,200,55,109,141,213,
  78,169,108,86,244,234,101,122,174,8,186,120,37,46,28,166,180,198,232,221,
  116,31,75,189,139,138,112,62,181,102,72,3,246,14,97,53,87,185,134,193,29,
  158,225,248,152,17,105,217,142,148,155,30,135,233,206,85,40,223,140,161,
  137,13,191,230,66,104,65,153,45,15,176,84,187,22};

static int AES_Sbox_Inv[256];

static int AES_ShiftRowTab[] = { 0,5,10,15,4,9,14,3,8,13,2,7,12,1,6,11 };

static int AES_ShiftRowTab_Inv[16];

static int AES_xtime[256];

static void AES_SubBytes(unsigned char *state, int *sbox) {
  int i;
  for(i = 0; i < 16; i++)
    state[i] = sbox[state[i]];  
}

static void AES_AddRoundKey(unsigned char *state, unsigned char *rkey) {
  int i;
  for(i = 0; i < 16; i++)
    state[i] ^= rkey[i];
}

static void AES_ShiftRows(unsigned char *state, int *shifttab) {
  int i;
  unsigned char h[16];

  memcpy(h, state, 16);

  for(i = 0; i < 16; i++)
    state[i] = h[shifttab[i]];
}

static void AES_MixColumns(unsigned char *state) {
  int i, s0, s1, s2, s3, h;

  for(i = 0; i < 16; i += 4) {
    s0 = state[i + 0];
    s1 = state[i + 1];
    s2 = state[i + 2];
    s3 = state[i + 3];

    h = s0 ^ s1 ^ s2 ^ s3;

    state[i + 0] ^= h ^ AES_xtime[s0 ^ s1];
    state[i + 1] ^= h ^ AES_xtime[s1 ^ s2];
    state[i + 2] ^= h ^ AES_xtime[s2 ^ s3];
    state[i + 3] ^= h ^ AES_xtime[s3 ^ s0];
  }
}

static void AES_MixColumns_Inv(unsigned char *state) {
  int i, s0, s1, s2, s3, h, xh, h1, h2;

  for(i = 0; i < 16; i += 4) {

    s0 = state[i + 0];
    s1 = state[i + 1];
    s2 = state[i + 2];
    s3 = state[i + 3];

    h = s0 ^ s1 ^ s2 ^ s3;
    xh = AES_xtime[h];

    h1 = AES_xtime[AES_xtime[xh ^ s0 ^ s2]] ^ h;
    h2 = AES_xtime[AES_xtime[xh ^ s1 ^ s3]] ^ h;

    state[i + 0] ^= h1 ^ AES_xtime[s0 ^ s1];
    state[i + 1] ^= h2 ^ AES_xtime[s1 ^ s2];
    state[i + 2] ^= h1 ^ AES_xtime[s2 ^ s3];
    state[i + 3] ^= h2 ^ AES_xtime[s3 ^ s0];
  }
}

static int AES_init_done = 0;

static void AES_init() {
  int i;

  for(i = 0; i < 256; i++)
    AES_Sbox_Inv[AES_Sbox[i]] = i;
  
  for(i = 0; i < 16; i++)
    AES_ShiftRowTab_Inv[AES_ShiftRowTab[i]] = i;

  for(i = 0; i < 128; i++) {
    AES_xtime[i] = i << 1;
    AES_xtime[128 + i] = (i << 1) ^ 0x1b;
  }
}

/*
   AES_ExpandKey: expand a cipher key. Depending on the desired encryption 
   strength of 128, 192 or 256 bits 'key' has to be a byte array of length 
   16, 24 or 32, respectively. The key expansion is done "in place", meaning 
   that the array 'key' is modified.
*/


// returns expanded keysize

static int AES_ExpandKey(unsigned char *key, int key_length) {
  int i, j, kl, ks, Rcon = 1;
  unsigned  char temp[4], temp0[4];

  kl = key_length;
  
  switch (kl) {
    case 16: 
      ks = 16 * (10 + 1); 
      break;
    case 24: 
      ks = 16 * (12 + 1); 
      break;
    case 32: 
      ks = 16 * (14 + 1); 
      break;
      default: 
        fprintf(stderr, "AES_ExpandKey: Only key lengths of 16, 24 or 32 bytes allowed!, key length: %d", kl);
        exit(1);
  }
  
  for(i = kl; i < ks; i += 4) {
    //var temp = key.slice(i - 4, i);
    memcpy(temp, key + i - 4, 4);

    if (i % kl == 0) {
      temp0[0] = AES_Sbox[temp[1]] ^ Rcon;
      temp0[1] = AES_Sbox[temp[2]];
      temp0[2] = AES_Sbox[temp[3]];
      temp0[3] = AES_Sbox[temp[0]]; 
      
      memcpy(temp, temp0, 4);

      if ((Rcon <<= 1) >= 256)
	Rcon ^= 0x11b;
    }
    else if ((kl > 24) && (i % kl == 16)) {
      temp[0] = AES_Sbox[temp[0]];
      temp[1] = AES_Sbox[temp[1]];
      temp[2] = AES_Sbox[temp[2]];
      temp[3] = AES_Sbox[temp[3]];
    }

    for(j = 0; j < 4; j++)
      key[i + j] = key[i + j - kl] ^ temp[j];
  }

  return ks;
}

/* 
   AES_Encrypt: encrypt the 16 byte array 'block' with the previously 
   expanded key 'key'.
*/

void AES_Encrypt(unsigned char *block, unsigned char *key, int key_length) {
  int i, l = key_length;

  AES_AddRoundKey(block, key);
  for(i = 16; i < l - 16; i += 16) {
    AES_SubBytes(block, AES_Sbox);
    AES_ShiftRows(block, AES_ShiftRowTab);
    AES_MixColumns(block);
    AES_AddRoundKey(block, key + i);
  }
  AES_SubBytes(block, AES_Sbox);
  AES_ShiftRows(block, AES_ShiftRowTab);
  AES_AddRoundKey(block, key + i);
}

/* 
   AES_Decrypt: decrypt the 16 byte array 'block' with the previously 
   expanded key 'key'.
*/

void AES_Decrypt(unsigned char *block, unsigned char *key, int key_length) {
  int i, l = key_length;

  AES_AddRoundKey(block, key + l - 16);
  AES_ShiftRows(block, AES_ShiftRowTab_Inv);
  AES_SubBytes(block, AES_Sbox_Inv);
  for(i = l - 32; i >= 16; i -= 16) {
    AES_AddRoundKey(block, key + i);
    AES_MixColumns_Inv(block);
    AES_ShiftRows(block, AES_ShiftRowTab_Inv);
    AES_SubBytes(block, AES_Sbox_Inv);
  }
  AES_AddRoundKey(block, key);
}

void aes_ecb_encrypt(unsigned char *block, int len, unsigned char *key, int key_len) {
  unsigned char expanded_key[16*(14+1)];
  int ks;

  if(!AES_init_done) {
    AES_init();
    AES_init_done = 1;
  }

  memcpy(expanded_key, key, key_len);
  ks = AES_ExpandKey(expanded_key, key_len);
  
  while(len >= 16) {
    AES_Encrypt(block, expanded_key, ks);
    len -= 16;
    block += 16;
  }
}

void aes_ecb_decrypt(unsigned char *block, int len, unsigned char *key, int key_len) {
  unsigned char expanded_key[16*(14+1)];
  int ks;

  if(!AES_init_done) {
    AES_init();
    AES_init_done = 1;
  }

  memcpy(expanded_key, key, key_len);
  ks = AES_ExpandKey(expanded_key, key_len);
  
  while(len >= 16) {
    AES_Decrypt(block, expanded_key, ks);
    len -= 16;
    block += 16;
  }
}

void aes_cbc_encrypt(unsigned char *block, int len, unsigned char *key, int key_len, unsigned char *iv) {
  unsigned char expanded_key[16*(14+1)];
  int i, ks;

  if(!AES_init_done) {
    AES_init();
    AES_init_done = 1;
  }

  memcpy(expanded_key, key, key_len);
  ks = AES_ExpandKey(expanded_key, key_len);
  
  while(len >= 16) {

    for(i = 0; i < 16; i++) {
      block[i] ^= iv[i];
    }

    AES_Encrypt(block, expanded_key, ks);
    
    iv = block;

    len -= 16;
    block += 16;
  }

}

void aes_cbc_decrypt(unsigned char *block, int len, unsigned char *key, int key_len, unsigned char *iv) {
  unsigned char expanded_key[16*(14+1)], iv0[16], iv1[16];
  int i, ks;

  if(!AES_init_done) {
    AES_init();
    AES_init_done = 1;
  }

  memcpy(expanded_key, key, key_len);

  ks = AES_ExpandKey(expanded_key, key_len);

  memcpy(iv1, iv, 16);
  
  while(len >= 16) {
    memcpy(iv0, iv1, 16);
    memcpy(iv1, block, 16);

    AES_Decrypt(block, expanded_key, ks);

    for(i = 0; i < 16; i++) {
      block[i] ^= iv0[i];
    }

    len -= 16;
    block += 16;
  }
}

void aes_ctr_decrypt(unsigned char *stream, int len, const unsigned char *key, int key_len, unsigned char *nonce, unsigned int block_lo, unsigned int block_hi) {
  unsigned char expanded_key[16*(14+1)];
  int i, ks;
  unsigned char block[16];
  unsigned char block_no[8];

  if(!AES_init_done) {
    AES_init();
    AES_init_done = 1;
  }

  memcpy(expanded_key, key, key_len);
  ks = AES_ExpandKey(expanded_key, key_len);

  for(i = 0; i < 4; i++) {
    block_no[4+i] = (unsigned char)(block_hi >> 8*i);
    block_no[i] = (unsigned char)(block_lo >> 8*i);
  }
  
  while(len > 0) {
    memcpy(block, nonce, 8);
    memcpy(block+8, block_no, 8);

    AES_Encrypt(block, expanded_key, ks);
    xor_encrypt(stream, block, len >= 16 ? 16 : len, 16);

    i = 0;
    do {
      block_no[i++]++;
    } while(i < 8 && !block_no[i-1]);

    stream += 16;
    len -= 16;
  }
}

void aes_ctr_encrypt(unsigned char *stream, int len, const unsigned char *key, int key_len, unsigned char *nonce, unsigned int block_lo, unsigned int block_hi) {
  aes_ctr_decrypt(stream, len, key, key_len, nonce, block_lo, block_hi);
}

void aes_ctr_edit(unsigned char *ciphertext, const unsigned char *key, int offset, const char *newtext) {
  unsigned char nonce[8], lead[16];
  int len, block, boffset;

  memset(nonce, 0, sizeof(nonce));
  len = strlen(newtext);
  block = offset / 16;
  boffset = offset % 16;

  memcpy(lead, ciphertext+16*block, 16);
  memcpy(ciphertext+offset, newtext, len);

  aes_ctr_encrypt(ciphertext+offset-boffset, len+boffset, key, 16, nonce, block, 0);
  memcpy(ciphertext+16*block, lead, boffset);
  
}

//////////////////////////

// based on wikipedia pseudo code

void sha1_init(unsigned int h[5]) {
  h[0] = 0x67452301;
  h[1] = 0xEFCDAB89;
  h[2] = 0x98BADCFE;
  h[3] = 0x10325476;
  h[4] = 0xC3D2E1F0;

}

void sha1_update(unsigned char *data, unsigned int h[5]) {
  unsigned int w[80];

  unsigned int a, b, c, d, e, f, temp;
  int i;

  //printf("sha1_update\n");
  //hexdump(data, 64);

  //Note 1: All variables are unsigned 32 bits and wrap modulo 2^32 when calculating
  //Note 2: All constants in this pseudo code are in big endian.
  //        Within each word, the most significant byte is stored in the leftmost byte position

    //Initialize variables:

  /*
Pre-processing:
append the bit '1' to the message
append 0 ≤ k < 512 bits '0', so that the resulting message length (in bits)
   is congruent to 448 (mod 512)
append length of message (before pre-processing), in bits, as 64-bit big-endian integer

Process the message in successive 512-bit chunks:
  */


    //break chunk into sixteen 32-bit big-endian words w[i], 0 ≤ i ≤ 15
    for(i = 0; i < 16; i++) {
      w[i] = 
        data[4*i]   << 24 |
        data[4*i+1] << 16 |
        data[4*i+2] <<  8 |
        data[4*i+3];
    }

    //Extend the sixteen 32-bit words into eighty 32-bit words:
    for( i = 16; i <  80; i++) {
      w[i] = (w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]);
      w[i] = (w[i] << 1) | (w[i] >> 31);
    }

    //Initialize hash value for this chunk:
    a = h[0];
    b = h[1];
    c = h[2];
    d = h[3];
    e = h[4];

    for(i = 0; i < 20; i++) {
      f = (b & c) | ((~b) & d);
      //      k = 0x5A827999;

      temp = ( (a << 5) | (a >> 27) ) + f + e + 0x5A827999 + w[i];
      e = d;
      d = c;
      c = (b << 30) | (b >> 2);//b leftrotate 30
      b = a;
      a = temp;
    }

    for(i = 20; i < 40; i++) {
      f = b ^ c ^ d;
      //      k = 0x5A827999;

      temp = ( (a << 5) | (a >> 27) ) + f + e + 0x6ED9EBA1 + w[i];
      e = d;
      d = c;
      c = (b << 30) | (b >> 2);//b leftrotate 30
      b = a;
      a = temp;
    }

    for(i = 40; i < 60; i++) {
      f = (b & c) | (b & d) | (c & d);
      //      k = 0x5A827999;

      temp = ( (a << 5) | (a >> 27) ) + f + e + 0x8F1BBCDC + w[i];
      e = d;
      d = c;
      c = (b << 30) | (b >> 2);//b leftrotate 30
      b = a;
      a = temp;
    }

    for(i = 60; i < 80; i++) {
      f = b ^ c ^ d;
      //      k = 0x5A827999;

      temp = ( (a << 5) | (a >> 27) ) + f + e + 0xCA62C1D6 + w[i];
      e = d;
      d = c;
      c = (b << 30) | (b >> 2);//b leftrotate 30
      b = a;
      a = temp;
    }

    // Add this chunk's hash to result so far:
    h[0] = h[0] + a;
    h[1] = h[1] + b;
    h[2] = h[2] + c;
    h[3] = h[3] + d;
    h[4] = h[4] + e;
} 

void sha1_finish(unsigned char *lastdata, int len, unsigned int h[5], unsigned char *digest) {
  unsigned char lastblock[64];
  unsigned int bits;
  int i;

  //printf("sha1_finish\n");
  //hexdump(lastdata, len % 64);

  memset(lastblock, 0, 64);

  i = len % 64;
  memcpy(lastblock, lastdata, i);
  lastblock[i] = 0x80;

  bits = len*8;

  lastblock[60] = bits >> 24;
  lastblock[61] = bits >> 16;
  lastblock[62] = bits >> 8;
  lastblock[63] = bits & 0xff;

  sha1_update(lastblock, h);

  for(i = 0; i < 5; i++) {
    digest[i*4] = (unsigned char)(h[i] >> 24);
    digest[i*4+1] = (unsigned char)(h[i] >> 16);
    digest[i*4+2] = (unsigned char)(h[i] >> 8);
    digest[i*4+3] = (unsigned char)(h[i]);
  }
}

void sha1(unsigned char *data, int len, unsigned char *digest) {
  unsigned int h[5];
  int i = 0;

  sha1_init(h);

  while(i + 64 < len) {
    sha1_update(data+i, h);
    i += 64;
  }

  sha1_finish(data + i, len, h, digest);
}

void _sha1(unsigned char *data, int len, unsigned char *digest) {
  unsigned char lastblock[64];

  unsigned int w[80];
  unsigned int h[5];
  unsigned int a, b, c, d, e, f, temp;
  int i;
  unsigned int bits;

  //Note 1: All variables are unsigned 32 bits and wrap modulo 2^32 when calculating
  //Note 2: All constants in this pseudo code are in big endian.
  //        Within each word, the most significant byte is stored in the leftmost byte position

    //Initialize variables:

  h[0] = 0x67452301;
  h[1] = 0xEFCDAB89;
  h[2] = 0x98BADCFE;
  h[3] = 0x10325476;
  h[4] = 0xC3D2E1F0;

  memset(lastblock, 0, 64);

  i = len % 64;
  memcpy(lastblock, data + len - i, i);
  lastblock[i] = 0x80;

  bits = len*8;

  lastblock[60] = bits >> 24;
  lastblock[61] = bits >> 16;
  lastblock[62] = bits >> 8;
  lastblock[63] = bits & 0xff;

  /*
Pre-processing:
append the bit '1' to the message
append 0 ≤ k < 512 bits '0', so that the resulting message length (in bits)
   is congruent to 448 (mod 512)
append length of message (before pre-processing), in bits, as 64-bit big-endian integer

Process the message in successive 512-bit chunks:
  */

  while(len >= 0) {

    if(len < 64) {
      data = lastblock;
    }

    //break chunk into sixteen 32-bit big-endian words w[i], 0 ≤ i ≤ 15
    for(i = 0; i < 16; i++) {
      w[i] = 
        data[4*i]   << 24 |
        data[4*i+1] << 16 |
        data[4*i+2] <<  8 |
        data[4*i+3];
    }

    //Extend the sixteen 32-bit words into eighty 32-bit words:
    for( i = 16; i <  80; i++) {
      w[i] = (w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]);
      w[i] = (w[i] << 1) | (w[i] >> 31);
    }

    //Initialize hash value for this chunk:
    a = h[0];
    b = h[1];
    c = h[2];
    d = h[3];
    e = h[4];

    for(i = 0; i < 20; i++) {
      f = (b & c) | ((~b) & d);
      //      k = 0x5A827999;

      temp = ( (a << 5) | (a >> 27) ) + f + e + 0x5A827999 + w[i];
      e = d;
      d = c;
      c = (b << 30) | (b >> 2);//b leftrotate 30
      b = a;
      a = temp;
    }

    for(i = 20; i < 40; i++) {
      f = b ^ c ^ d;
      //      k = 0x5A827999;

      temp = ( (a << 5) | (a >> 27) ) + f + e + 0x6ED9EBA1 + w[i];
      e = d;
      d = c;
      c = (b << 30) | (b >> 2);//b leftrotate 30
      b = a;
      a = temp;
    }

    for(i = 40; i < 60; i++) {
      f = (b & c) | (b & d) | (c & d);
      //      k = 0x5A827999;

      temp = ( (a << 5) | (a >> 27) ) + f + e + 0x8F1BBCDC + w[i];
      e = d;
      d = c;
      c = (b << 30) | (b >> 2);//b leftrotate 30
      b = a;
      a = temp;
    }

    for(i = 60; i < 80; i++) {
      f = b ^ c ^ d;
      //      k = 0x5A827999;

      temp = ( (a << 5) | (a >> 27) ) + f + e + 0xCA62C1D6 + w[i];
      e = d;
      d = c;
      c = (b << 30) | (b >> 2);//b leftrotate 30
      b = a;
      a = temp;
    }

    // Add this chunk's hash to result so far:
    h[0] = h[0] + a;
    h[1] = h[1] + b;
    h[2] = h[2] + c;
    h[3] = h[3] + d;
    h[4] = h[4] + e;

    data += 64;
    len -= 64;
  }
//Produce the final hash value (big-endian):

  //digest = hash = h0 append h1 append h2 append h3 append h4
  
  for(i = 0; i < 5; i++) {
    digest[i*4] = (unsigned char)(h[i] >> 24);
    digest[i*4+1] = (unsigned char)(h[i] >> 16);
    digest[i*4+2] = (unsigned char)(h[i] >> 8);
    digest[i*4+3] = (unsigned char)(h[i]);
  }
}

/////////////////////////////

void  generate_mac(unsigned char *msg, int msg_len, unsigned char *key, int key_len, unsigned char *mac) {
  unsigned int h[5];

  unsigned char buffer[64];

  int i = 0;

  int len = key_len + msg_len;

  sha1_init(h);

  // deal with keys longer than 512 bits
  while(i + 64 < key_len) {
    sha1_update(key+i, h);
    i += 64;
  }

  memcpy(buffer, key+i, key_len-i);
  i = key_len % 64;

  if(i + msg_len >= 64) {
    memcpy(buffer + i, msg, 64 - i);
    i = 64 - i;

    sha1_update(buffer, h);
    while(i + 64 < msg_len) {
      sha1_update(msg+i, h);
      i += 64;
    }
    memcpy(buffer, msg+i, msg_len - i);
  } else {
    memcpy(buffer + i, msg, msg_len);
  }

  sha1_finish(buffer, len, h, mac);
}

int validate_mac(unsigned char *msg, int msg_len, unsigned char *key, int key_len, unsigned char *mac) {
  unsigned char digest[20];
  
  generate_mac(msg, msg_len, key, key_len, digest);
  return !memcmp(mac, digest, 20);
}





/////////////////////////////

void mt19937_encrypt(unsigned char *data, int len, unsigned short key) {
  struct MT_generator cipher;
  int i;
  
  unsigned char stream[4];
  
  initialize_generator(&cipher, key);

  while(len > 0) {
    *(unsigned int *)&stream = extract_number(&cipher);

    if(len >= 4) {
      *data++ ^= stream[0];
      *data++ ^= stream[1];
      *data++ ^= stream[2];
      *data++ ^= stream[3];
    } else {
      for(i = 0; i < len; i++) {
	*data++ ^= stream[i];	
      }
    }
    len -= 4;
  }
  
}

void mt19937_decrypt(unsigned char *data, int len, unsigned short key) {
  mt19937_encrypt(data, len, key);
}


void generate_reset_token(char *token, int len) {
  int now = time(NULL), bytecount;
  unsigned char buffer[1024], *d; // largest token... e.g largest URL possible to handle
  struct MT_generator gen;

  bytecount = (len - 3) * 6 / 8;
  if(bytecount > 1) {
    if(bytecount + 4 > sizeof(buffer)) {
      fprintf(stderr, "Too large reset token size requested: %d, max random bytes can be: %d\n", len, sizeof(buffer));
      exit(1);
    }
    
    initialize_generator(&gen, (unsigned int)now);
    d = buffer;

    while(bytecount > 0) {
      *(unsigned int *)d = extract_number(&gen);
      d += 4;
      bytecount -= 4;

    }

    // adjust length
    d += bytecount;

    len = base64encode(buffer, d - buffer, token);
    token[len] = '\0';
  }
}

int validate_reset_token(char *token, int margin) {
  int now = time(NULL), len, bytecount, n;
  unsigned char buffer[1024], *d; // largest token... e.g largest URL possible to handle
  unsigned char btoken[1024];
  struct MT_generator gen;


  if(margin > 600) {
    fprintf(stderr, "Too large reset token time margin, max +/- 10 minutes");
    exit(1);    
  }

  if(margin < 0) {
    fprintf(stderr, "invalid time margin: %d\n", margin);
    exit(1);    
  }
  
  now -= margin;

  margin *= 2;

  len = strlen(token);

  // reset token too large, invalid
  if( (len * 6 / 8) > sizeof(btoken)) {
    return 0;
  }

  bytecount = base64decode(token, len, btoken);

  while(margin) {
    initialize_generator(&gen, (unsigned int)now);    

    d = buffer;
    n = bytecount;

    while(n > 0) {
      *(unsigned int *)d = extract_number(&gen);
      d += 4;
      n -= 4;
    }

    if(!memcmp(buffer, btoken, bytecount)) {
      return 1;
    }
    margin--;
    now++;
  }

  // invalid token
  return 0;
}


// AES in counter mode as pseudorandom source

static unsigned char random_expanded_key[16*(14+1)];
static int random_ks;
static unsigned int random_counter[4]; // 128 bit counter
static unsigned char random_nonce[16]; // 128 bit nonce

void seed_random(const unsigned char *seed, int len) {
  if(len < 32) {
    fprintf(stderr, "seed data to small, 32 bytes needed, got: %d\n", len);
    exit(1);
  }

  memcpy(random_expanded_key, seed, 16);
  random_ks = AES_ExpandKey(random_expanded_key, 16);

  memcpy(random_nonce, seed+16, 16);
  random_counter[0] = 1;
  random_counter[1] = 0;
  random_counter[2] = 0;
  random_counter[3] = 0;
}

void random_bytes(unsigned char *data, int len) {
  int i;
  unsigned char buffer[16];
  unsigned char seed[32];
  struct timeval tv;

  if(!random_ks) {
    memcpy(seed, "sdvjhrGHWenI gnERuighnER hWEN vn", 32);
    gettimeofday(&tv, NULL);
    memcpy(seed, &tv.tv_usec, sizeof(tv.tv_usec));
    seed_random(seed, 32);
  }

  while(len > 0) {
    
    // increment counter
    i = 0;
    while(i < 4) {
      random_counter[i]++;
      if(random_counter[i]) {
        break;
      }
      i++;
    }
    
    memcpy(buffer, random_counter, 16);
    for(i = 0; i < 16; i++) {
      buffer[i] ^= random_nonce[i];
    }
    
    AES_Encrypt(buffer, random_expanded_key, random_ks);
    
    memcpy(data, buffer, len > 16 ? 16 : len);
    data += 16;
    len -= 16;
  }
}

int detect_ecb(unsigned char *data, int len, int blocklen) {
  int i, j, k;

  for(i = 0; i <= len - blocklen; i+= blocklen) {
    for(j = i+blocklen; j <= len - blocklen; j += blocklen) {
      for(k = 0; k < blocklen; k++) {
        if(data[i+k] != data[j+k]) {
          break;
        }
      }
      if(k >= blocklen) {
        return 1;
      }
    }
  }
  return 0;
}

char *append(char *d, const char *s, char *e) {
  while(*s && d < e) {
    *d++ = *s++;
  }
  if(d < e) {
    *d = '\0';
  } else {
    *(d-1) = '\0';
  }
  return d;
}

int kv_parse(const char *in, char *out, int len) {
  char *end = out+len;
  char *separator = "";
  int val;

  out = append(out, "{\n  ", end);

  while(*in) {
    out = append(out, separator, end);

    while(*in && *in != '=' && out < end) {
      *out++ = *in++;
    }
    // skip =
    in++;
    if(sscanf(in, "%d", &val) == 1) {
      out += snprintf(out, end-out, ": %d", val);
      while(*in && *in != '&')
	in++;
    } else {
      out = append(out, ": \'", end);
      while(*in && *in != '&' && out < end) {
	*out++ = *in++;
      }
      out = append(out, "\'", end);
    }
    //skip &
    if(*in) {
      in++;
    }
    separator = ",\n  ";
  }

  out = append(out, "\n}\n", end);

  //printf("kv_parse, buff ok: %d, out: %p, end: %p\n", out < end, out, end);


  return out < end;
}

int profile_for(const char *email, char *out, int len) {
  char *end = out+len;

out = append(out, "email=", end);

  while(*email && out < end) {
    if(*email != '&' && *email != '=') {
      *out++ = *email++;
    } else {
      // skip illegal chars
      email++;
    }
  }
  out = append(out, "&uid=10&role=user", end);

  return (out < end);
}

static unsigned char profile_key[16];
static int profile_init = 0;

int encrypt_profile(const char *email, char *data, int len) {
  if(!profile_init) {
    random_bytes(profile_key, sizeof(profile_key));
    profile_init = 1;
  }

  if(!profile_for(email, data, len)) {
    fprintf(stderr, "Failed to generate profile, buffer to small\n");
    exit(1);
  }
  
  len = add_padding((unsigned char *)data, strlen(data), 16);
  aes_ecb_encrypt((unsigned char *)data, len, profile_key, sizeof(profile_key));
  return len;
}

void decrypt_profile(unsigned char *data, int len, char *profile, int size) {
  unsigned char *end;
  int pad = 0;
  aes_ecb_decrypt(data, len, profile_key, sizeof(profile_key));
  end = data+len;

  while(*(end-1) && *(end-1) <= 16) {
    end--;
    if(!pad) {
      pad = *end;
    } else {
      if(pad != *end) {
	fprintf(stderr, "Invalid encrypted profile\n");
	exit(1);
      }
    }
    *end = 0;
  }

  if( (data + len) - end != pad) {
    fprintf(stderr, "PL Invalid encrypted profile\n");
    exit(1);
  }

  if(!kv_parse((const char *)data, (char *)profile, size)) {
    fprintf(stderr, "PE Invalid encrypted profile");
    exit(1);
  }
}

int validate_padding(const unsigned char *data, int len) {
  int p, i;

  if(!len || len % 16 != 0) {
    return 0;
  }
  
  p = data[len-1];
  if(p < 1 || p > 16) {
    return 0;
  }
  
  for(i = len - p; i < len -1; i++) {
    if(data[i] != p) {
      return 0;
    }
  }
  return 1;
}

void initialize_generator(struct MT_generator *gen, unsigned int seed) {
  int i;

  gen->index = 0;
  gen->state[0] = seed;
  
  for(i = 1; i < 624; i++) { // loop over each other element
    //MT[i] := last 32 bits of(1812433253 * (MT[i-1] xor (right shift by 30 bits(MT[i-1]))) + i) // 0x6c078965
    gen->state[i] = (unsigned int)(((gen->state[i-1] ^ (gen->state[i-1] >> 30) )) * 1812433253 + i);
  }
}
void MT_initialize_generator(unsigned int seed) {
  initialize_generator(&MT, seed);
}

 // Generate an array of 624 untempered numbers
void generate_numbers(struct MT_generator *gen) {
  int i;
  for( i = 0; i < 624; i++)  {
    unsigned int y = (gen->state[i] & 0x80000000)                       // bit 31 (32nd bit) of MT[i]
      | (gen->state[(i+1) % 624] & 0x7fffffff);   // bits 0-30 (first 31 bits) of MT[...]
    
    gen->state[i] = gen->state[(i + 397) % 624] ^ (y >> 1);
    if( (y & 1) != 0 ){ // y is odd
      gen->state[i] = gen->state[i] ^ (0x9908b0df); //2567483615
    }
  }
}

 // Extract a tempered pseudorandom number based on the index-th value,
 // calling generate_numbers() every 624 numbers


unsigned int extract_number(struct MT_generator *gen) {
  if(gen->index == 0) {
    generate_numbers(gen);
  }
 
  unsigned int y = gen->state[gen->index];

  //  printf("untemp: %u\n", y);

  y = y ^ (y >> 11);
  y = y ^ ( (y << 7) & 0x9d2c5680); // 2636928640
  y = y ^ ( (y << 15) & 0xefc60000); // 4022730752
  y = y ^ (y >> 18);

  gen->index = (gen->index + 1) % 624;

  return y;
}

unsigned int MT_extract_number() {
  return extract_number(&MT);
}

unsigned int MT_untemper_number(unsigned int y) {
  // reverse y = y ^ (y >> 18)
  // y    aaaaaaaabbbbbbbbccccccccdddddddd
  // y18                    aaaaaaaabbbbbb

  y = y ^ (y >> 18);

   // reverse y = y ^ ( (y << 15) & 0xefc60000)

  // y    aaaaaaaabbbbbbbbccccccccdddddddd
  // y15  bccccccccdddddddd
  y = y ^ ( ((y & 0x7fff) << 15) &  0xefc60000);

  //      3              1
  //      1              6
  // y    aaaaaaaabbbbbbbbccccccccdddddddd
  // y15  bc
  y = y ^ ( ((y & 0x18000) << 15) &  0xefc60000);

  // y    aaaaaaaabbbbbbbbccccccccdddddddd
  // y7   abbbbbbbbccccccccdddddddd
  y = y ^ ( ((y & 0x7f) << 7) & 0x9d2c5680);

  //                      0011 1111 1000 0000
  //      3              1111     0
  //      1              6543210987
  // y    aaaaaaaabbbbbbbbccccccccdddddddd
  // y7   abbbbbbbbccccccccd
  y = y ^ ( ((y & 0x3f80) << 7) & 0x9d2c5680);

  //      3       2       
  //      1       3      6543210987
  // y    aaaaaaaabbbbbbbbccccccccdddddddd
  // y7   abbbbbbbbcc
  // mask         000111111100000000000000
  //                 1   f   c   0   0   0
  y = y ^ ( ((y & 0x1fc000) << 7) & 0x9d2c5680);

  //      3       2       
  //      1       3      6543210987
  // y    aaaaaaaabbbbbbbbccccccccdddddddd
  // y7   abbb
  // mask 00001111111000000000000000000000
  //         0   f   e   0   0   0   0   0          
  y = y ^ ( ((y & 0xfe00000) << 7) & 0x9d2c5680);

  // reverse  y = y ^ (y >> 11);
  // y    aaaaaaaabbbbbbbbccccccccdddddddd
  // y11             aaaaaaaabbbbbbbbccccc
  //      11111111111000000000000000000000
  //         f   f   e   0   0   0   0   0
  y = y ^ ( (y & 0xffe00000) >> 11);

  // y    aaaaaaaabbbbbbbbccccccccdddddddd
  // y11                        bbbbbccccc
  //              000111111111100000000000
  //                 1   f   f   8   0   0
  y = y ^ ( (y & 0x1ff800) >> 11);
  
  return y;
}


static unsigned char ud_key[16];
static unsigned char ud_iv[16];
static int ud_init = 0;


int add_userdata(const char *userdata, unsigned char *ciphertext, int bufflen) {
  char *prefix = "comment1=cooking%20MCs;userdata=";
  char *postfix = ";comment2=%20like%20a%20pound%20of%20bacon";

  const char *s;
  char *d;

  int len;

  if(!ud_init) {
    random_bytes(ud_key, 16);
    random_bytes(ud_iv, 16);
    ud_init = 1;
  }

  strcpy((char *)ciphertext, prefix);
  
  s = userdata;
  d = (char *)ciphertext + strlen((char *)ciphertext);

  while(*s) {
    if(*s == ';' || *s == '=') {
      d += sprintf(d, "%%%02x", *s);
    } else {
      *d++ = *s;
    }
    s++;
  }

  *d++ = '\0';
  
  // strcat(ciphertext, userdata);
  strcat((char *)ciphertext, postfix);

  len = strlen((char *)ciphertext);
  len = add_padding(ciphertext, len, 16);

  aes_cbc_encrypt(ciphertext, len, ud_key, sizeof(ud_key), ud_iv);
  
  return len;
}

int is_user_admin(unsigned char *data, int len) {
  unsigned char plaintext[1024];

  memcpy(plaintext, data, len);

  aes_cbc_decrypt(plaintext, len, ud_key, sizeof(ud_key), ud_iv);

  if(validate_padding(plaintext, len)) {
    strip_padding(plaintext, len);
    if(strstr((char *)plaintext, ";admin=true;")) {
      return 1;
    }
  }

  return 0;
}

