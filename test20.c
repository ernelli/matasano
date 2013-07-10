#include<stdio.h>
#include<string.h>
#include"tools.h"


unsigned char cipherstrings[256][1024];
int string_len[256];


int num_strings = 0;


int main(int argc, char *argv[]) {
  unsigned char data[1024], testblock[1024], plaintext[1024], nonce[16], key[16];
  int i, j, len;

  memset(nonce, 0, 16);
  random_bytes(key, 16);

  int min_length = -1;

  while(!feof(stdin) && num_strings < sizeof(cipherstrings)/sizeof(cipherstrings[0])) {
    if(fgets(data, sizeof(data), stdin)) {
      
      len = strlen(data);
      
      if(!len) {
        break;
      }
      
      if(data[len-1] != '\n') {
        fprintf(stderr, "error, buffer too small %d\n", sizeof(data));
        return 1;
      }
      
      string_len[num_strings] = base64decode(data, len, cipherstrings[num_strings]);

      aes_ctr_encrypt(cipherstrings[num_strings], string_len[num_strings], key, sizeof(key), nonce, 0, 0);

      if(min_length == -1 || string_len[num_strings] < min_length) {
        min_length = string_len[num_strings];
      }
      
      num_strings++;
    }
  }

  // now try to break ctr stream cipher using text analysis
  // work on min_length bytes

  printf("decrypting %d bytes from %d strings\n", min_length, num_strings);
  
  unsigned char stream_key[1024];
  double best_rate = 0;

  for(i = 0; i < min_length; i++) {
    for(j = 0; j < num_strings; j++) {
      testblock[j] = cipherstrings[j][i];
      stream_key[i] = find_xor_key(testblock, num_strings, &best_rate, i == 0);
    }
  }

  // dump all text strings

  hexdump(stream_key, min_length);

  for(i = 0; i < num_strings; i++) {
    memcpy(plaintext, cipherstrings[i], string_len[i]);
    xor_encrypt(plaintext, stream_key, min_length, min_length);
    
    plaintext[min_length] = '\0';
    printf("%s\n", plaintext);

    //hexdump(plaintext, 20);
  }

  return 0;
}
