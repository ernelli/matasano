#include<stdio.h>
#include<string.h>
#include"tools.h"
       
// = 0x3d  0011 1101
// ; 0x3b  0011 1011
//          ||  |
// U 0x55  0101 0101 
// S 0x53  0101 0011
 
//   0x68  0110 1000

// |               |               |               |               |               |               
// comment1=cooking%20MCs;userdata=;comment2=%20like%20a%20pound%20of%20bacon

// |               |               |               |               |               |               |         pppppp
// comment1=cooking%20MCs;userdata=mesostrashymydatfoooa;admin=true;comment2=%20like%20a%20pound%20of%20bacon


int main(int argc, char *argv[]) {
  char plaintext[256];
  unsigned char *dst, data[32768];
  char hex[256];
  int add, len;

  char *userdata = "mesostrashymydatfoooaSadminUtrue";

  len = add_userdata("data;admin=true", data, sizeof(data));
  printf("simple attack, user_is_admin: %s\n", is_user_admin(data, len) ? "true" : "false");

  len = add_userdata(userdata, data, sizeof(data));

  data[32+5] ^= 0x68;
  data[32+11] ^= 0x68;

  printf("real attack, user_is_admin: %s\n", is_user_admin(data, len) ? "true" : "false");
}
