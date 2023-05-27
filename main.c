#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PORT 80

int main() {
  int new_sock;
  int sockfd;
  struct sockaddr_in addr;
  memset((char *) &addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(PORT);
  int addr_len = sizeof(addr);
  char* buffer = malloc(65535*sizeof(char));

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket generation failed");
    return 0;
  }
  if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    perror("Port bind failed");
    return 0;
  }
  if (listen(sockfd, 32) < 0) {
    perror("Listen failed");
    exit(-1);
  }
    
  while(1) {
    int status = 200;
    char* status_str = "OK";
    char* content_type = "text/html";
    char* message = "<p>This should not be shown.</p>";
    char* filepath = "/index.html";
    long readVal;
    char resp[65535];
    char url[2048] = ".";

    if ((new_sock = accept(sockfd, (struct sockaddr * ) &addr, (socklen_t *) &addr_len)) < 0) {
      perror("Accept failed");
      exit(-1);
    }

    readVal = read(new_sock, buffer, 65535);
    if (readVal < 0) {
      printf("Nothing to Read.\n");
    }
    
    printf("%s\n", buffer);
    char * token = strtok(buffer, " ");
    token = strtok(NULL, " ");

    printf("%s\n", token);
    printf("%d\n", strcmp(token, "/"));

    if(strcmp(token, "/")) {
      char *token_tmp = calloc(strlen(token), sizeof(char));
      strcpy(token_tmp, token);
      char * file_end = strtok(token_tmp, ".");
      file_end = strtok(NULL, ".");

      printf("%s\n", file_end);

      if(!strcmp(file_end, "js")) {
        content_type = "application/javascript";
      }
      else if (!strcmp(file_end, "css")) {
        content_type = "text/css";
      }
      else if (!strcmp(file_end, "png")) {
        content_type = "image/png";
      }
      else if (!strcmp(file_end, "jpg")) {
        content_type = "image/jpeg";
      }
      else {
        content_type = "text/plain";
      }
      free(token_tmp);
    }

    printf("%s\n", content_type);

    strcat(url, strcmp(token, "/") ? token : filepath);
    FILE *fptr = fopen(url, "r");

    printf("%s\n", url);

    if(fptr == NULL) {
      printf("Here");
      status = 404;
      status_str = "Not Found";
      char* m = "<p>That file doesn't exist dawg.</p>";
      sprintf(resp, "HTTP/1.1 %d %s\nContent-Type: %s\nContent-Length: %d\n\n%s", status, status_str, content_type, (int) strlen(m), m);
      write(new_sock, resp, strlen(resp));
    }
    else {
      printf("here\n");
      fseek(fptr, 0L, SEEK_END);
      long n_bytes = ftell(fptr);
      fseek(fptr, 0L, SEEK_SET);
      printf("here2\n");
      char* f_buffer = (char*) calloc(n_bytes, sizeof(char));
      fread(f_buffer, sizeof(char), n_bytes, fptr);
      printf("here3\n");
      fclose(fptr);
      sprintf(resp, "HTTP/1.1 %d %s\nContent-Type: %s\nContent-Length: %d\n\n%s", status, status_str, content_type, (int) strlen(f_buffer), f_buffer);
      write(new_sock, resp, strlen(resp));
      printf("%s\n", resp);
    }
  }

  close(new_sock);
  free(buffer);
}