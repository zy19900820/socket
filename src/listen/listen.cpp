#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv)
{
  if (argc <= 2)
  {
    printf("usage:%s ip port_num \n", basename(argv[0]));
    return 0;
  }

  const char *ip = argv[1];
  int port = atoi(argv[2]);

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;

  inet_pton(AF_INET, ip, &address.sin_addr);

  address.sin_port = htons(port);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock > 0);

  int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(sock, 5);
  assert(ret != -1);

  sleep(20);

  struct sockaddr_in client;
  socklen_t client_length = sizeof(client);

  int connfd = accept(sock, (struct sockaddr *)&client, &client_length);
  if (connfd < 0)
  {
    printf("errno is:%d \n", errno);
  }
  else
  {
    char remote[INET_ADDRSTRLEN];
    printf("conn with ip:%s port:%d \n",
        inet_ntop(AF_INET, &client.sin_addr, remote, INET_ADDRSTRLEN),
        ntohs(client.sin_port));
    close(connfd);
  }

  close(sock);

  return 0;
}
