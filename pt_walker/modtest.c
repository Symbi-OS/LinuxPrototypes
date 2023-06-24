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

void* walk_pte(void) {
    struct task_struct* task = current;
    struct mm_struct* mm = task->mm;

    unsigned long addr;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    return task;
}

int init_module(void) {
	return 0;
}

void cleanup_module(void) {
}
