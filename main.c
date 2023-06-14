#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define PORT 80

char buffer[65535];
char resp[65535];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * socket_thread(void* arg) {
  int socket = *((int*) arg);
  recv(socket, buffer, 65535, 0);

  pthread_mutex_lock(&lock);
  int status = 200;
  char* status_str = "OK";
  char* content_type = "text/html";
  char* message = "<p>This should not be shown.</p>";
  char* filepath = "/index.html";
  char url[2048] = ".";
  
  char * token = strtok(buffer, " ");
  token = strtok(NULL, " ");

  if(strcmp(token, "/")) {
    char *token_tmp = calloc(strlen(token), sizeof(char));
    strcpy(token_tmp, token);
    char * file_end = strtok(token_tmp, ".");
    file_end = strtok(NULL, ".");

    if(!strcmp(file_end, "js")) {
      content_type = "application/javascript";
    } else if (!strcmp(file_end, "css")) {
      content_type = "text/css";
    } else if (!strcmp(file_end, "png")) {
      content_type = "image/png";
    } else if (!strcmp(file_end, "jpg")) {
      content_type = "image/jpeg";
    } else {
      content_type = "text/plain";
    }
    free(token_tmp);
  }

  strcat(url, strcmp(token, "/") ? token : filepath);
  FILE *fptr = fopen(url, "r");

  if(fptr == NULL) {
    status = 404;
    status_str = "Not Found";
    fptr = fopen("./404.html", "r");
    if (fptr == NULL) {
      char* m = "<p>That file doesn't exist dawg.</p>";
      sprintf(resp, "HTTP/1.1 %d %s\nContent-Type: %s\nContent-Length: %d\n\n%s", status, status_str, content_type, (int) strlen(m), m);
    } else {
      fseek(fptr, 0L, SEEK_END);
      long n_bytes = ftell(fptr);
      fseek(fptr, 0L, SEEK_SET);
      char* f_buffer = (char*) calloc(n_bytes, sizeof(char));
      fread(f_buffer, sizeof(char), n_bytes, fptr);
      fclose(fptr);
      sprintf(resp, "HTTP/1.1 %d %s\nContent-Type: %s\nContent-Length: %d\n\n%s", status, status_str, content_type, (int) strlen(f_buffer), f_buffer);
      free(f_buffer);
    }
  } else {
    fseek(fptr, 0L, SEEK_END);
    long n_bytes = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);
    char* f_buffer = (char*) calloc(n_bytes, sizeof(char));
    fread(f_buffer, sizeof(char), n_bytes, fptr);
    fclose(fptr);
    sprintf(resp, "HTTP/1.1 %d %s\nContent-Type: %s\nContent-Length: %d\n\n%s", status, status_str, content_type, (int) strlen(f_buffer), f_buffer);
    free(f_buffer);
  }
  pthread_mutex_unlock(&lock);
  sleep(1);
  send(socket, resp, strlen(resp), 0);
  close(socket);
  pthread_exit(NULL);
}

int main() {
  int new_sock, sockfd;
  struct sockaddr_in addr;
  socklen_t addr_len;

  memset((char *) &addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(PORT);
  addr_len = sizeof(addr);

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

  pthread_t tid[60];
  int i = 0;
    
  while (1) {
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

    if(pthread_create(&tid[i++], NULL, socket_thread, &new_sock)) {
      printf("Failed to create thread\n");
    }

    if (i >= 50) {
      i = 0;
      while (i < 50) {
        pthread_join(tid[i++], NULL);
      }
      i = 0;
    }
  }
  return 0;
}