#include <stdint.h>
#include <stdio.h>
#include <x86_64Constants.h>
#include <x86_64vmm.h>
#include <LINF/sym_all.h>

#define PAGE_SIZE        4096
#define NUM_ENTRIES      512
#define PAGE_MASK        ((uint64_t)(~0xFFF))
#define PAGE_PRESENT     0x1

struct pgd;
struct pud;
struct pmd;
struct mm_struct;

struct pgd* base_pgd; // You would set this to the value in the CR3 register using __va()

extern uint64_t read_cr3(void);
extern struct pgd* read_pgd(void);
extern void* walk_pte();

uint64_t read_rsp(void) {
    uint64_t rsp;
    asm volatile ("mov %%rsp, %0" : "=r"(rsp));
    return rsp;
}

int main() {
    void* ksys_write = sym_get_fn_address("ksys_write");

    unsigned int level;
    struct pte* pte = sym_get_pte((uint64_t)ksys_write, &level);

    sym_print_pte(pte);

    sym_elevate();

    uint64_t rsp = read_rsp();
    printf("rsp   : 0x%lx\n", rsp);
    
    uint64_t cr3 = read_cr3();
    printf("cr3   : 0x%lx\n", cr3);

    struct pgd* top_level_pgd = read_pgd();
    printf("pgd   : 0x%lx\n", (uint64_t)top_level_pgd);
    
    uint64_t pml4 = CR3_READ_PML4_ADDR();
    printf("pml4  : 0x%lx\n", (uint64_t)pml4);

    uint64_t walk = walk_pte();
    printf("walk  : 0x%lx\n", (uint64_t)walk);
    
    sym_lower();

    return 0;
}
