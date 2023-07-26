#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <LINF/sym_all.h>

#define PAGE_SIZE        4096
#define NUM_ENTRIES      512
#define PAGE_PRESENT     0x1

#define PHYSICAL_PAGE_MASK  0x000FFFFFFFFFF000
#define KERNEL_PAGE_OFFSET  0xffff888000000000

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

void* get_vaddr(uint64_t addr) {
    return (void*)(addr + KERNEL_PAGE_OFFSET);
}

uint64_t read_cr3(void) {
    uint64_t cr3_value;
    asm volatile("mov %%cr3, %0" : "=r" (cr3_value));
    return cr3_value;
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

struct page_table_entry* get_pte(uint64_t addr) {
    uint64_t cr3;
    uint64_t pml4_base;
    struct page_index_dict idx_dict;
    struct page_table *pml4, *pml3, *pml2, *pml1;
    struct page_table_entry *pml4e, *pml3e, *pml2e, *pte;

    vaddr_to_page_offsets(addr, &idx_dict);

    cr3 = read_cr3();
    pml4_base = cr3 & PHYSICAL_PAGE_MASK;

    pml4 = (struct page_table*)get_vaddr(pml4_base);

    pml4e = &pml4->entries[idx_dict.pt_lvl_4];
    if (!pml4e->present)
        return NULL;

    pml3 = (struct page_table*)get_vaddr((uint64_t)pml4e->page_frame_number << 12);

    pml3e = &pml3->entries[idx_dict.pt_lvl_3];
    if (!pml3e->present)
        return NULL;

    pml2 = (struct page_table*)get_vaddr((uint64_t)pml3e->page_frame_number << 12);

    pml2e = &pml2->entries[idx_dict.pt_lvl_2];
    if (!pml2e->present)
        return NULL;

    pml1 = (struct page_table*)get_vaddr((uint64_t)pml2e->page_frame_number << 12);

    pte = &pml1->entries[idx_dict.pt_lvl_1];

    if (!pte->present)
        return NULL;

    return pte;
}

uint64_t walk_pagetables() {
    uint64_t pages_visited;
    uint64_t pages_iterated;
    uint64_t vmpage;
    struct page_table_entry* pte;

    pages_iterated = 0;

    for (pages_visited = 0, vmpage = 0; vmpage < 0x100000000; vmpage += PAGE_SIZE) {
        ++pages_iterated;

        pte = get_pte(vmpage);

        if (!pte)
            continue;

        print_page_table_entry(pte);
        ++pages_visited;
    }

    return pages_visited;
}

int main() {
    void* ksys_write = sym_get_fn_address("ksys_write");
    printf("ksys_write is at %p\n", ksys_write);

    sym_elevate();
    printf("Is PCIDE Enabled: %i\n", is_pcide_enabled());

    uint64_t pages_visited = walk_pagetables();
    printf("\n==============================\n");
    printf("----- Visited %li Pages -----\n", pages_visited);
    printf("==============================\n\n");

    unsigned char* ptr = (unsigned char*)malloc(30 * PAGE_SIZE);
    ptr[1 * PAGE_SIZE] = 'h';
    ptr[2 * PAGE_SIZE] = 'e';
    ptr[3 * PAGE_SIZE] = 'l';
    ptr[4 * PAGE_SIZE] = 'l';
    ptr[5 * PAGE_SIZE] = 'o';
    ptr[6 * PAGE_SIZE] = 'w';
    ptr[7 * PAGE_SIZE] = 'o';
    ptr[8 * PAGE_SIZE] = 'r';
    ptr[9 * PAGE_SIZE] = 'l';
    ptr[10 * PAGE_SIZE] = 'd';
    ptr[25 * PAGE_SIZE] = 'd';

    pages_visited = walk_pagetables();
    printf("\n==============================\n");
    printf("----- Visited %li Pages -----\n", pages_visited);
    printf("==============================\n\n");

    sym_lower();

    return 0;
}
