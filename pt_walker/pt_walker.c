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

struct pgd;
struct pud;
struct pmd;
struct mm_struct;

struct pgd* base_pgd; // You would set this to the value in the CR3 register using __va()

//extern uint64_t read_cr3(void);
//extern struct pgd* read_pgd(void);
// extern void* pte_walk(void*);

// typedef void *(*bpf_get_current_task_t)();

// void* get_current_task() {
//     bpf_get_current_task_t gct = NULL;

//     if (gct == NULL) {
//         gct = (bpf_get_current_task_t) sym_get_fn_address("bpf_get_current_task");
//         if (gct == NULL) {
//             fprintf(stderr, "failed to find __fdget() \n");
//             exit(-1);
//         }
//     }

//     sym_elevate();
//     return gct();
// }

void walk_page_table(void)
{
    unsigned long cr3, pml4e, pdpte, pde, pte;
    unsigned long *pml4, *pdpt, *pd, *pt;

    (void)pdpte;(void)pde;(void)pte;(void)pdpt;(void)pd;(void)pt;

    // Read the value of CR3 register to obtain the physical address of the base page table
    asm volatile("mov %%cr3, %0" : "=r" (cr3));
    printf("cr3   : 0x%lx\n", cr3);

    pml4 = (unsigned long *)(cr3 & PHYSICAL_PAGE_MASK);

    printf("walk> pml4: %p\n", pml4);
    printf("dereferencing...\n");
    unsigned long i = *pml4;
    printf("it worked!\n");

    // Traverse each level of the page table
    for (pml4e = 0; pml4e < 512; pml4e++) {
        if (!(pml4[pml4e] & 1))
            continue; // Entry not present, skip

        printf("pml4e: %p\n", pml4 + pml4e);
        // pdpt = (unsigned long *)(pml4[pml4e] & PHYSICAL_PAGE_MASK);
        // pdpt = (unsigned long *)((unsigned long)pdpt + __START_KERNEL_MAP);

        // for (pdpte = 0; pdpte < 512; pdpte++) {
        //     if (!(pdpt[pdpte] & 1))
        //         continue; // Entry not present, skip

        //     pd = (unsigned long *)(pdpt[pdpte] & PHYSICAL_PAGE_MASK);
        //     pd = (unsigned long *)((unsigned long)pd + __START_KERNEL_MAP);

        //     for (pde = 0; pde < 512; pde++) {
        //         if (!(pd[pde] & 1))
        //             continue; // Entry not present, skip

        //         pt = (unsigned long *)(pd[pde] & PHYSICAL_PAGE_MASK);
        //         pt = (unsigned long *)((unsigned long)pt + __START_KERNEL_MAP);

        //         for (pte = 0; pte < 512; pte++) {
        //             if (!(pt[pte] & 1))
        //                 continue; // Entry not present, skip

        //             phys_addr = (pt[pte] & PHYSICAL_PAGE_MASK);
        //             printk(KERN_INFO "Physical address: %lx\n", phys_addr);
        //         }
        //     }
        // }
    }
}

int main() {
    void* ksys_write = sym_get_fn_address("ksys_write");
    printf("ksys_write is at %p\n", ksys_write);

    // uint64_t testAddr = (uint64_t)ksys_write;

    unsigned int level;
    struct pte* pte = sym_get_pte((uint64_t)ksys_write, &level);

    sym_print_pte(pte);

    sym_elevate();

    // struct pgd* top_level_pgd = read_pgd();
    // printf("read_pgd : 0x%lx\n", (uint64_t)top_level_pgd);

    // void* walk = pte_walk(get_current_task());
    // printf("page walk result : 0x%lx\n", (uint64_t)walk);

    walk_page_table();

    sym_lower();

    return 0;
}
