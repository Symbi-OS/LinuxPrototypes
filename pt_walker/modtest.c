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

extern int printf(const char* format, ...);

void kwalk(struct task_struct* current_task) {
    struct vm_area_struct *vma;
    printf("Beginning kwalk, current_task: %p\n", current_task);

    struct mm_struct* task_mm = current_task->mm;
    printf("current_task->mm: %p\n", task_mm);

    if ((task_mm && task_mm->mmap))
    {
        // int i;
        // pgd_t *pgd;
        // pud_t *pud;
        // pmd_t *pmd;
        // pte_t *ptep, pte;

        vma = task_mm->mmap;
        printf("vma: %p\n", vma);
    }
}

int init_module(void) {
	return 0;
}

void cleanup_module(void) {
}
