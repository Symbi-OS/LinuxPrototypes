#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>
#include <linux/types.h>

#include "LINF/sym_all.h"

// Really should include this from linux headers.
// #error
// struct fd {
//     struct file *file;
//     unsigned int flags;
// };

// typedef unsigned long (*__fdget_pos_type)(unsigned int fd);

// static inline struct fd __to_fd(unsigned long v) {
//     return (struct fd){(struct file *)(v & ~3), v & 3};
// }

/* typedef int(*vfs_write_type)(unsigned int fd, const char *buf, size_t count);
 */

/* typedef ssize_t (*vfs_write_type)(struct file *file, const char *buf, size_t
 * count, loff_t *pos); */

/* static inline struct fd fdget_pos(int fd) */
/* { */
/*   // Get fn ptr to __fd_get_pos */
/*   __fdget_pos_type my___fdget_pos  = (__fdget_pos_type)
 * get_fn_address("__fdget_pos"); */
/*   /\* assert(my___fdget_pos != NULL); *\/ */
/* 	return __to_fd(my___fdget_pos(fd)); */
/* } */

// typedef ssize_t (*vfs_write_type)(struct file *file, const char *buf, size_t
// count, loff_t *pos);

#if 0
static inline loff_t *file_ppos(struct file *file)
{
	return file>f_mode & FMODE_STREAM ? NULL : &file->f_pos;
}

ssize_t local_ksys_write(unsigned int fd, const char *buf, size_t count)
{
  /* file_ppos_type my_file_ppos  = (file_ppos_type) get_fn_address("file_ppos"); */
  /* assert(my_file_ppos != NULL); */

  struct fd f = fdget_pos(fd);
  ssize_t ret = -9;

  if (f.file) {
    loff_t pos, *ppos = file_ppos(f.file);
    if (ppos) {
      pos = *ppos;
      ppos = &pos;
    }
    ret = vfs_write(f.file, buf, count, ppos);
    if (ret >= 0 && ppos)
      f.file->f_pos = pos;
    fdput_pos(f);
  }

  return ret;
}
#endif

// void vfs_write_shortcut(int reps, ksys_write_t my_vfs_write) {
//     // assert(reps > 0);
//     // assert(my_ksys_write != NULL);

//     while (reps--) {
//         //		if( (count % (1<<10)) == 0) {
//         /* write(1, "Opportunity to catch a signal\n", 30); */
//         //		} else {
//         my_vfs_write(1, "Tommy\n", 6);
//         /* ffprintf(stderr, "Tommy\n"); */
//         //		}
//     }
//     /* kallsymlib_cleanup(); */
// }

struct params {
    // raw program input
    char path[256];
    int test_id;
    unsigned iter;
    unsigned size;

    // Derived at runtime
    char *buf;
    int fd;
};

struct timers {
    clock_t start;
    clock_t end;
};
struct timers t;

void parse_args(int argc, char *argv[], struct params *p) {
    int expected_argc = 5;
    if (argc != expected_argc) {
        fprintf(stderr, "want <file_path> <test #> <iterations> <wr size> \n");
        fprintf(stderr, "0: glibc write(), 1 glibc syscall wrapper 2 raw syscall ... 5: ksys_write\n");
        // print wanted expected_argc, got argc
        fprintf(stderr, "got %d, want %d\n", argc, expected_argc);
        exit(1);
    }

    strcpy(p->path, argv[1]);
    p->test_id = atoi(argv[2]);
    p->iter = atoi(argv[3]);
    p->size = atoi(argv[4]);

    assert(strlen(p->path) > 0);
    assert(p->test_id >= 0);
    assert(p->iter > 0);
    assert(p->size > 0);
}

void glibc_write_loop(struct params *p) {
    // loop iter many times calling write()
    t.start = clock();
    for (unsigned i = 0; i < p->iter; i++) {
        int rc = write(p->fd, p->buf, p->size);
        // check for error
        if (rc < 0) {
            fprintf(stderr, "write() failed: %s\n", strerror(errno));
            exit(1);
        }
    }
    t.end = clock();
}
void glibc_syscall_wrapper_loop(struct params *p) {
    // loop iter many times calling write()
    t.start = clock();
    for (unsigned i = 0; i < p->iter; i++) {
        int rc = syscall(SYS_write, p->fd, p->buf, p->size);
        // check for error
        if (rc < 0) {
            fprintf(stderr, "write() failed: %s\n", strerror(errno));
            exit(1);
        }
    }
    t.end = clock();
}
void raw_syscall_loop(struct params *p) {
    // loop iter many times calling write()

    long syscall_number = SYS_write;
    int file_descriptor = p->fd;
    const void *buffer = p->buf;
    size_t buffer_size = p->size;

    t.start = clock();
    for (unsigned i = 0; i < p->iter; i++) {
        long result;
        __asm__ __volatile__("syscall"
                             : "=a"(result)
                             : "a"(syscall_number), "D"(file_descriptor),
                               "S"(buffer), "d"(buffer_size)
                             : "rcx", "r11", "memory");

        // Check for error
        if (result == -1) {
            fprintf(stderr, "write() failed: %s\n", strerror(errno));
            exit(1);
        }
    }

    t.end = clock();
}
/* https://www.cs.fsu.edu/~langley/CNT5605/2017-Summer/assembly-example/assembly.html
 * 64-bit SYSCALL instruction entry. Up to 6 arguments in registers.
 *
 * This is the only entry point used for 64-bit system calls.  The
 * hardware interface is reasonably well designed and the register to
 * argument mapping Linux uses fits well with the registers that are
 * available when SYSCALL is used.
 *
 * SYSCALL instructions can be found inlined in libc implementations as
 * well as some other programs and libraries.  There are also a handful
 * of SYSCALL instructions in the vDSO used, for example, as a
 * clock_gettimeofday fallback.
 *
 * 64-bit SYSCALL saves rip to rcx, clears rflags.RF, then saves rflags to r11,
 * then loads new ss, cs, and rip from previously programmed MSRs.
 * rflags gets masked by a value from another MSR (so CLD and CLAC
 * are not needed). SYSCALL does not save anything on the stack
 * and does not change rsp.
 *
 * Registers on entry:
 * rax  system call number
 * rcx  return address
 * r11  saved rflags (note: r11 is callee-clobbered register in C ABI)
 * rdi  arg0
 * rsi  arg1
 * rdx  arg2
 * r10  arg3 (needs to be moved to rcx to conform to C ABI)
 * r8   arg4
 * r9   arg5
 * (note: r12-r15, rbp, rbx are callee-preserved in C ABI)
 *
 * Only called from user space.
 *
 * When user can change pt_regs->foo always force IRET. That is because
 * it deals with uncanonical addresses better. SYSRET has trouble
 * with them due to bugs in both AMD and Intel CPUs.
 */
#if 0
// A function that only returns, do not setup stack or have any
// side effects. Use gcc attribute
void __attribute__((naked)) my_nop() {
    asm volatile("nop");
}
#endif
uint64_t get_lstar() {
    uint32_t eax_lower, edx_upper;
    sym_elevate();
    asm("rdmsr" : "=a"(eax_lower), "=d"(edx_upper) : "c"(0xc0000082));
    sym_lower();
    uint64_t lstar = ((uint64_t)edx_upper << 32) | eax_lower;
    return lstar;
}

uint64_t get_star() {
    uint32_t eax_lower, edx_upper;
    sym_elevate();
    asm("rdmsr" : "=a"(eax_lower), "=d"(edx_upper) : "c"(0xc0000081));
    sym_lower();
    uint64_t star = ((uint64_t)edx_upper << 32) | eax_lower;
    return star;
}

uint64_t get_fmask() {
    uint32_t eax_lower, edx_upper;
    sym_elevate();
    asm("rdmsr" : "=a"(eax_lower), "=d"(edx_upper) : "c"(0xc0000084));
    sym_lower();
    uint64_t fmask = ((uint64_t)edx_upper << 32) | eax_lower;
    return fmask;
}
void foo() {
    fprintf(stderr, "foo\n");
}
uint64_t get_fmask_comp(){
    // Get the syscall entry point from lstar
    // uint64_t sch_entry = get_lstar();
    
    // Get the old flags, needed for restore
    uint64_t old_rflags;
    sym_elevate(); asm("pushfq"); sym_lower();
    asm("popq %0" : "=r"(old_rflags));
    fprintf(stderr, "old_rflags: were %lx\n", old_rflags);

    // Get masked flags, how they should be for entering syscall handler
    uint64_t ia32_fmask = get_fmask();
    uint64_t ia32_fmask_comp = ~ia32_fmask;
    uint64_t syscall_rflags = old_rflags & ia32_fmask_comp;

    fprintf(stderr, "after complement rflags: %lx\n", syscall_rflags);
    return ia32_fmask_comp;
}
void skipping_syscall_instruction(struct params *p) {

    long syscall_number = SYS_write;
    int file_descriptor = p->fd;
    const void *buffer = p->buf;
    size_t buffer_size = p->size;

    uint64_t ia32_fmask_comp = get_fmask_comp();

    t.start = clock();

    sym_elevate();
    for (unsigned i = 0; i < p->iter; i++) {
        asm("pushfq");
        asm("popq %r11"); // User flags in r11

        asm("pushfq");
        asm("popq %rcx");
        asm("andq %0, %%rcx" : : ""(ia32_fmask_comp));
        asm("pushq %rcx");
        asm("popfq"); // rflags = rflags & not(fmask)

        // NOTE: Skip loading ss and cs, should be done from elevating

        // Unconvinced this works...
        asm("lea return_point(%rip), %rcx"); // Return address in rcx

        asm("mov %0, %%rax" : : "r"(syscall_number));
        asm("mov %0, %%edi" : : "r"(file_descriptor));
        asm("mov %0, %%rsi" : : "r"(buffer));
        asm("mov %0, %%rdx" : : "r"(buffer_size));
        // All args up to the 3rd are in standard location...
        
        // NOTE: if more args, will have to do more work.
     
        asm("mov $0xffffffff81e00000, %r10"); 
        asm("jmp *%r10"); // Update rip to syscall entry point
        // asm("jmp *0xffffffff81c00000"); // Update rip to syscall entry point

        __asm__ volatile("return_point:");
        asm("nop");

    }
    // print survived
    sym_lower();
    t.end = clock();
    fprintf(stderr, "Survived\n");
}

int g_fd;
char g_buf[1];
int g_size = 1;

typedef int (*ksys_write_t)(unsigned int fd, const char *buf, size_t count);
void ksys_write_shortcut(struct params *p) {
    // ksys_write_t my_ksys_write = NULL;
    // sym_touch_stack(); 
    // my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
    // // check it isn't null
    // if (my_ksys_write == NULL) {
    //     fprintf(stderr, "Failed to get ksys_write address\n");
    //     exit(1);
    // }

    ksys_write_t my_ksys_write = (ksys_write_t) 0xffffffff81367220;


    // fprintf(stderr, "using non stack version of write args\n");
    // g_fd = p->fd;
    // g_buf[0] = 'a';

    // assert(reps > 0);
    /* assert(my_ksys_write != NULL); */
    // int count = 0;
    for (unsigned i = 0; i < p->iter; i++) {

        if ((i % (10)) == 0) {
            int rc = write(p->fd, p->buf, p->size);
            if (rc < 0) {
                fprintf(stderr, "write() failed: %s\n", strerror(errno));
                exit(1);
            }
        } else {
            sym_elevate();
            int rc = my_ksys_write(p->fd, p->buf, p->size);
            sym_lower();
            // int rc = my_ksys_write(g_fd, g_buf, g_size);
            if (rc < 0) {
                fprintf(stderr, "write() failed: %s\n", strerror(errno));
                exit(1);
            }
        }
    }
}
void run_selected_test(struct params *p) {
    switch (p->test_id) {
    case 0:
        fprintf(stderr, "Loop using glibc write\n");
        glibc_write_loop(p);
        break;
    case 1:
        fprintf(stderr, "Loop using glibc syscall() wrapper\n");
        glibc_syscall_wrapper_loop(p);
        break;
    case 2:
        fprintf(stderr, "Loop using raw syscall\n");
        raw_syscall_loop(p);
        break;
    case 3:
        fprintf(stderr, "Loop using entry_syscall_64 without syscall ins\n");
        skipping_syscall_instruction(p);
        break;
    case 4:
        fprintf(stderr, "Loop using x64_sys_write\n");
        fprintf(stderr, "NYI, exiting\n");
        exit(1);
        break;
    case 5:
        fprintf(stderr, "Loop using ksys_write\n");
        ksys_write_shortcut(p);
        break;
    case 6:
        fprintf(stderr, "Loop using vfs_write\n");
        fprintf(stderr, "vfs shortcut unsupported\n");
        fprintf(stderr, "NYI, exiting\n");
        exit(1);
        break;
    default:
        fprintf(stderr, "idk what you want me to do with that input %d\n",
                p->test_id);
        exit(1);
    }
}
int init_file(struct params *p) {
    // Skip deletion if it's /dev/null
    if (strcmp(p->path, "/dev/null") == 0) {
        fprintf(stderr, "Skipping deletion of /dev/null\n");
    } else {
        if (access(p->path, F_OK) != -1) {
            fprintf(stderr, "File %s exists, deleting\n", p->path);
            int rc = remove(p->path);
            // check that remove worked
            if (rc != 0) {
                fprintf(stderr, "remove failed\n");
                exit(1);
            }
        }
    }

    int fd = open(p->path, O_WRONLY | O_CREAT);
    if (fd == -1) {
        fprintf(stderr, "open failed\n");
        exit(1);
    }
    return fd;
}

void init_buffer(struct params *p) {
    p->buf = malloc(p->size);
    if (p->buf == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    memset(p->buf, 'a', p->size);
}

void init(struct params *p) {
    p->fd = init_file(p);
    init_buffer(p);
}

void check_output(struct params *p) {
    // check size of file p->fd is p->iter * p->size
    int file_sz = lseek(p->fd, 0, SEEK_END);
    if (file_sz == -1) {
        fprintf(stderr, "lseek failed\n");
        exit(1);
    }

    fprintf(stderr, "file size: %d\n", file_sz);
    fprintf(stderr, "expected size: %d\n", p->iter * p->size);

    assert(file_sz == (int)(p->iter * p->size));

    fprintf(stderr, "file size: ok\n");

    // print md5sum of file
    char cmd[256];
    sprintf(cmd, "md5sum %s", p->path);
    system(cmd);
}

void report_time() {
    double time_taken = ((double)(t.end - t.start)) / CLOCKS_PER_SEC;
    fprintf(stderr, "\nTime taken: %f\n", time_taken);
}
void cleanup(struct params *p) {
    free(p->buf);
    close(p->fd);

    int rc = remove(p->path);
    if (rc != 0) {
        fprintf(stderr, "remove failed\n");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    struct params params;
    struct params *p = &params;

    parse_args(argc, argv, p);
    init(p);
    init_buffer(p);
    run_selected_test(p);

    check_output(p);
    cleanup(p);

    report_time();

    return 0;
}
