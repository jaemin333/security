#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd;
    size_t rop[10] = {0, };
    
    fd = open("/dev/test", O_RDWR);

    memset(rop, 0x41, 40);

    write(fd, rop, sizeof(rop));

    close(fd);

    return 0;
}
