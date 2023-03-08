#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/types.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

#include "LINF/sym_all.h"

// int g_fd;
// char *g_buf;
// size_t g_size = 1;
uint64_t rsp_squirrel;

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
    size_t size;

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
        fprintf(stderr, "0: glibc write(), 1 glibc syscall wrapper 2 raw "
                        "syscall ... 5: ksys_write\n");
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
        asm volatile("syscall"
                     : "=a"(result)
                     : "a"(syscall_number), "D"(file_descriptor), "S"(buffer),
                       "d"(buffer_size)
                     : "rcx", "r11", "memory");

        // Check for error
        if (result == -1) {
            fprintf(stderr, "write() failed: %s\n", strerror(errno));
            exit(1);
        }
    }

    t.end = clock();
}

uint64_t get_lstar() {
    uint32_t eax_lower, edx_upper;
    sym_elevate();
    asm volatile("rdmsr" : "=a"(eax_lower), "=d"(edx_upper) : "c"(0xc0000082));
    sym_lower();
    uint64_t lstar = ((uint64_t)edx_upper << 32) | eax_lower;
    return lstar;
}

uint64_t get_star() {
    uint32_t eax_lower, edx_upper;
    sym_elevate();
    asm volatile("rdmsr" : "=a"(eax_lower), "=d"(edx_upper) : "c"(0xc0000081));
    sym_lower();
    uint64_t star = ((uint64_t)edx_upper << 32) | eax_lower;
    return star;
}

uint64_t get_fmask() {
    uint32_t eax_lower, edx_upper;
    sym_elevate();
    asm volatile("rdmsr" : "=a"(eax_lower), "=d"(edx_upper) : "c"(0xc0000084));
    sym_lower();
    uint64_t fmask = ((uint64_t)edx_upper << 32) | eax_lower;
    return fmask;
}
uint64_t get_fmask_comp() {
    // Get the syscall entry point from lstar
    // uint64_t sch_entry = get_lstar();

    // Get the old flags, needed for restore
    uint64_t old_rflags;
    sym_elevate();
    asm volatile("pushfq");
    sym_lower();
    asm volatile("popq %0" : "=m"(old_rflags) : : "memory");

    // Get masked flags, how they should be for entering syscall handler
    uint64_t ia32_fmask = get_fmask();
    uint64_t ia32_fmask_comp = ~ia32_fmask;

    return ia32_fmask_comp;
}

// switch stack
inline void __attribute__((always_inline)) stack_switch_to_kern() {
    asm volatile("mov %%rsp, %0" : "=m"(rsp_squirrel)); // cookie
    asm volatile("mov  %gs:0x17b90,%rsp");
}

inline void __attribute__((always_inline)) stack_switch_to_user() {
    asm volatile("mov %0, %%rsp" : : "m"(rsp_squirrel));
}

// // Inline fn push 64 bit val to stack
// inline void __attribute__((always_inline)) push_64(uint64_t val) {
//     asm volatile("push %rax"); // preserve

//     asm volatile("mov %0, %%rax" : : "m"(val) : "rax");
//     asm volatile("push %rax");

//     asm volatile("pop %rax"); // restore
// }

// This doesn't work for rax ...
// #define push_64(val)
//     asm volatile("push %rax");
//     asm volatile("mov %0, %%rax" : : "m"(val) : "rax");
//     asm volatile("push %rax");
//     asm volatile("pop %rax");

// In this memory model, it appears to be impossible to embed a 64 bit value
// in text, this is a workaround.
#define push_64(val)                                                           \
    asm volatile("push %%rax" : : : "memory");                                 \
    asm volatile("mov %0, %%rax" : : "g"(val) : "rax");                        \
    asm volatile("xor (%%rsp), %%rax" : : : "rax");                            \
    asm volatile("xor %%rax, (%%rsp)" : : : "memory");                         \
    asm volatile("xor (%%rsp), %%rax" : : : "rax");

void emulate_syscall_ins(struct params *p) {
    // Syscall handler rip
    uint64_t lstar = get_lstar();

    // What flags should be masked for the syscall handler
    uint64_t ia32_fmask_comp = get_fmask_comp();

    uint64_t user_flags;

    t.start = clock();
    sym_elevate();
    // load syscall number
    for (unsigned i = 0; i < p->iter; i++) {

        push_64(lstar);

        // load user rflags, has to come before they get modified.
        asm volatile("pushf" : : : "memory");
        // pop into user_flags
        asm volatile("pop %0" : "=m"(user_flags) : : "memory");

        push_64(user_flags);

        uint64_t masked_flags = user_flags & ia32_fmask_comp;

        // Put syscall_flags into rflags
        asm volatile("push %%rax" : : : "memory"); // preserve
        asm volatile("mov %0, %%rax" : : "m"(masked_flags) : "rax");
        asm volatile("push %%rax" : : : "memory");
        asm volatile("popf" : : : "cc", "memory");
        asm volatile("pop %%rax" : : : "rax", "memory"); // restore

        // No regs are hot at this point.

        // Syscall number
        push_64(SYS_write);

        // args
        push_64((uint64_t)p->fd);
        push_64((uint64_t)p->buf);
        push_64(p->size);

        // After this, regs go hot, be careful.

        // Args
        asm volatile("pop %%rdx" : : : "rdx", "memory");
        asm volatile("pop %%rsi" : : : "rsi", "memory");
        asm volatile("pop %%rdi" : : : "rdi", "memory");

        // Syscall number
        asm volatile("pop %%rax" : : : "rax", "memory");

        // Flags
        asm volatile("pop %%r11" : : : "r11", "memory");

        // Return RIP
        asm volatile("lea return_point(%%rip), %%rcx" : : : "rcx");

        // Send us to the syscall handler
        asm volatile("ret" : : : "memory");

        asm volatile("return_point:");
    }
    sym_lower();
    t.end = clock();
}
// This function is like the above, but it also switches stacks and
// enters after the syscall handler switches.

void emulate_syscall_ins_and_stack_switch(struct params *p) {

    uint64_t ia32_fmask_comp = get_fmask_comp();
    uint64_t user_flags;
    void *syscall_handler =
        (void *)sym_get_fn_address("entry_SYSCALL_64_safe_stack");
    // void *syscall_handler = (void *)sym_get_fn_address("entry_SYSCALL_64");
    // check it isn't null
    if (syscall_handler == NULL) {
        fprintf(stderr, "Failed to get entry_SYSCALL_64_safe_stack\n");
        exit(1);
    }

    t.start = clock();
    sym_elevate();
    // load syscall number
    for (unsigned i = 0; i < p->iter; i++) {
        // stack_switch_to_kern();
        asm volatile("mov %rsp, %gs:0x6014");
        asm volatile("mov  %gs:0x17b90,%rsp");

        push_64(syscall_handler);

        // load user rflags, has to come before they get modified.
        asm volatile("pushf" : : : "memory");
        // pop into user_flags
        asm volatile("pop %0" : "=m"(user_flags) : : "memory");

        push_64(user_flags);

        uint64_t masked_flags = user_flags & ia32_fmask_comp;

        // Put syscall_flags into rflags
        asm volatile("push %%rax" : : : "memory"); // preserve
        asm volatile("mov %0, %%rax" : : "m"(masked_flags) : "rax");
        asm volatile("push %%rax" : : : "memory");
        asm volatile("popf" : : : "cc", "memory");
        asm volatile("pop %%rax" : : : "rax", "memory"); // restore

        // No regs are hot at this point.

        // Syscall number
        push_64(SYS_write);

        // args
        push_64((uint64_t)p->fd);
        push_64((uint64_t)p->buf);
        push_64(p->size);

        // After this, regs go hot, be careful.

        // Args
        asm volatile("pop %%rdx" : : : "rdx", "memory");
        asm volatile("pop %%rsi" : : : "rsi", "memory");
        asm volatile("pop %%rdi" : : : "rdi", "memory");

        // Syscall number
        asm volatile("pop %%rax" : : : "rax", "memory");

        // Flags
        asm volatile("pop %%r11" : : : "r11", "memory");

        // Return RIP
        asm volatile("lea return_point_ss(%%rip), %%rcx" : : : "rcx");

        // Send us to the syscall handler
        asm volatile("ret" : : : "memory");

        asm volatile("return_point_ss:");
    }
    sym_lower();
    t.end = clock();
}

typedef int (*ksys_write_t)(unsigned int fd, const char *buf, size_t count);

void ksys_write_shortcut(struct params *p, int do_stack_switch) {
    ksys_write_t my_ksys_write = NULL;
    sym_touch_stack();
    my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
    // check it isn't null
    if (my_ksys_write == NULL) {
        fprintf(stderr, "Failed to get ksys_write address\n");
        exit(1);
    }

    t.start = clock();
    sym_elevate();
    for (unsigned i = 0; i < p->iter; i++) {

        if ((i % (10)) == 0) {
            int rc = write(p->fd, p->buf, p->size);
            if (rc < 0) {
                fprintf(stderr, "write() failed: %s\n", strerror(errno));
                exit(1);
            }
        } else {
            if (do_stack_switch) {
                asm volatile("cli");
                stack_switch_to_kern();
                asm volatile("sti");
            }

            int rc = my_ksys_write(p->fd, p->buf, p->size);

            if (do_stack_switch) {
                asm volatile("cli");
                stack_switch_to_user();
                asm volatile("sti");
            }

            if (rc < 0) {
                fprintf(stderr, "write() failed: %s\n", strerror(errno));
                exit(1);
            }
        }
    }
    sym_lower();
    t.end = clock();
}
#if 0
void vfs_write_shortcut(struct params *p) {
    ksys_write_t my_vfs_write = NULL;
        my_vfs_write = (ksys_write_t)sym_get_fn_address("vfs_write");
            if (my_vfs_write == NULL) {
        fprintf(stderr, "Failed to get ksys_write address\n");
        exit(1);
    }
    t.start = clock();
    sym_elevate();
    for (unsigned i = 0; i < p->iter; i++) {

        if ((i % (10)) == 0) {
            int rc = write(p->fd, p->buf, p->size);
            if (rc < 0) {
                fprintf(stderr, "write() failed: %s\n", strerror(errno));
                exit(1);
            }
        } else {

            SYM_ON_KERN_STACK_DO( \
                #error
                int rc = my_vfs_write(p->fd, p->buf, p->size); \
            );

            if (rc < 0) {
                fprintf(stderr, "write() failed: %s\n", strerror(errno));
                exit(1);
            }
        }
    }
    sym_lower();
    t.end = clock();
}
#endif

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
        emulate_syscall_ins(p);
        break;
    case 4:
        fprintf(
            stderr,
            "Loop using entry_syscall_64 without syscall ins stack switch\n");
        emulate_syscall_ins_and_stack_switch(p);
        break;
    case 5:
        fprintf(stderr, "Loop using x64_sys_write\n");
        fprintf(stderr, "NYI, exiting\n");
        exit(1);
        break;
    case 6:
        fprintf(stderr, "Loop using ksys_write\n");
        int do_stack_switch = 0;
        ksys_write_shortcut(p, do_stack_switch);
        break;
    case 7:
        fprintf(stderr, "Loop using ksys_write with stack switch\n");
        do_stack_switch = 1;
        ksys_write_shortcut(p, do_stack_switch);
        break;
    case 8:
        fprintf(stderr, "Loop using vfs_write\n");
        // vfs_write_shortcut(p);
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

    // fprintf(stderr, "file size: %d\n", file_sz);
    // fprintf(stderr, "expected size: %ld\n", p->iter * p->size);

    assert(file_sz == (int)(p->iter * p->size));

    // fprintf(stderr, "file size: ok\n");

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

    report_time();

    check_output(p);
    cleanup(p);


    return 0;
}
