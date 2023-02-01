// Include for pt_regs struct
#include <linux/ptrace.h>
#include <stdio.h>

struct pt_regs regs;

void print_pt_regs_struct() {
    printf("rax: %lx\n", regs.rax);
    printf("rbx: %lx\n", regs.rbx);
    printf("rcx: %lx\n", regs.rcx);
    printf("rdx: %lx\n", regs.rdx);
    printf("rsi: %lx\n", regs.rsi);
    printf("rdi: %lx\n", regs.rdi);
    printf("rbp: %lx\n", regs.rbp);
    printf("rsp: %lx\n", regs.rsp);
    printf("r8: %lx\n", regs.r8);
    printf("r9: %lx\n", regs.r9);
    printf("r10: %lx\n", regs.r10);
    printf("r11: %lx\n", regs.r11);
    printf("r12: %lx\n", regs.r12);
    printf("r13: %lx\n", regs.r13);
    printf("r14: %lx\n", regs.r14);
    printf("r15: %lx\n", regs.r15);
    printf("rip: %lx\n", regs.rip);
    printf("eflags: %lx\n", regs.eflags);
    printf("cs: %lx\n", regs.cs);
    printf("ss: %lx\n", regs.ss);
    printf("orig_rax: %lx\n", regs.orig_rax);
}

void print_args(){
    printf("arg0: %lx\n", regs.rdi);
    printf("arg1: %lx\n", regs.rsi);
    printf("arg2: %lx\n", regs.rdx);
    printf("arg3: %lx\n", regs.rcx);
    printf("arg4: %lx\n", regs.r8);
    printf("arg5: %lx\n", regs.r9);
}

void load_regs_6(unsigned long arg0, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
    regs.rdi = arg0;
    regs.rsi = arg1;
    regs.rdx = arg2;
    regs.rcx = arg3;
    regs.r8 = arg4;
    regs.r9 = arg5;
}

void load_pt_regs_from_regs(unsigned long arg0, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
    // Move contents of rdi register into regs.rdi
    asm volatile("movq %%rdi, %0" : "=m" (regs.rdi));
    asm volatile("movq %%rsi, %0" : "=m" (regs.rsi));
    asm volatile("movq %%rdx, %0" : "=m" (regs.rdx));
    asm volatile("movq %%rcx, %0" : "=m" (regs.rcx));
    asm volatile("movq %%r8, %0" : "=m" (regs.r8));
    asm volatile("movq %%r9, %0" : "=m" (regs.r9));
}

void load_regs_3(unsigned long arg0, unsigned long arg1, unsigned long arg2) {
    regs.rdi = arg0;
    regs.rsi = arg1;
    regs.rdx = arg2;
}

int main(int argc, char **argv) {

    print_args();
    // load_regs_3(&regs, 0x7, 0x42, 0x53);
    load_pt_regs_from_regs(0x7, 0x42, 0x53, 0x82, 0x107, 0x666);
    print_args();

    return 0;
}