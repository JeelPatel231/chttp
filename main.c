#include <assert.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PENDING_CONNECTIONS 10
#define PORT 8000

typedef struct {
  int length;
  char *content;
} Response;

const char http_response_line[] = "HTTP/1.1 %d\r\n";
const char content_length_str[] = "Content-Length: %d\r\n";
const char body_str[] = "%s";

void free_response(Response *res) {
  printf("DEBUG: freed response @ %p\n", res);
  free(res->content);
  free(res);
}

Response *response_builder(int status, char *body) {
  if (body == NULL) {
    body = "";
  }

  int response_line_length = snprintf(NULL, 0, http_response_line, status);
  int body_length = snprintf(NULL, 0, body_str, body);
  int content_length_len = snprintf(NULL, 0, content_length_str, body_length);

  int header_lines_length = content_length_len + 2;

  int total_response_length =
      response_line_length + header_lines_length + body_length + 1;

  printf("Bytes to be allocated : %d\n", total_response_length);

  // Can be reduced to 1 malloc and then typecast
  // but that isnt portable, so i wont do it
  Response *res = malloc(sizeof(Response));
  res->content = malloc(total_response_length * sizeof(char));
  char *res_appender = res->content;
  res_appender += sprintf(res_appender, http_response_line, status);
  res_appender += sprintf(res_appender, content_length_str, body_length);
  res_appender += sprintf(res_appender, "\r\n"); // header-body differentiator
  res_appender += sprintf(res_appender, body_str, body);

  res->length = res_appender - res->content;
  return res;
}

int main(void) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  assert(sock >= 0 && "Socket Creation Failed");

  const struct sockaddr_in addr = {.sin_family = AF_INET,
                                   .sin_port = htons(PORT),
                                   .sin_addr.s_addr = htonl(INADDR_ANY)};

  int bind_err = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  assert(bind_err >= 0 && "Socket Binding failed");

  int sockopt_addr_err =
      setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
  assert(sockopt_addr_err >= 0 && "Failed to set socket option SO_REUSEADDR");

  int sockopt_port_err =
      setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
  assert(sockopt_port_err >= 0 && "Failed to set socket option SO_REUSEPORT");

  int listen_err = listen(sock, PENDING_CONNECTIONS);
  assert(listen_err >= 0 && "Failed to listen");

  printf("Listening on port %d!\n", PORT);

  while (true) {
    int client_fd = accept(sock, NULL, NULL);

    Response *res = response_builder(200, "Hello World");
    send(client_fd, res->content, res->length, 0);
    free_response(res);

    // close the connection instantly, even if it is http/1.1
    close(client_fd);
  }

  close(sock);

  return EXIT_SUCCESS;
}
