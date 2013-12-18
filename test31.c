#include<stdio.h>
#include<string.h>
#include<errno.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>


int http_get(const char *url, char *entity_body, int len) {
  char scheme[32];
  char hostname[64];
  char path[256];
  int port = 80;
  int s;

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

  s = socket(AF_INET, SOCK_STREAM, 0);

  struct hostent *hostent = gethostbyname(hostname);

  if(!hostent) {
    return -1;
  }

  //printf("addr_type: %d, AF_INET: %d\n", hostent->h_addrtype, AF_INET);
  //hexdump(hostent->h_addr_list[0], 16);
  //printf("lookup: %08X\n", *(unsigned int *)hostent->h_addr_list[0]);

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  memcpy(&addr.sin_addr, hostent->h_addr_list[0], sizeof(addr.sin_addr));


  //printf("connect to: %08X:%d\n", *(unsigned int *)&addr.sin_addr, port);
  
  if(connect(s, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    printf("connect failed: %s\n", strerror(errno));
    return -1;
  }

  FILE *fp = fdopen(s, "r+");

  fprintf(fp, "GET %s HTTP/1.1\r\nhost: %s\r\nconnection: close\r\n\r\n", path, hostname);
  fflush(fp);
  
  char status[4096];
  char buffer[4096];

  if(!fgets(status, sizeof(status), fp)) {
    return -1;
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
        return -1;
      }

      entity_body += chunk_size;

      len -= chunk_size;

      if(fscanf(fp, "\r\n") == EOF) {
        return -1;
      }

    } while(len && chunk_size);
    
  } else {
    entity_body += fread(entity_body, 1, len, fp);
  }

  *entity_body = '\0';

  fclose(fp);

  int status_code;

  if(sscanf(status, "HTTP/%*d.%*d %d", &status_code) == 1) {
    return status_code;
  } else {
    return -1;
  }
}


int main(int argc, char *argv[]) {
  char body[131072];
  int status;

  if(argc > 1) {
    if( (status=http_get(argv[1], body, sizeof(body))) != -1) {
      printf("status: %d\nentity body\n%s\n", status, body);
    }
  }

  return 0;
}
