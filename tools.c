#include<stdlib.h>
#include<stdio.h>
#include<string.h>

static char inttob64[64];
static unsigned char b64toint[256];

static int b64_init_done = 0;
static void b64_init() {
  int i, n;
  
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
    b64toint[inttob64[i]] = i;
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

    d[0] = (b64toint[s[0]] << 2) | (b64toint[s[1]] >> 4);
    d[1] = (b64toint[s[1]] << 4) | (b64toint[s[2]] >> 2);
    d[2] = (b64toint[s[2]] << 6) | (b64toint[s[3]]);
    
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


/*
struct freq_table_t {
  char letter;
  double f;
} freq_table[] = {
'a', 8.167,
'b', 1.492,
'c', 2.782,
'd', 4.253,
'e', 12.702,
'f', 2.228,
'g', 2.015,
'h', 6.094,
'i', 6.966,
'j', 0.153,
'k', 0.772,
'l', 4.025,
'm', 2.406,
'n', 6.749,
'o', 7.507,
'p', 1.929,
'q', 0.095,
'r', 5.987,
's', 6.327,
't', 9.056,
'u', 2.758,
'v', 0.978,
'w', 2.360,
'x', 0.150,
'y', 1.974,
'z', 0.074,
};
*/

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

static int freq_table_size = sizeof(freq_table)/sizeof(freq_table[0]);

static int f_init_done = 0;
static f_init() {
  int i;
  for(i = 0; i < freq_table_size; i++) {
    freq_table[i] /= 100;
  }
}


double xcorr(const double *X, const double *Y, int N) {
  int i;
  double s = 0;

  for(i = 0; i < N; i++) {
    s += X[i] * Y[i];
  }

  return s;
}

double rate_text_key(const unsigned char *s, int len, unsigned char key) {
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

  return v*xcorr(rel_freq, freq_table, 26);
}


double rate_text(const unsigned char *s, int len, int dump) {
  return rate_text_key(s, len, 0);
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


unsigned char find_xor_key(unsigned char *s, int len, double *best_rate) {
  int i;
  double score[256];

  memset(score, 0, sizeof(score));

  for(i = 0; i < 256; i++) {
    score[i] = rate_text_key(s, len, i);
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

void strip_terminate(char *data, int len) {
  int i;

  data[len] = '\0';
  for(i = 0; i < len; i++) {
    if(!isprint(data[i])) {
      data[i] = '.';
    }
  }
}

void strip_padding(char *data, int len) {
  while(len && data[len-1] <= 16) {
    len--;
    data[len] = '\0';
  }
}


void hexdump(unsigned char *data, int len) {
  int i;
  char line[60], *d;
  int m,a;
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