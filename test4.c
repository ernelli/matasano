#include<stdio.h>
#include<string.h>

#include "base64.h"
#include "rate.h"

int main(int argc, char *argv[]) {
  //printf("freq_table_size: %d\n", freq_table_size);
  char data[32768], binbuf[32768], testbuf[32768];
  int i, j, len;

  char bestline[32768];
  double total_bestscore = 0;
  int best_xor = 0;
  int best_len = 0;
  char outbuf[32768];
  

  double score[256];

  _init();

  //len = fread(buffer, 1, sizeof(buffer), stdin);
  //buffer[len] = '\0';

  //for(i = 0; i < len; i++) {
  // buffer[i] ^= 0xff;
  //}
  
  

  while(fgets(data, sizeof(data), stdin)) {
    len = strlen(data) - 1;
    data[len] = '\0';
    len /= 2;

    hexdecode(data, binbuf);

    memset(score, 0, sizeof(score));
    for(i = 0; i < 256; i++) {
      memcpy(testbuf, binbuf, len);
      for(j = 0; j < len; j++) {
        testbuf[j] ^= i;
      }
      testbuf[j] = '\0';
      //printf("%02x %s\n", i, testbuf);

      //score[i] = rate_text(testbuf, len, i == 0x1d || i == 0x58 || i == 0x52);
      score[i] = rate_text(testbuf, len, 0);
      /*
      for(j = 0; j < len; j++) {
        if(!isprint(testbuf[j])) {
          testbuf[j] = '.';
        }
      }
      printf("%02x %f %s\n", i, score[i], testbuf);
      */
    }

    double maxs = score[0];
    int xor = 0;

    for(i = 0; i < 256; i++) {
      if(score[i] > maxs) {
        maxs = score[i];
        xor = i;
      }
    }
    
    memcpy(testbuf, binbuf, len);
    for(j = 0; j < len; j++) {
      testbuf[j] ^= xor;
    }
    testbuf[len] = '\0';
      for(j = 0; j < len; j++) {
        if(!isprint(testbuf[j])) {
          testbuf[j] = '.';
        }
      }
      printf("best score: %f, cipher: %02x, message: %s\n", maxs, xor, testbuf);
      //printf("message: %s\n", testbuf);

      if(maxs > total_bestscore) {
        total_bestscore = maxs;
        memcpy(bestline, binbuf, len);
        best_xor = xor;
        best_len = len;
      }
  }
  
  hexencode(bestline, outbuf, best_len);
  outbuf[2*best_len] = '\0';
  printf("bestline: %s\n", outbuf);
  for(i = 0; i < best_len; i++) {
    bestline[i] ^= best_xor;
  }
  bestline[i] = '\0';
  printf("message: %s\n", bestline);
}
