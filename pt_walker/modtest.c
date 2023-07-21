#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <linux/printk.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Albert Slepak");
MODULE_DESCRIPTION("Test Module");
MODULE_VERSION("1.0");

typedef void(*pte_walk_callback_t)(void*);

uint64_t read_cr3(void) {
    uint64_t cr3_value;
    asm volatile("mov %%cr3, %0" : "=r" (cr3_value));
    return cr3_value;
}

struct pgd* read_pgd(void) {
    uint64_t cr3_value = read_cr3();
    struct pgd* pgd = (struct pgd*)(cr3_value & PAGE_MASK);
    return pgd;
}

extern int printf(const char* format, ...);

#define PHYSICAL_PAGE_MASK 0x000FFFFFFFFFF000UL

void* pte_walk(struct task_struct* task) {
    printf("task: %p\n", task);
    struct mm_struct* mm = task->mm;
    printf("mm_struct: %p\n", mm);
    return task;

    printf("ksys_write: %p\n", (void*)0);
    return 0;

    uint64_t testAddr = (uint64_t)task;
    // pgd_t* pgd;
    // p4d_t* p4d;
    // pud_t* pud;
    // pmd_t* pmd;
    // pte_t* pte;

    // pgd = pgd_offset(mm, addr);
    // p4d = p4d_offset(pgd, addr);
    // pud = pud_offset(p4d, addr);
    // pmd = pmd_offset(pud, addr);
    // pte = pte_offset_map(pmd, addr);

    // printf("mm   : %p\n", mm);
    // printf("pgd  : %p\n", pgd);

    unsigned long cr3_val;
    unsigned long *pml4, *pdpt, *pdt, *pt, *pte;

    // Read CR3 register value
    asm volatile("mov %%cr3, %0" : "=r"(cr3_val));
    printf("walk> cr3: %ld\n", cr3_val);

    // Level 5: PML4
    pml4 = (unsigned long *)(cr3_val & PHYSICAL_PAGE_MASK);
    printf("walk> pml4: %p\n", pml4);

    // Level 4: PDPT
    pdpt = (unsigned long *)((unsigned long)pml4 + ((testAddr >> 39) & 0x1FF) * sizeof(unsigned long));
    printf("walk> pdpt: %p\n", pdpt);

    // Level 3: PDT
    pdt = (unsigned long *)((unsigned long)pdpt + ((testAddr >> 30) & 0x1FF) * sizeof(unsigned long));
    printf("walk> pdt: %p\n", pdt);

    // Level 2: PT
    pt = (unsigned long *)((unsigned long)pdt + ((testAddr >> 21) & 0x1FF) * sizeof(unsigned long));
    printf("walk> pt: %p\n", pt);

    // Level 1: PTE
    pte = (unsigned long *)((unsigned long)pt + ((testAddr >> 12) & 0x1FF) * sizeof(unsigned long));
    printf("walk> pte: %p\n", pte);

    // Print the physical address obtained from the PTE
    //printk(KERN_INFO "Physical address: 0x%lx\n", (*pte & PHYSICAL_PAGE_MASK));

    return pte;
}

int init_module(void) {
	return 0;
}

void cleanup_module(void) {
}
