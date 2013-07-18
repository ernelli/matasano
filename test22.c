#include<stdio.h>
#include<string.h>
#include<signal.h>
#include"tools.h"

void SIGINT_handler() {
  printf("interrupt sleep\n\n");
}

int main(int argc, char *argv[]) {
  int i, wait1, wait2, seed, start;
  unsigned int value;

  signal(SIGINT, SIGINT_handler);

  if(argc < 2) {
    random_bytes((unsigned char *)&wait1, sizeof(wait1));
    wait1 &= 0x7fffffff;
    wait1 = wait1 % 960 + 40;
    
    random_bytes((unsigned char *)&wait2, sizeof(wait2));
    wait2 &= 0x7fffffff;
    wait2 = wait2 % 960 + 40;
    
    printf("about to spend %d seconds running bad crypto code, OK?\n", wait1 + wait2);
    
    if(sleep(wait1)) {
      printf("sleep was interrupted\n");
    }
    
    seed = time(NULL);  
    FILE *fp = fopen("seed", "w");
    fprintf(fp, "%d", seed);
    fclose(fp);
    
    MT_initialize_generator(seed);
    
    sleep(wait2);  
    
    printf("secret: %u\n", MT_extract_number());
  } else {
    sscanf(argv[1], "%u", &value);
    if(argc > 2)
      sscanf(argv[2], "%d", &start);
    else
      start = time(NULL);

    printf("brute force search for seed generating %u, starting at %d\n", value, start);

    do {
      MT_initialize_generator(start--);
    } while(MT_extract_number() != value);
    start++;
    printf("generator seeded with: %d\n", start);
  }

  return 0;
}
