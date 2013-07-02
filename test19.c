#include<stdio.h>
#include<string.h>
#include"tools.h"

const char *strings[] = {
  "SSBoYXZlIG1ldCB0aGVtIGF0IGNsb3NlIG9mIGRheQ==",   
  "Q29taW5nIHdpdGggdml2aWQgZmFjZXM=",
  "RnJvbSBjb3VudGVyIG9yIGRlc2sgYW1vbmcgZ3JleQ==",
  "RWlnaHRlZW50aC1jZW50dXJ5IGhvdXNlcy4=",
  "SSBoYXZlIHBhc3NlZCB3aXRoIGEgbm9kIG9mIHRoZSBoZWFk",
  "T3IgcG9saXRlIG1lYW5pbmdsZXNzIHdvcmRzLA==",
  "T3IgaGF2ZSBsaW5nZXJlZCBhd2hpbGUgYW5kIHNhaWQ=",
  "UG9saXRlIG1lYW5pbmdsZXNzIHdvcmRzLA==",
  "QW5kIHRob3VnaHQgYmVmb3JlIEkgaGFkIGRvbmU=",
  "T2YgYSBtb2NraW5nIHRhbGUgb3IgYSBnaWJl",
  "VG8gcGxlYXNlIGEgY29tcGFuaW9u",
  "QXJvdW5kIHRoZSBmaXJlIGF0IHRoZSBjbHViLA==",
   "QmVpbmcgY2VydGFpbiB0aGF0IHRoZXkgYW5kIEk=",
  "QnV0IGxpdmVkIHdoZXJlIG1vdGxleSBpcyB3b3JuOg==",
  "QWxsIGNoYW5nZWQsIGNoYW5nZWQgdXR0ZXJseTo=",
  "QSB0ZXJyaWJsZSBiZWF1dHkgaXMgYm9ybi4=",
  "VGhhdCB3b21hbidzIGRheXMgd2VyZSBzcGVudA==",
  "SW4gaWdub3JhbnQgZ29vZCB3aWxsLA==",
  "SGVyIG5pZ2h0cyBpbiBhcmd1bWVudA==",
  "VW50aWwgaGVyIHZvaWNlIGdyZXcgc2hyaWxsLg==",
  "V2hhdCB2b2ljZSBtb3JlIHN3ZWV0IHRoYW4gaGVycw==",
  "V2hlbiB5b3VuZyBhbmQgYmVhdXRpZnVsLA==",
   "U2hlIHJvZGUgdG8gaGFycmllcnM/",
  "VGhpcyBtYW4gaGFkIGtlcHQgYSBzY2hvb2w=",
  "QW5kIHJvZGUgb3VyIHdpbmdlZCBob3JzZS4=",
  "VGhpcyBvdGhlciBoaXMgaGVscGVyIGFuZCBmcmllbmQ=",
  "V2FzIGNvbWluZyBpbnRvIGhpcyBmb3JjZTs=",
  "SGUgbWlnaHQgaGF2ZSB3b24gZmFtZSBpbiB0aGUgZW5kLA==",
  "U28gc2Vuc2l0aXZlIGhpcyBuYXR1cmUgc2VlbWVkLA==",
  "U28gZGFyaW5nIGFuZCBzd2VldCBoaXMgdGhvdWdodC4=",
  "VGhpcyBvdGhlciBtYW4gSSBoYWQgZHJlYW1lZA==",
  "QSBkcnVua2VuLCB2YWluLWdsb3Jpb3VzIGxvdXQu",
  "SGUgaGFkIGRvbmUgbW9zdCBiaXR0ZXIgd3Jvbmc=",
  "VG8gc29tZSB3aG8gYXJlIG5lYXIgbXkgaGVhcnQs",
  "WWV0IEkgbnVtYmVyIGhpbSBpbiB0aGUgc29uZzs=",
  "SGUsIHRvbywgaGFzIHJlc2lnbmVkIGhpcyBwYXJ0",
  "SW4gdGhlIGNhc3VhbCBjb21lZHk7",
  "SGUsIHRvbywgaGFzIGJlZW4gY2hhbmdlZCBpbiBoaXMgdHVybiw=",
  "VHJhbnNmb3JtZWQgdXR0ZXJseTo=",
  "QSB0ZXJyaWJsZSBiZWF1dHkgaXMgYm9ybi4="
};

int string_len[sizeof(strings)/sizeof(strings[0])];
unsigned char cipherstrings[sizeof(strings)/sizeof(strings[0])][64];


const int num_strings = sizeof(strings)/sizeof(strings[0]);


int main(int argc, char *argv[]) {
  unsigned char data[1024], testblock[1024], plaintext[1024], nonce[16], key[16];
  int i, j, len;

  memset(nonce, 0, 16);
  random_bytes(key, 16);

  int min_length = -1;
  
  printf("encrypting %d strings\n", num_strings);
  for(i = 0; i < num_strings; i++) {
    string_len[i] = base64decode(strings[i], strlen(strings[i]), cipherstrings[i]);
    
    if(min_length == -1 || string_len[i] < min_length) {
      min_length = string_len[i];
    }

    aes_ctr_encrypt(cipherstrings[i], string_len[i], key, sizeof(key), nonce, 0, 0);
    //hexdump(cipherstrings[i], string_len[i]);
  }

  // now try to break ctr stream cipher using text analysis
  // work on min_length bytes

  printf("decrypting %d bytes\n", min_length);
  
  unsigned char stream_key[1024];
  double best_rate;

  for(i = 0; i < min_length; i++) {
    for(j = 0; j < num_strings; j++) {
      testblock[j] = cipherstrings[j][i];
      stream_key[i] = find_xor_key(testblock, num_strings, &best_rate);
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
