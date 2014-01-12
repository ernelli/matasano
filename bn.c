#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include<stdarg.h>
#include<stdlib.h>

#include"tools.h"

#define USE_BIGNUM
//#define DEBUG 
#define TEST_BIGNUM

struct bignum
{
  int size;
  int n;
  int sign;
  unsigned int *num;
};

void bignum_print(struct bignum *a);
void bignum_print_hex(struct bignum *a);
void bignum_mul_u32(struct bignum *a, unsigned b);

struct bignum * bignum_clear(struct bignum *a)
{
  memset(a->num, 0, a->size*sizeof(int));
}

int bignum_is_zero(struct bignum *a)
{
  int a_n = a->n-1;

  while(a_n > 0 && a->num[a_n] == 0)
    a_n--;

  if(a_n == 0 && a->num[a_n] == 0)
    return 1;
  else
    return 0;
}


struct bignum * bignum_alloc(int size)
{
  struct bignum *a = (struct bignum *)malloc(sizeof(struct bignum));
  a->size = size;
  a->num = (unsigned int *)malloc(a->size*sizeof(int));
  a->n = 1;
  a->num[0] = 0;
  a->sign = 1;

  return a;
}

void bignum_set_i32(struct bignum *a, int value)
{
  a->n = 1;
  if(value < 0) 
    {
      a->num[0] = -value;
      a->sign = -1;
    }
  else
    {
      a->num[0] = value;
      a->sign = 1;
    }
}

int bignum_get_int(struct bignum *a)
{
  if(a->n > 1 || a->num[0] & 0x80000000)
    {
      printf("bignum_get_int, failed size > 2^31-1, a->n=%d, num[0] = %u", a->n, a->num[0]);
      exit(2);
    }
  return a->sign*a->num[0];
}

unsigned bignum_get_unsigned(struct bignum *a)
{
  if(a->n > 1)
    {
      printf("bignum_get_unsigned, failed size > 2^31-1, a->n=%d, ", a->n);
      bignum_print(a);
      exit(3);
    }
  return a->num[0];
}

struct bignum * bignum_create()
{
  struct bignum *a = bignum_alloc(128);
  bignum_set_i32(a,0);
  return a;
}

struct bignum * bignum_create_i32(int value)
{
  struct bignum *a = bignum_alloc(4);
  bignum_set_i32(a,value);
  return a;
}

void bignum_free(struct bignum *a)
{
  free(a->num);
  free(a);
}

void bignum_assign(struct bignum *a, struct bignum *b)
{
  if(a->size < b->size)
    {
      free(a->num);
      a->num = (unsigned int *)malloc(b->size*sizeof(unsigned));
      a->size = b->size;
    }
  memcpy(a->num, b->num, b->n*sizeof(int));
  a->sign = b->sign;
  a->n = b->n;
}

struct bignum *bignum_copy(struct bignum *a)
{
  struct bignum *b = bignum_alloc(a->size);
  bignum_assign(b,a);
  return b;
}


void bignum_resize(struct bignum *a, int size)
{
  if(a->size > size)
    return;
  
  a->size = size;
  a->num = realloc(a->num, a->size*sizeof(int));
}

void bignum_inc(struct bignum *a)
{
  int n = 0;
  do
    {
      if(a->size <= n)
	bignum_resize(a, a->n+1);

      if(a->n <= n)
	a->num[a->n++] = 0;

      a->num[n]++;

    } while(a->num[n++] == 0);
}

void bignum_neg(struct bignum *a)
{
  a->sign *= -1;
}

void bignum_neg_unsigned(struct bignum *a)
{
  int n;
  for(n = 0; n < a->n; n++)
    a->num[n] = ~a->num[n];

  n = a->n; // workaround to handle 0xffff... + 1 = 0x1000...
  bignum_inc(a);
  a->n = n; // Which happends when we negate 0.
}

void bignum_sub_unsigned(struct bignum *a, struct bignum *b)
{
  int n;
  
  unsigned int c = 0;

  if(a->size < (b->n+1))
    bignum_resize(a, b->n+1);

  for(n = 0; n < b->n; n++)
    {
      unsigned int c0 = c;

      c = 0;

      unsigned int r = a->num[n] - b->num[n];
      if(r > a->num[n])
	c = 1;
      r = r - c0;
      if(r > a->num[n])
	c = 1;
      a->num[n] = r;
    }

  while(c && a->n > n)
    {
      a->num[n] -= c;

      if(a->num[n] == 0xffffffff)
	c = 1;

      n++;
    }

}

void bignum_add_unsigned(struct bignum *a, struct bignum *b)
{
  int n;

  unsigned int c = 0;
  long long int s = 0;

  for(n = 0; n < b->n; n++)
    {
      if(a->n <= n)
	a->num[a->n++] = 0;

      s = (long long int)a->num[n] + (long long int)b->num[n] + (long long int)c;
      a->num[n] = (unsigned int)s;
      if(s >= 0x100000000LLU)
	c = 1;
      else
        c = 0;
    }
 
  while(c)
    {
      if(a->n <= n)
	{
	  if(n >= a->size)
	    bignum_resize(a,a->size+1);
	  a->num[a->n++] = 0;
	}
      a->num[n] += c;
      c = 0;
      if(a->num[n] == 0)
	{
	  c = 1;
	}
      n++;
    }
  
}

void bignum_add_unsigned_shifted(struct bignum *a, struct bignum *b, int sl)
{
  int n;

  if(sl % 32 != 0) {
    fprintf(stderr, "bignum_add_unsigned_shifted: unaligned shifts not supported yet, sl = %d\n", sl);
    exit(1);
  }

  int offset = sl / 32;
  if(a->size < 1 + b->n + offset) {
    bignum_resize(a,1 + b->n + offset);
  }

  for(n = a->n; n < 1 + b->n+offset; n++) {
    a->num[n] = 0;
  }

  unsigned int c = 0;
  long long int s = 0;

  //  printf("offset: %d, a->n: %d, b->n %d\n", offset, a->n, b->n);
  //  printf("a: "); bignum_print_hex(a); printf("\n");
  //  printf("b: "); bignum_print_hex(b); printf("\n");

  for(n = 0; n < b->n; n++)
    {
      s = (long long int)a->num[n+offset] + (long long int)b->num[n] + (long long int)c;
      a->num[n+offset] = (unsigned int)s;
      if(s >= 0x100000000LLU) 
	c = 1;
      else
        c = 0;
    }
 
  //  printf("+: "); bignum_print_hex(a); printf("\n");

  while(c)
    {
      a->num[n+offset] += c;
      c = 0;
      if(a->num[n+offset] == 0) {
        c = 1;
      }
      n++;
    }

  if(a->n < n+offset) {
    a->n = n+offset;
  }
  //  printf("S: "); bignum_print_hex(a); printf("\n");

}


//unsigned compare
int bignum_cmp_unsigned(struct bignum *a, struct bignum *b)
{
  int a_n = a->n-1;
  int b_n = b->n-1;

  while(a_n > 0 && a->num[a_n] == 0)
    a_n--;

  while(b_n > 0 && b->num[b_n] == 0)
    b_n--;

  if(a_n >= 0 && b_n >= 0)
    {
      // equal length
      if(a_n == b_n)
	{
	  int n = a_n;
	  while(a->num[n] == b->num[n] && (n > 0) )
	    n--;
	  
	  if(a->num[n] == b->num[n])
	    return 0;
	  else
	    if(a->num[n] > b->num[n])
	      return 1;
	  return -1;
	}
      else
	if(a_n > b_n) // a larger than b
	  {
	      return 1;
	  }
	else // b_n > a_n
	  {
	      return -1;
	  }
    }
  
  if(a_n >= 0)
    return 1;
  else
    return -1;
}


// signed compare
int bignum_cmp(struct bignum *a, struct bignum *b)
{
  int cmp = bignum_cmp_unsigned(a,b);

  if(cmp != 0)
    {
      // case 1
      // a > b   -a  -b    a < b   cmp = -1
      // a > b   -a   b    a < b   cmp = -1
      // a > b    a  -b    a > b   cmp =  1
      // a > b    a   b    a > b   cmp =  1

      // case 2
      // a < b   -a  -b    a > b   cmp =  1
      // a < b   -a   b    a < b   cmp = -1
      // a < b    a  -b    a > b   cmp =  1
      // a < b    a   b    a < b   cmp = -1

      if(cmp > 0) // case 1
        cmp = a->sign;
      else        // case 2
        cmp = -b->sign;
    }
  return cmp;
}

void bignum_add(struct bignum *a, struct bignum *b)
{
  if(a->sign * b->sign == -1)
    {
      if(bignum_cmp_unsigned(a,b) > 0) // a larger than b
	{
	  bignum_sub_unsigned(a, b); // subtract b from a, keep a's sign
	}
      else // subtract a from b
	{
	  int size = a->n;
	  bignum_neg_unsigned(a);
	  bignum_add_unsigned(a,b);
	  a->n = size;
	  a->sign = b->sign;
	}
      //printf("done, res: "); bignum_print(a); printf("\n");

    }
  else // same sign, add b onto a
    {
      if(a->size < (b->n+1))
	bignum_resize(a, b->n+1);
      bignum_add_unsigned(a,b);
    }

}

void bignum_sub(struct bignum *a, struct bignum *b)
{
  // do it the easy way
  b->sign = -b->sign;
  bignum_add(a,b);
  b->sign = -b->sign;
}

//         ABCD
//         EFGH*
//        -----
//       H*ABCD
//      G*ABCD
//     F*ABCD
//    E*ABCD   +
//-------------
//     ABCDEFGH
//

void bignum_mul(struct bignum *a, struct bignum *b) {
  // allocate bignum large enough to hold the product
  struct bignum *prod = bignum_alloc(1 + a->n + b->n);
  struct bignum *part = bignum_alloc(1 + a->n);

  int n;

  for(n = 0; n < b->n; n++) {
    bignum_assign(part, a);
    bignum_mul_u32(part, b->num[n]);
    bignum_add_unsigned_shifted(prod, part, n*32);

    //    printf("multiplier: %u\n", b->num[n]);
    // printf("part: %d ", n); bignum_print(part); printf("\n");
    //printf("prod: %d ", n); bignum_print(prod); printf("\n");

  }

  prod->sign = a->sign * b->sign;

  bignum_assign(a, prod);
  
  bignum_free(prod);
  bignum_free(part);
}

void bignum_add_u32(struct bignum *a, unsigned int val)
{
  int n;

  unsigned int c = 0;
  long long int s = 0;

  n = 0;

  if(a->n <= n)
    a->num[a->n++] = 0;

  s = (long long int)a->num[n] + (long long int)val;
  a->num[n] = (unsigned int)s;
  if(s >= 0x100000000LLU)
    c = 1;

  n++;

  while(c)
    {
      if(a->n <= n)
	{
	  if(n >= a->size)
	    bignum_resize(a,a->size+1);
	  a->num[a->n++] = 0;
	}
      a->num[n] += c;
      c = 0;
      if(a->num[n] == 0)
	{
	  c = 1;
	}
      n++;
    }
}


void bignum_mul_u32(struct bignum *a, unsigned b)
{
  int n;

  long long unsigned p;
  long long unsigned c = 0;

  for(n = 0; n < a->n; n++)
    {
      p = (long long unsigned)a->num[n] * (long long unsigned)b + c;
      a->num[n] = (unsigned)p;
      c = p / 0x100000000LLU;
    }
  if(c != 0)
    {
      if(a->size <= n)
	bignum_resize(a,a->size+1);	
      a->n++;
      a->num[n] = (unsigned)c;
    }
}

void bignum_mul_i32(struct bignum *a, int b) { 
  int bs = 1;
  if(b < 0) {
    b = -b;
    bs = -1;
  }
  bignum_mul_u32(a, b);
  a->sign *= bs;
}

unsigned int bignum_div_i32(struct bignum *a, int b)
{
  if(b == 0)
    {
      printf("Division by zero\n");
      exit(1);
    }

  int bs = 1;
  if(b < 0)
    {
      b = -b;
      bs = -1;
    }

  int n = a->n-1;

  long long unsigned r = 0;
  long long unsigned q;

  while(n >= 0)
    {
      long long unsigned div =  (a->num[n] + r*0x100000000LLU);
      q = div / (long long unsigned)b;
      r = div % (long long unsigned)b;

      if(q >= 0x100000000LLU)
	{
	  printf("Division overflow: %llu\n", q);
	  exit(1);
	}
      a->num[n] = (unsigned int)q;
      n--;
    }

  while(a->n > 1 && (a->num[a->n-1] == 0) )
    a->n--;

  if(a->n == 1 && a->num[0] == 0)
    a->sign = 1;
  else
    a->sign = a->sign * bs;

  return (unsigned)r;
}



void bignum_print(struct bignum *a)
{
  if(a->sign < 0)
    printf("-");
  
  unsigned char *digits;

  digits = (unsigned char *)malloc(30*a->n);

  unsigned char *num = digits;

  if(!bignum_is_zero(a))
    {

      struct bignum *b = bignum_copy(a);

      while(!bignum_is_zero(b))
	{
	  unsigned r = bignum_div_i32(b,10);
          
          //          printf("%d", r);

	  *num++ = r;
	}

      while(num-- > digits)
	printf("%d", (unsigned int)*num);

      bignum_free(b);
    }
  else
    printf("0");

  free(digits);
}

void bignum_print_hex(struct bignum *a) {
  int i;

  for(i = a->n - 1; i >= 0; i--) {
    printf("%10u ", a->num[i]);
  }
}

void bignum_parse(struct bignum *a, unsigned char *str) {
  bignum_set_i32(a, 0);

  while(*str && isdigit(*str)) {
    bignum_mul_u32(a,10);
    bignum_add_u32(a, *str - '0');
    str++;
  }
}

#ifdef TEST_BIGNUM

int main(int argc, char *argv[]) {
  int i;

  //printf("sizeof long long unsigned: %d\n", sizeof(long long unsigned));

  struct bignum *a = bignum_alloc(128);
  struct bignum *b = bignum_alloc(128);

  if(argc >= 3) {
    bignum_parse(a, argv[1]);
    bignum_parse(b, argv[2]);
    
    //printf("p a: "); bignum_print(a); printf("\n");
    //printf("p b: "); bignum_print(b); printf("\n");
    bignum_mul(a,b);
    //printf("a*b: "); 
    bignum_print(a); printf("\n");
  } else {
    a->n = a->size;
    random_bytes((unsigned char *)a->num, a->n*4);
    b->n = b->size;
    random_bytes((unsigned char *)b->num, b->n*4);
    printf("a: "); bignum_print(a); printf("\n");
    printf("b: "); bignum_print(b); printf("\n");
    bignum_mul(a,b);
    printf("a*b: "); bignum_print(a); printf("\n");

    /*
    a->num[0] = 123456789;
    a->n = 1;
    
    b->num[0] = 123456789;
    b->n = 1;
    
    bignum_mul(a,b);
    bignum_mul(a,b);
    
    bignum_print(a); printf("\n");
    */
  }

  return 0;
}

#endif
