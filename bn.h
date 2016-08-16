#ifndef BN_H
#define BN_H

struct bignum
{
  int size;
  int n;
  int sign;
  unsigned int *num;
};

void bignum_print(struct bignum *a);
void bignum_print_hex(struct bignum *a);
void bignum_print_parts(struct bignum *a);
void bignum_mul_u32(struct bignum *a, unsigned b);
void bignum_mul(struct bignum *a, struct bignum *b);



#endif
