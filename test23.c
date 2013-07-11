#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  int i, wait1, wait2, seed, start;
  unsigned int val;

  MT_initialize_generator(1337);

  for(i = 0; i < 10; i++) {
    val =MT_extract_number();
    printf("revers: %u\n", MT_reverse_number(val));
  }
  

  return 0;
}
