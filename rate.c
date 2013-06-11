#include<stdio.h>
#include<string.h>


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

static int init_done = 0;
static init() {
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

double rate_text(const unsigned char *s, int len, int dump) {
  int i, N = 0, n = 0, v = 0, c, freq[26];
  double rel_freq[26];

  if(!init_done) {
    init();
    init_done = 1;
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
}

double rate_text_key(const unsigned char *s, int len, unsigned char key) {
  int i, N = 0, n = 0, v = 0, c, freq[26];
  double rel_freq[26];

  if(!init_done) {
    init();
    init_done = 1;
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
