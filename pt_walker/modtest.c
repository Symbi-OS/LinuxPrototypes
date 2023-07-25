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

extern int printf(const char* format, ...);

void pagetable_walk(struct mm_struct* mm, uint64_t* vm_start, uint64_t* vm_end) {
    // struct vm_area_struct* vma;
    // struct mm_struct* task_mm;
    // unsigned long vmpage;

    // printf("Beginning pagetable_walk, current_task: %p\n", task);

    // task_mm = task->mm;
    // printf("current_task->mm: %p\n", task_mm);

    printf("k> mmap: %p\n", mm);

    struct vm_area_struct* vma = mm->mmap;
    *vm_start = vma->vm_start;
    *vm_end = vma->vm_end;
}

int init_module(void) {
	return 0;
}

void cleanup_module(void) {
}
