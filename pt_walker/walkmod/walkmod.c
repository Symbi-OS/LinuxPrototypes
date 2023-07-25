#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <linux/printk.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Albert Slepak");
MODULE_DESCRIPTION("Page-walk Module");
MODULE_VERSION("1.0");

void print_pte(pte_t* pte) {
    pr_info("Page Table Entry (PTE) info:\n");

    // Checking the present bit
    if (pte_present(*pte))
        pr_info("PTE is present\n");
    else
        pr_info("PTE is not present\n");

    // Checking the read/write bit
    if (pte_write(*pte))
        pr_info("PTE is writable\n");
    else
        pr_info("PTE is read-only\n");

    // Checking the user/supervisor bit
    if (pte_val(*pte) & _PAGE_USER)
        pr_info("PTE is accessible by user-mode code\n");
    else
        pr_info("PTE is accessible by supervisor-mode code\n");

    // Checking the accessed bit
    if (pte_young(*pte))
        pr_info("PTE has been accessed\n");
    else
        pr_info("PTE has not been accessed\n");

    // Checking the dirty bit
    if (pte_dirty(*pte))
        pr_info("PTE is dirty (the page has been written to)\n");
    else
        pr_info("PTE is clean (the page has not been written to)\n");

    // Checking the PAT bit
    if (pte_special(*pte))
        pr_info("PTE has the PAT bit set\n");
    else
        pr_info("PTE has the PAT bit unset\n");
}

void pagetable_walk(void) {
    struct vm_area_struct* vma;
    struct mm_struct* task_mm;
    unsigned long vmpage;

    printk("Beginning kwalk, current_task: %p\n", current);

    task_mm = current->mm;
    printk("current_task->mm: %p\n", task_mm);

    if ((task_mm && task_mm->mmap))
    {
        pgd_t* pgd;
        p4d_t* p4d;
        pud_t* pud;
        pmd_t* pmd;
        pte_t* pte;

        vma = task_mm->mmap;
        printk("vma: %p\n", vma);

        printk("vma->vm_start  : 0x%lx\n", vma->vm_start);
        printk("vma->vm_end    : 0x%lx\n", vma->vm_end);
        printk("region size    : %li pages\n", (vma->vm_end - vma->vm_start) / PAGE_SIZE);

        for (vmpage = vma->vm_start; vmpage < vma->vm_end; vmpage += PAGE_SIZE)
        {
            pgd = pgd_offset(task_mm, vmpage);
            if (pgd_none(*pgd) || pgd_bad(*pgd))
                return;

            printk("pgd  : %p\n", pgd);

            p4d = p4d_offset(pgd, vmpage);
            if (p4d_none(*p4d) || p4d_bad(*p4d))
                return;

            printk("p4d  : %p\n", p4d);

            pud = pud_offset(p4d, vmpage);
            if (pud_none(*pud) || pud_bad(*pud))
                return;

            printk("pud  : %p\n", pud);

            pmd = pmd_offset(pud, vmpage);
            if (pmd_none(*pmd) || pmd_bad(*pmd))
                return;

            printk("pmd  : %p\n", pmd);
            pte = pte_offset_kernel(pmd, vmpage);
            if (!pte)
                return;

            printk("pte  : %p\n", pte);
            print_pte(pte);

            printk("\n");
        }
    }
}

int init_module(void) {
    printk("Kernel Module Loaded!\n");

    pagetable_walk();
	return 0;
}

void cleanup_module(void) {
    printk("Kernel Module Unloaded!\n");
}
