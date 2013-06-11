#include<stdio.h>
#include<string.h>

int main(int argc, char *argv[]) {
  char inbuf[4096];
  unsigned char data[32768], *dst;
  unsigned char testbuf[32768];
  int i, j, k, len, hit;
  //printf("hamming distance: %d\n", hamming_distance("this is a test", "wokka wokka!!!", 14));

  while(fgets(inbuf, sizeof(inbuf), stdin)) {

    len = hexdecode(inbuf, data);

    hit = 0;

    for(i = 0; i <= len - 16; i+= 16) {
      for(j = i+16; j <= len - 16; j += 16) {
        for(k = 0; k < 16; k++) {
          if(data[i+k] != data[j+k]) {
            break;
          }
        }
        if(k >= 16) {
          /*
          hexencode(data+i, testbuf, 16);
          printf("Same ciphertext at pos %d and pos %d, %.32s\n", i, j, testbuf); 
          hexencode(data+j, testbuf, 16);
          printf("Same ciphertext at pos %d and pos %d, %.32s\n", i, j, testbuf); 
          */
          hit = 1;
        }
      }
    }
    
    if(hit) {
      printf("%s\n", inbuf);
    }
    
  }
  return 0;
}
