#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <LINF/sym_all.h>
#include <dlfcn.h>

#define PAGE_SIZE        4096
#define NUM_ENTRIES      512
#define PAGE_PRESENT     0x1

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
    printf("------ page_table_entry info ------\n");
    printf("    present             : %i\n", (int)entry->present);
    printf("    read_write          : %i\n", (int)entry->read_write);
    printf("    user_supervisor     : %i\n", (int)entry->user_supervisor);
    printf("    page_write_through  : %i\n", (int)entry->page_write_through);
    printf("    page_cache_disabled : %i\n", (int)entry->page_cache_disabled);
    printf("    accessed            : %i\n", (int)entry->accessed);
    printf("    dirty               : %i\n", (int)entry->dirty);
    printf("    page_access_type    : %i\n", (int)entry->page_access_type);
    printf("    global              : %i\n", (int)entry->global);
    printf("    page_frame_number   : %lx\n",(uint64_t)entry->page_frame_number);
    printf("    protection_key      : %i\n", (int)entry->protection_key);
    printf("    execute_disable     : %i\n", (int)entry->execute_disable);
    printf("\n");
}

uint64_t read_cr4(void) {
    uint64_t cr4_value;
    asm volatile("mov %%cr4, %0" : "=r" (cr4_value));
    return cr4_value;
}

int is_pcide_enabled() {
    uint64_t cr4 = read_cr4();
    return (cr4 >> 17) & 1;
}

extern void* get_task_base_vma(void* task);
extern void* get_next_vma(void* vma);

extern uint64_t get_task_vma_start(void* vma);
extern uint64_t get_task_vma_end(void* vma);

extern struct page_table_entry* get_pte_for_address(void* task, uint64_t addr);

void walk_pagetable(int should_print_pte) {
    uint64_t pages_visited = 0;
    uint64_t pages_present = 0;
    void* current_task = get_current_task();
    
    void* vma = get_task_base_vma(current_task);
    while (vma) {
        uint64_t vm_start = get_task_vma_start(vma);
        uint64_t vm_end = get_task_vma_end(vma);

        if (should_print_pte) {
            printf("vma->vm_start : 0x%lx\n", vm_start);
            printf("vma->vm_end   : 0x%lx\n", vm_end);
        }

        for (uint64_t vmpage = vm_start; vmpage < vm_end; vmpage += PAGE_SIZE) { 
            struct page_table_entry* pte = get_pte_for_address(current_task, vmpage);
            if (!pte)
                continue;

            ++pages_visited;

            if (pte->present) {
                ++pages_present;
                if (should_print_pte) {
                    print_page_table_entry(pte);
                }
            }
        }

        vma = get_next_vma(vma);
    }

    printf("\n==============================\n");
    printf("----- Visited %li Pages -----\n", pages_visited);
    printf("     Pages Present : %li\n", pages_present);
    printf("     Pages Absent  : %li\n", (pages_visited - pages_present));
    printf("==============================\n\n");
}

extern uint64_t printk;
extern void* page_offset_base;

int main() {
    sym_elevate();
    printf("Is PCIDE Enabled: %i\n", is_pcide_enabled());
    printf("page_offset_base: %p\n", page_offset_base);

    walk_pagetable(0);

    unsigned char* ptr = (unsigned char*)malloc(5 * PAGE_SIZE);
    ptr[1 * PAGE_SIZE - 1] = 'h';
    ptr[2 * PAGE_SIZE - 1] = 'e';
    ptr[3 * PAGE_SIZE - 1] = 'l';
    ptr[4 * PAGE_SIZE - 1] = 'l';
    ptr[5 * PAGE_SIZE - 1] = 'o';

    walk_pagetable(0);

    sym_lower();

    return 0;
}
