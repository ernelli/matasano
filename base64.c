static char inttob64[64];
static unsigned char b64toint[256];

static int init_done = 0;
static void init() {
  int i, n;
  
  if(init_done) {
    return;
  }

  for(i = 0; i < 10; i++) {
    inttob64[i] = 'A' + i;
    inttob64[i+26] = 'a' + i;
    inttob64[i+52] = '0' + i;
  }

  for(; i < 26; i++) {
    inttob64[i] = 'A' + i;
    inttob64[i+26] = 'a' + i;
  }

  inttob64[62] = '+';
  inttob64[63] = '/';

  for(i = 0; i < 64; i++) {
    b64toint[inttob64[i]] = i;
  }
  
  init_done = 1;
}


//   s0       s1       s2    
// 76543210 76543210 76543210

//   s0     s0  s1     s1  s2     s2
// 765432 | 10 7654 | 3210 76 | 543210

void base64encode(const unsigned char *s, int len, char *d) {
  init();
  
  while(len >= 3) {

    d[0] = inttob64[s[0] >> 2];
    d[1] = inttob64[( (s[0] << 4) | (s[1] >> 4) ) & 0x3f];
    d[2] = inttob64[( (s[1] << 2) | (s[2] >> 6) ) & 0x3f];
    d[3] = inttob64[ s[2] & 0x3f];
   
    len -= 3;
  }
}

//   s0     s1     s2     s3
// 543210 543210 543210 543210

//   s0   s1   s1   s2    s2   s3
// 543210 54| 3210 5432 | 10 543210


int base64decode(const char *s, int len, unsigned char *d) {
  unsigned char *d0 = d;
  init();

  while(len >= 4) {

    d[0] = (b64toint[s[0]] << 2) | (b64toint[s[1]] >> 4);
    d[1] = (b64toint[s[1]] << 4) | (b64toint[s[2]] >> 2);
    d[2] = (b64toint[s[2]] << 6) | (b64toint[s[3]]);
    
    d += 3 - (s[2] == '=' ? 2 : (s[3] == '=' ? 1 : 0));
    s += 4;
    len -= 4;
  }

  return d - d0;
}

#define hex2int(c) ((c >= '0' && c <= '9') ? c - '0' : (c >= 'A' && c <= 'F') ? c - 'A' + 10 : (c >= 'a' && c <= 'f') ? c - 'a' + 10: 0)

void hexdecode(const char *s, unsigned char *d) {
  while(*s && *(s+1)) {
    *d++ = 16*hex2int(s[0]) + hex2int(s[1]);
    s+=2;
  }
}

#define int2hex(n) ( (n) >= 10 ? 'a' + (n) - 10 : '0' + (n))

void hexencode(const unsigned char *s, char *d, int len) {
  while(len--) {
    *d++ = int2hex(*s >> 4);
    *d++ = int2hex(*s & 0xf);
    s++;
  }
}



