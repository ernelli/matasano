#include<stdio.h>
#include<string.h>
#include"tools.h"

int encryption_oracle_ecb_random_prefix(const unsigned char *in, int len, unsigned char *outbuff, int outlen);

int main(int argc, char *argv[]) {

  unsigned char *testdata = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  unsigned char *markdata0 = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
  unsigned char *markdata1 = "cccccccccccccccccccccccccccccccccccccccccccccccc";

  int testdata_len;
  unsigned char testbuff[256];

  unsigned char ciphertext[1024];
  unsigned char plaintext[1024];

  unsigned char testblock0[64];
  unsigned char testblock1[64];

  char hex0[128];
  char hex1[128];

  int len, blocklen;

  unsigned char block_mark[32];

  int i, j, k, l;

  testdata_len = strlen(testdata);

  len = encryption_oracle_ecb_random_prefix(markdata0, testdata_len, ciphertext, sizeof(ciphertext));

  if(!detect_ecb(ciphertext, len, blocklen)) {
    printf("Cannot decrypt non ECB encoded data\n");
    return 0;
  }

  // repeat to verify that we really find same block marker over and over
  //for(k = 0; k < 10; k++) {
    
  // detect blockmark0 pattern
    for(i = 0; i < len; i+= 16) {
      for(j = i+16; j < len; j += 16) {
        if(!memcmp(ciphertext+i, ciphertext+j, 16))  {
          memcpy(block_mark, ciphertext+i, 16);
          break;
        }
        if(j < len) {
          break;
        }
      }
    }

    len = encryption_oracle_ecb_random_prefix(markdata1, testdata_len, ciphertext, sizeof(ciphertext));

    // detect blockmark1 pattern
    for(i = 0; i < len; i+= 16) {
      for(j = i+16; j < len; j += 16) {
        if(!memcmp(ciphertext+i, ciphertext+j, 16))  {
          memcpy(block_mark+16, ciphertext+i, 16);
          break;
        }
        if(j < len) {
          break;
        }
      }
    }

    //hexdump(block_mark, 16);
    //len = encryption_oracle_ecb_random_prefix(testdata, testdata_len, ciphertext, sizeof(ciphertext));    
    //}


#if 0
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
#endif


  //int plain_len = encryption_oracle_ecb(testdata, 0, ciphertext, sizeof(ciphertext));

#if 0
  for(i = 0; i < 15; i++) {
    if(plain_len != encryption_oracle_ecb(testdata, i, ciphertext, sizeof(ciphertext))) {
      plain_len = plain_len - 16 + i;
      break;
    }
  }
  printf("ECB detected, decrypting secret %d bytes data...\n", plain_len);
#endif

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

      memcpy(testbuff, markdata0, 16);
      memcpy(testbuff+16, markdata1, 16);
      memcpy(testbuff+32, testdata, 16);
      
      do {
        len = encryption_oracle_ecb_random_prefix(testbuff, 32+i, ciphertext, sizeof(ciphertext));
        
        //printf("got cipherblock of size: %d, find block mark\n", len);

        for(l = 0; l < (len-16); l += 16) {
          if(!memcmp(ciphertext+l, block_mark, 32)) {
            break;
          }
        }

      } while(l >= (len-16) );

      //printf("Found block marker at position %d\n", l);
      //printf("cipherlen %d, start at: %d, block_offset: %d\n", len, block_start, i);

      memcpy(testblock0, ciphertext+l+32+block_start, 16);

      
      //printf("testblock0:\n");
      //hexdump(testblock0, 32);
      
      if(k < 16){
        memcpy(testblock1, testdata, i);
        memcpy(testblock1+i, plaintext, k);
      } else {
        memcpy(testblock1, plaintext + k - 15, 16);
      }

      testblock1[15] = '\0';
      
      //printf("testblock1, i=%d\n", i);
      //hexdump(testblock1, 16);
      
      //find the next character
      for(j = 0; j < 256; j++) {
        testblock1[15] = j;

        memcpy(testbuff, markdata0, 16);
        memcpy(testbuff+16, markdata1, 16);
        memcpy(testbuff+32, testblock1, 16);
        
        do {
          len = encryption_oracle_ecb_random_prefix(testbuff, 48, ciphertext, sizeof(ciphertext));
          
          for(l = 0; l < (len-16); l += 16) {
            if(!memcmp(ciphertext+l, block_mark, 32)) {
              break;
            }
          }
          
        } while(l >= (len-16));

        //printf("testblock1, found block marker at position %d\n", l);

        if(!memcmp(testblock0, ciphertext+l+32, 16)) { // found next plaintext
          plaintext[k++] = j;
          //printf("found plaintext char: %02x at %d\n", j, k);
          break;
        }
      }

      if(j == 256) {
        printf("Failed to find plaintext character at index: %d!\n", k);
        break;
      }
      
      //      if(k >= plain_len) {
      //   break;
      // }

    }

    if(j == 256) {
      // no more decryptable plaintext
      break;
    }

    plaintext[k] = '\0';
    printf("found plaintext block: %.16s\n", plaintext+block_start);
    
    //return 0;

    block_start += 16;
    
    //printf("decrypted block %d\n", block_start / 16);

  } while(block_start < len);

  plaintext[k] = '\0';
  printf("Found %d bytes plaintext:\n%s\n", k, plaintext);
  hexdump(plaintext, k);

  return 0;
}
