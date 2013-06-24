#include<stdio.h>
#include<string.h>
#include"tools.h"

int encryption_oracle_ecb_random_prefix(const unsigned char *in, int len, unsigned char *outbuff, int outlen);

int main(int argc, char *argv[]) {

  unsigned char *testdata = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  unsigned char *markdata0 = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
  unsigned char *markdata1 = "cccccccccccccccccccccccccccccccccccccccccccccccc";

  int testdata_len;

  // prepend a prefix_len number of random/junk bytes to our testdata to avoid getting
  // stuck if encryption_oracle is not prepending a random size data... or prepends random sized
  // data which does not cover all % 16 possible sizes, e.g, random prefixed data of 2-3 different
  // lengths. Use one len variable for each step in decryption process so that previous successful
  // value can be reused in the case that encryption oracle prefixes with the same data/size every
  // time (16 times speedup in that case).

  unsigned char prefix_len0 = 0;
  unsigned char prefix_len1 = 0;

  unsigned char testbuff[1024];
  unsigned char testbuff_p[1024];

  unsigned char ciphertext[1024];
  unsigned char plaintext[1024];

  unsigned char testblock0[64];
  unsigned char testblock1[64];

  char hex0[128];
  char hex1[128];

  int len, blocklen;

  unsigned char block_mark[32];

  int i, j, k, l;

  const int *letter_test_table;

  // test letters using a letter frequency table instead of testing all letters 0..255.
  // table contains a-z, A-Z, punct and numbers then remaining bytes 0..255 in ascending order
  // result in a speedup *3
  letter_test_table = get_letter_test_table();

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
      
      // repeat until we get a block aligned encryption result
      do {
        memcpy(testbuff_p+prefix_len0, testbuff, 32+i);
        
        len = encryption_oracle_ecb_random_prefix(testbuff_p, prefix_len0+32+i, ciphertext, sizeof(ciphertext));
        
        //printf("got cipherblock of size: %d, find block mark\n", len);

        for(l = 0; l < (len-16); l += 16) {
          if(!memcmp(ciphertext+l, block_mark, 32)) {
            break;
          }
        }

        // if not found, try new prefix size
        if(l >= (len-16) ) {
          random_bytes(&prefix_len0, 1);
          prefix_len0 &= 0xf;
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
        testblock1[15] = letter_test_table[j];

        memcpy(testbuff, markdata0, 16);
        memcpy(testbuff+16, markdata1, 16);
        memcpy(testbuff+32, testblock1, 16);

        do {
          memcpy(testbuff_p+prefix_len1, testbuff, 48);
          
          // intially try again with last prefix size, if encryption_oracle uses fixed size random_prefix we 
          // can speed up the decryption a lot by using the same prefix in our test data.
          len = encryption_oracle_ecb_random_prefix(testbuff_p, prefix_len1+48, ciphertext, sizeof(ciphertext));
          
          for(l = 0; l < (len-16); l += 16) {
            if(!memcmp(ciphertext+l, block_mark, 32)) {
              break;
            }
          }

          // if not found, try new prefix size
          if(l >= (len-16)) {
            random_bytes(&prefix_len1, 1);
            prefix_len1 &= 0xf;
          }
          
        } while(l >= (len-16));

        //printf("testblock1, found block marker at position %d\n", l);

        if(!memcmp(testblock0, ciphertext+l+32, 16)) { // found next plaintext
          plaintext[k++] = letter_test_table[j];
          //printf("found plaintext char: %02x at %d\n", j, k);
          break;
        }
      }

      if(j == 256) {
        //printf("Failed to find plaintext character at index: %d!\n", k);
        break;
      }
      
    }

    if(j == 256) {
      // no more decryptable plaintext
      break;
    }

    //return 0;

    block_start += 16;
    
    //printf("decrypted block %d\n", block_start / 16);

  } while(block_start < len);

  k = strip_padding(plaintext, k);

  printf("Found %d bytes plaintext:\n%s\n", k, plaintext);
  //hexdump(plaintext, k);

  return 0;
}
