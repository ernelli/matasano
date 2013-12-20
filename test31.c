#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<time.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>


int http_get(const char *url, char *entity_body, int len) {
  char scheme[32];
  char hostname[64];
  char path[256];
  int port = 80;
  int status_code = -1;
  FILE *fp = NULL;
  int s = -1;

  if(sscanf(url, "%[^:]://", scheme) != 1) {
    return -1;
  }

  //printf("scheme: %s\n", scheme);

  // skip scheme://
  url += strlen(scheme) + 3;
  
  // port given
  if(strchr(url, ':')) {
    if(sscanf(url, "%[^:]:%d%s", hostname, &port, path) != 3) {
      return -1;
    }
    //printf("hostname: %s\nport: %d\npath: %s\n", hostname, port, path);
  } else {
    if(sscanf(url, "%[^/]%s", hostname, path) != 2) {
      return -1;
    }
    //printf("hostname: %s\nport: %d\npath: %s\n", hostname, port, path);
  }

  struct hostent *hostent = gethostbyname(hostname);

  if(!hostent) {
    goto cleanup;
  }

  //printf("addr_type: %d, AF_INET: %d\n", hostent->h_addrtype, AF_INET);
  //hexdump(hostent->h_addr_list[0], 16);
  //printf("lookup: %08X\n", *(unsigned int *)hostent->h_addr_list[0]);

  struct sockaddr_in addr;

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  memcpy(&addr.sin_addr, hostent->h_addr_list[0], sizeof(addr.sin_addr));

  s = socket(AF_INET, SOCK_STREAM, 0);

  if(s == -1) {
    goto cleanup;
  }

  //printf("connect to: %08X:%d\n", *(unsigned int *)&addr.sin_addr, port);
  
  if(connect(s, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    printf("connect failed: %s\n", strerror(errno));
    goto cleanup;
  }

  fp = fdopen(s, "r+");

  fprintf(fp, "GET %s HTTP/1.1\r\nhost: %s\r\nconnection: close\r\n\r\n", path, hostname);
  fflush(fp);
  
  char status[4096];
  char buffer[4096];

  if(!fgets(status, sizeof(status), fp)) {
    goto cleanup;
  }

  //printf("%s", status);

  int chunked = 0;

  while(fgets(buffer, sizeof(buffer), fp)) {
    //printf("header: %s", buffer);

    if(strcmp(buffer, "\r\n") == 0) {
      break;
    }

    if(strcasecmp(buffer, "transfer-encoding: chunked\r\n") == 0) {
      chunked = 1;
    }
  }

  // read entity body

  char *d = entity_body;
  int chunk_size;

  if(chunked) {

    do {
      //fgets(buffer, sizeof(buffer), fp)

      if(fscanf(fp, "%x\r\n", &chunk_size) != 1) {
        break;
      }
      
      //printf("read chunk_size: %d\n", chunk_size);

      if(chunk_size >= len) {
        chunk_size = len - 1;
      }


      if(fread(entity_body, 1, chunk_size, fp) != chunk_size) {
        goto cleanup;
      }

      entity_body += chunk_size;

      len -= chunk_size;

      if(fscanf(fp, "\r\n") == EOF) {
        goto cleanup;
      }

    } while(len && chunk_size);
    
  } else {
    entity_body += fread(entity_body, 1, len, fp);
  }

  *entity_body = '\0';


  if(sscanf(status, "HTTP/%*d.%*d %d", &status_code) != 1) {
    status_code = -1;
  }

 cleanup:
  if(fp) {
    fclose(fp);
  } else if(s != -1) {
    close(s);
  }
  return status_code;
}


int main(int argc, char *argv[]) {
  char body[4096];
  int status;
  unsigned char hmac[20];
  unsigned int *hmacint = (unsigned int *)hmac;
  const char *host = "localhost";


  struct timespec start, end;

  memset(hmac, 0, 20);

  if(argc > 1) {
    char *file = argv[1];
    if(argc > 2) {
      host = argv[2];
    }

    int i = 0;


    while(i < 20) {

      double latency, min_latency = 0, max_latency = 0, avg_latency = 0;
      int byte_max = 0;
    
      do {
        char url[256];
        sprintf(url, "http://%s:9000/test?file=%s&signature=%08x%08x%08x%08x%08x", host, file, bswap(hmacint[0]), bswap(hmacint[1]), bswap(hmacint[2]), bswap(hmacint[3]), bswap(hmacint[4]));
        //printf("testing url: %s\n", url);
        
        clock_gettime(CLOCK_REALTIME, &start);
        status = http_get(url, body, sizeof(body));
        clock_gettime(CLOCK_REALTIME, &end);

        if(status == 200) {
          printf("Found valid HMAC for %s\n%08x%08x%08x%08x%08x\n", file, bswap(hmacint[0]), bswap(hmacint[1]), bswap(hmacint[2]), bswap(hmacint[3]), bswap(hmacint[4]));
          break;
        }
        
        latency = (1E9*end.tv_sec + end.tv_nsec) - (1E9*start.tv_sec + start.tv_nsec);
        avg_latency += latency;
        
        if(!hmac[i]) {
          max_latency = latency;
          min_latency = latency;
        }
        
        if(latency > max_latency) {
          max_latency = latency;
          byte_max = hmac[i];
        }
        
        if(latency < min_latency) {
          min_latency = latency;
        }
        
        //printf("latency: %.6f\n", latency / 1E9);
        
        hmac[i]++;
      } while(hmac[i]);
      
      avg_latency /= 256;
      
      hmac[i] = byte_max;
      printf("max_latency: %.6f, avg: %.6f, byteval: %02x\n", max_latency / 1E9, avg_latency / 1E9, byte_max);
      
      i++;
    }
  }

  return 0;
}
