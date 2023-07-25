#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <linux/printk.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Albert Slepak");
MODULE_DESCRIPTION("Page-walk Module");
MODULE_VERSION("1.0");

#define KERNEL_PAGE_OFFSET 0xffff888000000000

struct page_index_dict {
    uint64_t pt_lvl_4;      // Page Directory Pointer table index
    uint64_t pt_lvl_3;      // Page Directory table index
    uint64_t pt_lvl_2;      // Page Table index
    uint64_t pt_lvl_1;      // Page index inside the page table
};

void vaddr_to_page_offsets(
    uint64_t addr,
    struct page_index_dict* dict
) {
    addr >>= 12;
    dict->pt_lvl_1 = addr & 0x1ff;
    addr >>= 9;
    dict->pt_lvl_2 = addr & 0x1ff;
    addr >>= 9;
    dict->pt_lvl_3 = addr & 0x1ff;
    addr >>= 9;
    dict->pt_lvl_4 = addr & 0x1ff;
}

struct page_table_entry {
    union
    {
        struct
        {
            uint64_t present                : 1;    // Must be 1, region invalid if 0.
            uint64_t read_write             : 1;    // If 0, writes not allowed.
            uint64_t user_supervisor        : 1;    // If 0, user-mode accesses not allowed.
            uint64_t page_write_through     : 1;    // Determines the memory type used to access the memory.
            uint64_t page_cache_disabled    : 1;    // Determines the memory type used to access the memory.
            uint64_t accessed               : 1;    // If 0, this entry has not been used for translation.
            uint64_t dirty                  : 1;    // If 0, the memory backing this page has not been written to.
            uint64_t page_access_type       : 1;    // Determines the memory type used to access the memory.
            uint64_t global                 : 1;    // If 1 and the PGE bit of CR4 is set, translations are global.
            uint64_t ignored2               : 3;
            uint64_t page_frame_number      : 36;   // The page frame number of the backing physical page.
            uint64_t reserved               : 4;
            uint64_t ignored3               : 7;
            uint64_t protection_key         : 4;    // If the PKE bit of CR4 is set, determines the protection key.
            uint64_t execute_disable        : 1;    // If 1, instruction fetches not allowed.
        };
        uint64_t value;
    };
} __attribute__((packed));

struct page_table {
    struct page_table_entry entries[512];
} __attribute__((aligned(PAGE_SIZE)));

void print_page_table_entry(struct page_table_entry* entry) {
    printk("------ page_table_entry info ------\n");
    printk("    present             : %i\n", (int)entry->present);
    printk("    read_write          : %i\n", (int)entry->read_write);
    printk("    user_supervisor     : %i\n", (int)entry->user_supervisor);
    printk("    page_write_through  : %i\n", (int)entry->page_write_through);
    printk("    page_cache_disabled : %i\n", (int)entry->page_cache_disabled);
    printk("    accessed            : %i\n", (int)entry->accessed);
    printk("    dirty               : %i\n", (int)entry->dirty);
    printk("    page_access_type    : %i\n", (int)entry->page_access_type);
    printk("    global              : %i\n", (int)entry->global);
    printk("    page_frame_number   : %llx\n", (uint64_t)entry->page_frame_number);
    printk("    protection_key      : %i\n", (int)entry->protection_key);
    printk("    execute_disable     : %i\n", (int)entry->execute_disable);
}

uint64_t read_cr3(void) {
    uint64_t cr3_value;
    asm volatile("mov %%cr3, %0" : "=r" (cr3_value));
    return cr3_value;
}

void* get_vaddr(uint64_t addr) {
    return (void*)(addr + KERNEL_PAGE_OFFSET);
}

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

void pagetable_walk_v1(uint64_t page_limit) {
    struct vm_area_struct* vma;
    struct mm_struct* task_mm;
    unsigned long vmpage;

    printk("Beginning pagewalk_v1, current_task: %p\n", current);

    task_mm = current->mm;
    printk("current_task->mm: %p\n", task_mm);

    if ((task_mm && task_mm->mmap)) {
        uint64_t pages_walked;
        pgd_t* pgd;
        p4d_t* p4d;
        pud_t* pud;
        pmd_t* pmd;
        pte_t* pte;

        pages_walked = 0;
        vma = task_mm->mmap;
        printk("vma: %p\n", vma);

        printk("vma->vm_start  : 0x%lx\n", vma->vm_start);
        printk("vma->vm_end    : 0x%lx\n", vma->vm_end);
        printk("region size    : %li pages\n", (vma->vm_end - vma->vm_start) / PAGE_SIZE);

        for (vmpage = vma->vm_start; vmpage < vma->vm_end; vmpage += PAGE_SIZE) {
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

            print_page_table_entry((struct page_table_entry*)pte);

            printk("\n");

            ++pages_walked;

            if (page_limit > 0 && pages_walked >= page_limit)
                break;
        }
    }
}

void pagetable_walk_v2(int page_limit) {
    struct vm_area_struct* vma;
    struct mm_struct* task_mm;
    unsigned long vmpage;

    printk("Beginning pagewalk_v2, current_task: %p\n", current);

    task_mm = current->mm;
    printk("current_task->mm: %p\n", task_mm);

    if ((task_mm && task_mm->mmap)) {
        uint64_t pages_walked;
        uint64_t cr3;
        uint64_t pml4_base;
        struct page_index_dict idx_dict;
        struct page_table *pml4, *pml3, *pml2, *pml1;
        struct page_table_entry *pml4e, *pml3e, *pml2e, *pml1e;

        pages_walked = 0;
        vma = task_mm->mmap;
        printk("vma: %p\n", vma);

        printk("vma->vm_start  : 0x%lx\n", vma->vm_start);
        printk("vma->vm_end    : 0x%lx\n", vma->vm_end);
        printk("region size    : %li pages\n", (vma->vm_end - vma->vm_start) / PAGE_SIZE);

        for (vmpage = vma->vm_start; vmpage < vma->vm_end; vmpage += PAGE_SIZE) {
            vaddr_to_page_offsets(vmpage, &idx_dict);

            printk("pml4 index: %lli\n", idx_dict.pt_lvl_4);
            printk("pml3 index: %lli\n", idx_dict.pt_lvl_3);
            printk("pml2 index: %lli\n", idx_dict.pt_lvl_2);
            printk("pml1 index: %lli\n", idx_dict.pt_lvl_1);

            cr3 = read_cr3();

            pml4_base = cr3 & PHYSICAL_PAGE_MASK;
            printk("PML4 (physical) : %p\n", (void*)pml4_base);

            pml4 = (struct page_table*)get_vaddr(pml4_base);
            printk("PML4 (virtual)  : %p\n", pml4);

            pml4e = &pml4->entries[idx_dict.pt_lvl_4];

            pml3 = (struct page_table*)get_vaddr((uint64_t)pml4e->page_frame_number << 12);
            printk("PML3 (virtual)  : %p\n", pml3);

            pml3e = &pml3->entries[idx_dict.pt_lvl_3];

            pml2 = (struct page_table*)get_vaddr((uint64_t)pml3e->page_frame_number << 12);
            printk("PML2 (virtual)  : %p\n", pml2);

            pml2e = &pml2->entries[idx_dict.pt_lvl_2];
            
            pml1 = (struct page_table*)get_vaddr((uint64_t)pml2e->page_frame_number << 12);
            printk("PML1 (virtual)  : %p\n", pml1);

            pml1e = &pml1->entries[idx_dict.pt_lvl_1];

            print_page_table_entry(pml1e);

            ++pages_walked;

            if (page_limit > 0 && pages_walked >= page_limit)
                break;
        }
    }
}

int init_module(void) {
    printk("Kernel Module Loaded!\n");

    // pagetable_walk_v1(0);
    // printk("\n================================================\n");

    pagetable_walk_v2(0);
	return 0;
}

void cleanup_module(void) {
    printk("Kernel Module Unloaded!\n");
}
