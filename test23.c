#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  int i, wait1, wait2, seed, start;
  unsigned int val;

  random_bytes((unsigned char *)&seed, sizeof(seed));

  MT_initialize_generator(seed);

  struct MT_generator clone;

  clone.index = 0;

  for(i = 0; i < 624; i++) {
    clone.state[i] = MT_untemper_number(MT_extract_number());
  }
  
  for(i = 0; i < 10; i++)  {
    printf("original: %u clone %u\n", MT_extract_number(), extract_number(&clone));
  }

  return 0;
}
