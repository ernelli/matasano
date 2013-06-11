#include<stdio.h>
#include<string.h>

#include "base64.h"
#include "rate.h"

int main(int argc, char *argv[]) {
  //printf("freq_table_size: %d\n", freq_table_size);
  char data[32768], binbuf[32768], testbuf[32768];
  int i, j, len;

  double score[256];

  _init();

  //len = fread(buffer, 1, sizeof(buffer), stdin);
  //buffer[len] = '\0';

  //for(i = 0; i < len; i++) {
  // buffer[i] ^= 0xff;
  //}

  if(fgets(data, sizeof(data), stdin)) {
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
      for(j = 0; j < len; j++) {
        if(!isprint(testbuf[j])) {
          testbuf[j] = '.';
        }
      }
      //printf("%02x %f %s\n", i, score[i], testbuf);
    }

    double maxs = score[0];
    int xor = 0;

    for(i = 0; i < 256; i++) {
      if(score[i] > maxs) {
        maxs = score[i];
        xor = i;
      }
    }
    
    printf("best score: %f, cipher: %02x\n", maxs, xor);
    memcpy(testbuf, binbuf, len);
    for(j = 0; j < len; j++) {
      testbuf[j] ^= xor;
    }
    testbuf[len] = '\0';
    printf("message: %s\n", testbuf);
  }
  return 0;
}
