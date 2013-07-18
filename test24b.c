#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
   unsigned char testbuff[256];
   
   generate_reset_token(testbuff, 64);
   printf("reset token: %s\n", testbuff);
   sleep(5);

   // test that the reset token is still valid (e.g within +/- 6 seconds)
   printf("token should be valid: %d\n", validate_reset_token(testbuff, 6));

   // test that the reset token is not valid (e.g not within +/- 4 seconds)
   printf("token should not be valid: %d\n", validate_reset_token(testbuff, 4));

   return 0;
}
