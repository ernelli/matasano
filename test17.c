#include<stdio.h>
#include<string.h>
#include"tools.h"

// dec ^ iv = plain

// dec ^ tv = 0x01

// 0x01 ^ tv = dec

int main(int argc, char *argv[]) {
  unsigned char data[1024], testblock[1024], plaintext[1024];

  unsigned char iv[16]; // = { 0x85 ,0x89 ,0x55 ,0xcb ,0x16 ,0x2d ,0xd4 ,0xa9,  0xbf ,0x30 ,0x26 ,0x94 ,0x2d ,0xd2 ,0xd9 ,0xc7 };

  int i, j, len, block_offset = 0, pad;

  
  len = cbc_padding_oracle(data, sizeof(data), iv);

  //hexdump(data, len);
  //printf("decrypting\n");
  
  while(block_offset  < len) {
    memset(testblock, 0, 16);
    memcpy(testblock+16, data+block_offset, 16);

    do {
      // keep changing last byte until we get a block with valid padding
      while(!cbc_padding_oracle_validate(testblock, 32)) {
        testblock[15]++;
      }

      // Now change byte 15 in testblock and check that padding still valid, e.g ends with 0x01
      // otherwise repeat search for valid padding again by repeatedly changing last byte.
      // since it our testblock after decryption ends with { 0x02,  xx  } and we keep changing xx 
      // until it becomes 0x02 then we also get valid padding but not with the last byte as 0x01

      testblock[14]++;

    } while(!cbc_padding_oracle_validate(testblock, 32));

    i = 15;
    pad = 0x01;
    
    // now repeat for bytes 15..1

    do {
      // store the decrypted byte in plaintext
      plaintext[block_offset+i] = testblock[i] ^ pad;

      // change block from ending with 0x01 to { xx,  0x02 } -> { xx, 0x03, 0x03 } and so forth...
      for(j = i; j < 16; j++) {
        testblock[j] = testblock[j] ^ pad ^ (pad+1);
      }

      i--;
      pad++;

      // keep testing until xx becomes 0x02, 0x03...
      while(!cbc_padding_oracle_validate(testblock, 32)) {
        testblock[i]++;
      }

    } while(i > 0);

    plaintext[block_offset+i] = testblock[i] ^ pad;

    // now plaintext contains a decrypted block, but not xor'ed with iv or previous block

    if(block_offset >= 16) {
      // xor with precious block
      xor_encrypt(plaintext+block_offset, data + block_offset-16, 16, 16);
    } else {
      // xor with IV
      xor_encrypt(plaintext+block_offset, iv, 16, 16);
    }

    //hexdump(plaintext+block_offset, 16);
    //printf("plaintext: %.16s\n", plaintext);
  
    block_offset += 16;
  }

  //hexdump(plaintext, len);
  strip_padding(plaintext, len);
  printf("%s\n", plaintext);

  return 0;
}
