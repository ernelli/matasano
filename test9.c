#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  char indata[32768];
  int len;

  fgets(indata, sizeof(indata), stdin);
  
  len = strlen(indata);
  
  printf("indata len: %d\n", len);
  hexdump(indata, len);

  //add padding
  len = add_padding(indata, len, 20);

  printf("padded message:\n");
  hexdump(indata, len);

  return 0;
}
