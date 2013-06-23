#include<stdio.h>
#include<string.h>
#include"tools.h"

int main(int argc, char *argv[]) {
  const char *valid1 = "ICE ICE BABY\x04\x04\x04\x04";
  const char *valid2 = "YELLOW SUBMARIN\x01";

  const char *invalid1 = "ICE ICE BABY\x05\x05\x05\x05";
  const char *invalid2 = "ICE ICE BABY\x01\x02\x03\x04";
  const char *invalid3 = "YELLOW SUBMARIN\x02";

  printf("valid: %d\n", validate_padding(valid1, 16));
  printf("valid: %d\n", validate_padding(valid2, 16));
  printf("invalid: %d\n", validate_padding(invalid1, 16));
  printf("invalid: %d\n", validate_padding(invalid2, 16));
  printf("invalid: %d\n", validate_padding(invalid3, 16));
}
