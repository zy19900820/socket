#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <fcntl.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE      1024

struct fds
{
  int epollfd;
  int sockfd;
};

int setnonblocking(int fd)
{
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}

// oneshot 决定是否注册EPOLLONESHOT
void addfd(int epollfd, int fd, bool oneshot)
{
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;
  if (oneshot)
  {
    event.events |= EPOLLONESHOT;
  }
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  setnonblocking(fd);
}

// 重置fd事件 操作系统仍然触发EPOLLIN事件 且触发一次
void reset_oneshot(int epollfd, int fd)
{
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
  epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void *worker(void *arg)
{
  int sockfd = ((fds*)arg)->sockfd;
  int epollfd = ((fds*)arg)->epollfd;

  printf("start new thread to receive data on %d \n", sockfd);
  
  char buf[BUFFER_SIZE];
  memset(buf, '\0', BUFFER_SIZE);

  while (1)
  {
    int ret = recv(sockfd, buf, BUFFER_SIZE - 1, 0);
    if (ret == 0)
    {
      close(sockfd);
      printf("close conn\n");
      break;
    }
    else if (ret < 0)
    {
      if (errno == EAGAIN)
      {
        reset_oneshot(epollfd, sockfd);
        printf("read later\n");
        break;
      }
    }
    else
    {
      printf("get content :%s \n", buf);
      sleep(5);
    }
  }
  printf("end thread %d \n", sockfd);
}


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

  int listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd > 0);

  int ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(listenfd, 5);
  assert(ret != -1);

  epoll_event events[MAX_EVENT_NUMBER];
  int epollfd = epoll_create(5);
  assert(epollfd != -1);

  addfd(epollfd, listenfd, false);

  while (1)
  {
    int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
    if (ret < 0)
    {
      printf("epoll fail");
      break;
    }

    for (int i = 0; i < ret; i++)
    {
      int sockfd = events[i].data.fd;
      if (sockfd == listenfd)
      {
        struct sockaddr_in client_address;
        socklen_t client_length = sizeof(client_address);
        int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_length);
        addfd(epollfd, connfd, true);
      }
      else if (events[i].events & EPOLLIN)
      {
        pthread_t thread;
        fds new_work;
        new_work.epollfd = epollfd;
        new_work.sockfd = sockfd;
        pthread_create(&thread, NULL, worker, (void *)&new_work);
      }
      else
      {
        printf("some happened\n");
      }
    }
  }

  close(listenfd);
  return 0;
}
