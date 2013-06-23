#include<stdio.h>
#include<string.h>
#include"tools.h"

int encryption_oracle_ecb(const unsigned char *in, int len, unsigned char *outbuff, int outlen);

int main(int argc, char *argv[]) {
  unsigned char *testdata = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  unsigned char testbuff[256];

  unsigned char ciphertext[256];
  unsigned char plaintext[256];

  unsigned char testblock0[64];
  unsigned char testblock1[64];

  char hex0[128];
  char hex1[128];

  int len, blocklen;

  int i, j, k;

  for(i = 3; i < 32; i++) {
    len = encryption_oracle_ecb(testdata, i, ciphertext, sizeof(ciphertext));
    memcpy(testblock1, testblock0, sizeof(testblock1));
    memcpy(testblock0, ciphertext, sizeof(testblock1));

    /*
    printf("Testblocks at len: %d\n", i);
    hexencode(testblock0, 32, hex0);
    hexencode(testblock1, 32, hex1);
    hex0[64] = '\0';
    hex1[64] = '\0';
    printf("testblock0: %s\ntestblock1: %s\n", hex0, hex1);
    */

    if(!memcmp(testblock0, testblock1, i-1)) {
      blocklen = i - 1;
      printf("blocksize: %d\n", blocklen);
    }
  }

  len = strlen(testdata);

  len = encryption_oracle_ecb(testdata, len, ciphertext, sizeof(ciphertext));

  if(!detect_ecb(ciphertext, len, blocklen)) {
    printf("Cannot decrypt non ECB encoded data\n");
    return 0;
  }

  // use 'aaaaaaaaaaaaaaaa' initially as testdata for block 0, 
  // replace with plaintext in subsequent blocks

  int block_start = 0;
  k = 0;
  
  //  b0               b1               b2               b_n
  //  aaaaaaaaaaaaaaaa cccccccccccccccc CCCCCCCCCCCCCCCC pppppppppppppppp


  //  b0               b1               b2
  //  aaaaaaaaaaaaaaac cccccccccccccccC cccccccccccccccp ppppppppppppppp

  do {

    for(i = 15; i >=0; i--) {
      // encode first ECB block with known data + part of ciphertext, also ensure that we get ciphertext shifted 1-15 bytes left
      len = encryption_oracle_ecb(testdata, i, ciphertext, sizeof(ciphertext));
      
      //printf("cipherlen %d, start at: %d, block_offset: %d\n", len, block_start, i);

      memcpy(testblock0, ciphertext+block_start, 16);
      
      //printf("testblock0:\n");
      //hexdump(testblock0, 16);
      
      if(k < 16){
        memcpy(testblock1, testdata, i);
        memcpy(testblock1+i, plaintext, k);
      } else {
        memcpy(testblock1, plaintext + k - 15, 16);
      }

      testblock1[15] = '\0';
      
      printf("testblock1, i=%d, block_start=%d\n", i, block_start);
      hexdump(testblock1, 16);
      
      //find the next character
      for(j = 0; j < 256; j++) {
        testblock1[15] = j;
        
        encryption_oracle_ecb(testblock1, 16, ciphertext, sizeof(ciphertext));
        if(!memcmp(testblock0, ciphertext, 16)) { // found next plaintext
          plaintext[k++] = j;
          //printf("found plaintext char: %02x at %d\n", j, k);
          break;
        }
      }
      if(j == 256) {
        //printf("Failed to find plaintext character at index: %d!\n", k);
        break;
      }
    }

    //printf("found plaintext: %.16s\n", plaintext+block_start);
    
    block_start += 16;
    
    //printf("decrypted block %d\n", block_start / 16);

  } while(block_start < len);

  plaintext[k] = '\0';
  printf("Found %d bytes plaintext:\n%s\n", k, plaintext);
  hexdump(plaintext, k);

  return 0;
}
