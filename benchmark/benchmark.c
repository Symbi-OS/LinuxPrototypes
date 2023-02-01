// This is super untested code. It's just a quick hack to get a
// functionality check.
#include <errno.h>
#include <fcntl.h>
#include <linux/sched.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <syscall.h>
#include <time.h>
#include <unistd.h>

void getpid_bench(int iterations, bool direct) {

    int i;
    int pid;
    for (i = 0; i < iterations; i++) {
        if (direct) {
            pid = getpid();
        } else {
            pid = syscall(SYS_getpid);
        }
        if (pid < 0) {
            printf("getpid failed\n");
            exit(1);
        }
    }
}
void getppid_bench(int iterations, bool direct) {
    int i;
    int ppid;
    for (i = 0; i < iterations; i++) {
        if (direct) {
            ppid = getppid();
        } else {
            ppid = syscall(SYS_getppid);
        }
        if (ppid < 0) {
            printf("getppid failed\n");
            exit(1);
        }
    }
}
#if 0
void sendto_bench(int iterations, bool direct) {
  int i;
  int sockfd;
  struct sockaddr_in addr;
  char buf[100];
  for (i = 0; i < iterations; i++) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
      printf("socket failed\n");
      exit(1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (direct) {
      sendto(sockfd, buf, 100, 0, (struct sockaddr *)&addr, sizeof(addr));
    } else {
      syscall(SYS_sendto, sockfd, buf, 100, 0, (struct sockaddr *)&addr,
              sizeof(addr));
    }
    close(sockfd);
  }
}
void recvfrom_bench(int iterations, bool direct) {
  int i;
  int sockfd;
  struct sockaddr_in addr;
  char buf[100];
  for (i = 0; i < iterations; i++) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
      printf("socket failed\n");
      exit(1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      printf("bind failed\n");
      exit(1);
    }
    if (direct) {
      recvfrom(sockfd, buf, 100, 0, (struct sockaddr *)&addr, sizeof(addr));
    } else {
      syscall(SYS_recvfrom, sockfd, buf, 100, 0, (struct sockaddr *)&addr,
              sizeof(addr));
    }
    close(sockfd);
  }
}
#endif
#define RD_SZ 10
void read_bench(int iterations, bool direct) {

    int i;
    int fd;
    char buf[RD_SZ];
    int ret;
    fd = open("/dev/zero", O_RDONLY);
    if (fd < 0) {
        printf("open failed\n");
        exit(1);
    }
    for (i = 0; i < iterations; i++) {
        if (direct) {
            ret = read(fd, buf, RD_SZ);
        } else {
            ret = syscall(SYS_read, fd, buf, RD_SZ);
        }
        // check error
        if (ret < 0) {
            printf("read failed\n");
            exit(1);
        }
    }
    close(fd);
}
void write_bench(int iterations, bool direct) {
    int i;
    int fd;
    char buf[100];
    int ret;
    fd = open("/dev/null", O_WRONLY);
    if (fd < 0) {
        printf("open failed\n");
        exit(1);
    }
    for (i = 0; i < iterations; i++) {
        if (direct) {
            ret = write(fd, buf, 100);
        } else {
            ret = syscall(SYS_write, fd, buf, 100);
        }
        // check error
        if (ret < 0) {
            printf("write failed\n");
            exit(1);
        }
    }
    close(fd);
}
#if 0
void thread_bench(int iterations, bool direct) {
  int i;
  pthread_t thread;
  for (i = 0; i < iterations; i++) {
    if (direct) {
      pthread_create(&thread, NULL, NULL, NULL);
    } else {
      syscall(SYS_clone,
              CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD |
                  CLONE_SYSVSEM | CLONE_SETTLS | CLONE_PARENT_SETTID |
                  CLONE_CHILD_CLEARTID,
              NULL, NULL, NULL, NULL);
    }
  }
}

void fork_bench(int iterations, bool direct) {
  int i;
  for (i = 0; i < iterations; i++) {
    if (direct) {
      fork();
    } else {
      syscall(SYS_clone,
              CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD |
                  CLONE_SYSVSEM | CLONE_SETTLS | CLONE_PARENT_SETTID |
                  CLONE_CHILD_CLEARTID,
              NULL, NULL, NULL, NULL);
    }
  }
}
#endif
void mmap_bench(int iterations, bool direct) {
    int i;
    void *addr;
    for (i = 0; i < iterations; i++) {
        if (direct) {
            addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        } else {
            addr = (void *)syscall(SYS_mmap, NULL, 4096, PROT_READ | PROT_WRITE,
                                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        }
        if (addr == MAP_FAILED) {
            printf("mmap failed\n");
            exit(1);
        }
        munmap(addr, 4096);
    }
}
void unmap_bench(int iterations, bool direct) {
    int i;
    void *addr;
    int ret;
    for (i = 0; i < iterations; i++) {
        // print all arguments to mmap
        addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (addr == MAP_FAILED) {
            printf("mmap failed\n");
            exit(1);
        }
        if (direct) {
            ret = munmap(addr, 4096);
        } else {
            ret = syscall(SYS_munmap, addr, 4096);
        }
        if (ret < 0) {
            printf("munmap failed\n");
            // print error number
            printf("ret: %d\n", ret);
            printf("errno: %d\n", errno);
            printf("errno: %s\n", strerror(errno));
            exit(1);
        }
    }
}
void poll_bench(int iterations, bool direct) {
    int i;
    int fd;
    struct pollfd fds[1];
    int ret;
    fd = open("/dev/zero", O_RDONLY);
    if (fd < 0) {
        printf("open failed\n");
        exit(1);
    }
    for (i = 0; i < iterations; i++) {
        fds[0].fd = fd;
        fds[0].events = POLLIN;
        if (direct) {
            ret = poll(fds, 1, 0);
        } else {
            ret = syscall(SYS_poll, fds, 1, 0);
        }
        // check error
        if (ret < 0) {
            printf("poll failed\n");
            exit(1);
        }
    }
    close(fd);
}
void epoll_bench(int iterations, bool direct) {
    int i;
    int fd;
    int epfd;
    struct epoll_event ev;
    int ret;
    fd = open("/dev/zero", O_RDONLY);
    if (fd < 0) {
        printf("open failed\n");
        exit(1);
    }
    for (i = 0; i < iterations; i++) {
        epfd = epoll_create(1);
        if (epfd < 0) {
            printf("epoll_create failed\n");
            exit(1);
        }
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (direct) {
            ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
        } else {
            ret = syscall(SYS_epoll_ctl, epfd, EPOLL_CTL_ADD, fd, &ev);
        }
        if (ret < 0) {
            printf("epoll_ctl failed\n");
            // print errno and string
            printf("errno: %d\n", errno);
            printf("strerror: %s\n", strerror(errno));
            exit(1);
        }
        close(epfd);
    }
    close(fd);
}
void select_bench(int iterations, bool direct) {
    int i;
    int fd;
    fd_set fds;
    int ret;
    fd = open("/dev/zero", O_RDONLY);
    if (fd < 0) {
        printf("open failed\n");
        exit(1);
    }
    for (i = 0; i < iterations; i++) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        if (direct) {
            ret = select(fd + 1, &fds, NULL, NULL, NULL);
        } else {
            ret = syscall(SYS_select, fd + 1, &fds, NULL, NULL, NULL);
        }
        if (ret < 0) {
            printf("select failed\n");
            exit(1);
        }
    }
    close(fd);
}
void run_all(int iterations, bool direct) {
    getpid_bench(iterations, direct);
    getppid_bench(iterations, direct);
    read_bench(iterations, direct);
    write_bench(iterations, direct);
    mmap_bench(iterations, direct);
    unmap_bench(iterations, direct);
    poll_bench(iterations, direct);
    select_bench(iterations, direct);

    //   epoll_bench(iterations, direct);
    //   sendto_bench(iterations, direct);
    //   recvfrom_bench(iterations, direct);
    //   thread_bench(iterations, direct);
    //   fork_bench(iterations, direct);
}
// include for pt_regs struct
#include <asm/ptrace.h>

int main(int argc, char **argv) {
    // Read input, if any.
    if (argc < 3) {
        printf("Usage: %s <iterations> <direct>\n", argv[0]);
        return 1;
    }

    int iterations = atoi(argv[1]);
    bool direct = atoi(argv[2]);

    run_all(iterations, direct);

    return 0;
}
