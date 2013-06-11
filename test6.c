#include<stdio.h>
#include<string.h>

#include "base64.h"
#include "rate.h"

static int hamming_table[256];

static int table_init = 0;
static void init_table() {
  int i;
  if(!table_init) {
    for(i = 0; i < 256; i++) {
      hamming_table[i] = (i & 1) + (i>>1 & 1) + (i>>2 & 1) + (i>>3 & 1) + (i>>4 & 1) + (i>>5 & 1) + (i>>6 & 1) + (i>>7 & 1);
    }
    table_init = 1;
  }
}

int hamming_distance(unsigned char *a, unsigned char *b, int len) {
  int dist = 0;
  init_table();
  while(len--) {
    dist += hamming_table[*a++ ^ *b++];
  }
  return dist;
}

int main(int argc, char *argv[]) {
  char inbuf[256];
  unsigned char data[32768], *dst;
  unsigned char testbuf[32768];
  int i, j, len, add;
  //printf("hamming distance: %d\n", hamming_distance("this is a test", "wokka wokka!!!", 14));

  dst = data;
  
  while(fgets(inbuf, sizeof(inbuf), stdin)) {
    len = strlen(inbuf);
    if(inbuf[len-1] == '\n') {
      len--;
    }
    dst += (add = base64decode(inbuf, len, dst));
  }

  len = dst - data;

  double keysize[50];
  int N;
  for(i = 2; i < 50; i++) {
    keysize[i] = 0;
    N = 0;
    for(j = 0; j + 2*i < len; j+= i) {
      keysize[i] += hamming_distance(data+j, data+j+i, i) / i;
      N++;
    }
    keysize[i] /= N;
    //    printf("keysize; %d, hamming distance: %f\n", i, keysize[i]);
  }

#define NUM_BEST 1

  int bestsizes[NUM_BEST];

  for(i = 0; i < NUM_BEST; i++) {
    double min = 0;
    int key = 0;
    for(j = 2; j < 50; j++) {
      if( keysize[j] != 0 && (key == 0 || keysize[j] < min) ) {
        min = keysize[j];
        key = j;
      }
    }
    bestsizes[i] = key;
    keysize[key] = 0;
    //printf("bestkey: %d, distance: %f\n", key, min);
  }

  int size;
  unsigned char *s, *d;

  unsigned char test_key[50];
  double rate;

  // test keysizes
  for(i = 0; i < NUM_BEST; i++) {
    size = bestsizes[i];
    //printf("Testing keysize: %d\n", size);
    
    for(j = 0; j < size; j++) {
      s = data + j;
      d = testbuf;

      while(s - data < len) {
        *d++ = *s;
        s += size;
      }
      // find best XOR key on testbuf
      test_key[j] = find_xor_key(testbuf, d-testbuf, &rate);
      //printf("best xor key: %02x, rate: %f\n", test_key[j], rate);
    }
    memcpy(testbuf, data, len);
    xor_encrypt(testbuf, test_key, len, size);
    
    strip_terminate(testbuf, len);
    printf("%s\n", testbuf);
    
  }
  return 0;
}
    
  

