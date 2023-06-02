#include "http.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>

void *client_thread(void *vptr)
{
  int fd = *((int *)vptr);
  HTTPRequest *req = malloc(sizeof(HTTPRequest));
  httprequest_read(req, fd);

  const char *path = req->path;

  if (strcmp(path, "/") == 0 || strcmp(path, "/340.png") == 0 || strcmp(path, "/getaway.html") == 0 || strcmp(path, "/index.html") == 0)
  {
    send(fd, req->version, strlen(req->version), 0);
    send(fd, " 200 OK\r\n", strlen(" 200 OK\r\n"), 0);

    FILE *f;
    if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0)
    {
      f = fopen("./static/index.html", "r");
    }
    else if (strcmp(path, "/340.png") == 0)
    {
      f = fopen("./static/340.png", "r");
    }
    else if (strcmp(path, "/getaway.html") == 0)
    {
      f = fopen("./static/getaway.html", "r");
    }

    long size = 0;
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);

    char *buf = malloc(size);
    fread(buf, 1, size, f);
    buf[size] = '\0';

    char *add[size + 1];

    write(fd, "Content-Type:", strlen("Content-Type: "));

    if (strcmp(path, "/getaway.html") == 0 || strcmp(path, "/index.html") == 0)
    {
      send(fd, "text/html", strlen("text/html"), 0);
      send(fd, "\r\n", strlen("\r\n"), 0);
    }
    else if (strcmp(path, "/340.png") == 0)
    {
      send(fd, "image/png", strlen("image/png"), 0);
      send(fd, "\r\n", strlen("\r\n"), 0);
    }

    send(fd, "Content-Length: ", strlen("Content-Length: "), 0);
    send(fd, add, strlen(buf), 0);
    send(fd, "\r\n\r\n", strlen("\r\n\r\n"), 0);

    send(fd, buf, size, 0);
    free(buf);
    buf = NULL;
    fclose(f);
    f = NULL;
  }
  else
  {
    send(fd, " 404 Not Found", strlen(" 404 Not Found"), 0);
    send(fd, "\r\n\r\n", strlen("\r\n\r\n"), 0);
  }
  close(fd);
  return NULL;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Usage: %s <port>\n", argv[0]);
    return 1;
  }
  int port = atoi(argv[1]);
  printf("Binding to port %d. Visit http://localhost:%d/ to interact with your server!\n", port, port);

  // socket:
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // bind:
  struct sockaddr_in server_addr, client_address;
  memset(&server_addr, 0x00, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);
  bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr));

  // listen:
  listen(sockfd, 10);

  // accept:
  socklen_t client_addr_len;
  while (1)
  {
    int *fd = malloc(sizeof(int));
    client_addr_len = sizeof(struct sockaddr_in);
    *fd = accept(sockfd, (struct sockaddr *)&client_address, &client_addr_len);
    printf("Client connected (fd=%d)\n", *fd);

    pthread_t tid;
    pthread_create(&tid, NULL, client_thread, fd);
    pthread_detach(tid);
  }

  return 0;
}