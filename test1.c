#include<stdio.h>
#include<string.h>

#include "base64.h"

int main(int argc, char *argv[]) {
  unsigned char inbuff[3];
  char hexbuff[7], outbuff[5];
  int i, n;

  outbuff[4] = '\0';
  inbuff[6] = '\0';
  hexbuff[6] = '\0';


  while(!feof(stdin)) {
    //n = fread(inbuff, sizeof(unsigned char), 3, stdin);

    n = fread(hexbuff, sizeof(char), 6, stdin);

    if(n) {
      for(i = n; i < 6; i++) {
        // zeropad
        hexbuff[i] = '0';
      }
      hexdecode(hexbuff, inbuff);

      base64encode(inbuff, 3, outbuff);

      // pad with =
      for(i = n; i < 3; i++) {
        outbuff[1+i] = '=';
      }

      memset(inbuff, 0, 3);

      base64decode(outbuff, 4, inbuff);

      hexencode(inbuff, hexbuff, 3);
      fputs(hexbuff, stdout);

      //for(i = 0; i < 3; i++) {
      //  fputc(inbuff[i], stdout);
      //}
      //fputs(outbuff, stdout);
    }
  }
  fputc('\n', stdout);
}
