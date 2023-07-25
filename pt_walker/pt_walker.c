#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <x86_64Constants.h>
#include <x86_64vmm.h>
#include <LINF/sym_all.h>

#define PAGE_SIZE        4096
#define NUM_ENTRIES      512
#define PAGE_MASK        ((uint64_t)(~0xFFF))
#define PAGE_PRESENT     0x1

#define PHYSICAL_PAGE_MASK 0x000FFFFFFFFFF000UL

#define __va(x)			((uint64_t)((unsigned long) (x) + 0xfffffc0000000000UL))

uint64_t read_cr4(void) {
    uint64_t cr4_value;
    asm volatile("mov %%cr4, %0" : "=r" (cr4_value));
    return cr4_value;
}

int is_pcide_enabled() {
    uint64_t cr4 = read_cr4();
    return (cr4 >> 17) & 1;
}

extern uint64_t read_cr3(void);
extern void kwalk(void*);

typedef void *(*bpf_get_current_task_t)();

void* get_current_task() {
    bpf_get_current_task_t gct = NULL;

    if (gct == NULL) {
        gct = (bpf_get_current_task_t) sym_get_fn_address("bpf_get_current_task");
        if (gct == NULL) {
            fprintf(stderr, "failed to find __fdget() \n");
            exit(-1);
        }
    }

    sym_elevate();
    return gct();
}

int main() {
    void* ksys_write = sym_get_fn_address("ksys_write");
    printf("ksys_write is at %p\n", ksys_write);

    // uint64_t testAddr = (uint64_t)ksys_write;

    unsigned int level;
    struct pte* pte = sym_get_pte((uint64_t)ksys_write, &level);

    sym_print_pte(pte);

    sym_elevate();

    printf("Is PCIDE Enabled: %i\n", is_pcide_enabled());

    // uint64_t cr3 = read_cr3();
    // printf("Read cr3: 0x%lx\n", cr3);

    // uint64_t pml5 = __va(cr3 & PHYSICAL_PAGE_MASK);
    // printf("pml5 base: %p\n", pml5);

    // uint64_t val = *pml5;
    // printf("pml5 value: 0x%lx\n", val);

    // struct pgd* top_level_pgd = read_pgd();
    // printf("read_pgd : 0x%lx\n", (uint64_t)top_level_pgd);

    kwalk(get_current_task());

    // walk_page_table();

    sym_lower();

    return 0;
}
