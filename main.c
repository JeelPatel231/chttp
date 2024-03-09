#include <assert.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define PENDING_CONNECTIONS 10
#define PORT 8000

const char HELLO_WORLD[] = "Hello World\n";
const int HELLO_WORLD_LEN = sizeof(HELLO_WORLD);

int main(void) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  assert(sock >= 0 && "Socket Creation Failed");

  const struct sockaddr_in addr = {.sin_family = AF_INET,
                                   .sin_port = htons(PORT),
                                   .sin_addr.s_addr = htonl(INADDR_ANY)};

  int bind_err = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  assert(bind_err >= 0 && "Socket Binding failed");

  int listen_err = listen(sock, PENDING_CONNECTIONS);
  assert(listen_err >= 0 && "Failed to listen");

  printf("Listening on port %d!\n", PORT);

  while (true) {
    int client_fd = accept(sock, NULL, NULL);

    send(client_fd, HELLO_WORLD, HELLO_WORLD_LEN, 0);

    close(client_fd);
  }

  close(sock);

  return EXIT_SUCCESS;
}
