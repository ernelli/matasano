#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  int i;
    MT_initialize_generator(1337);
  //MT_initialize_generator(0);
  
  for(i = 0; i < 20; i++) {
    printf("%u\n", MT_extract_number());
  }

  return 0;
}
