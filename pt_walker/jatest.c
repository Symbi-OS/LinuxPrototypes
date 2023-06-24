#include <stdint.h>
#include <stdio.h>
#include <L0/sym_lib.h>
#include <inttypes.h>
#include <x86_64Constants.h>
#include <x86_64vmm.h>



struct START_PAGE_TABLE {
  X86_VMM_PML4E PML4PAGE[X86_VMM_NUMENTRIES];
  X86_VMM_PDPTE PDPTPAGE[X86_VMM_NUMENTRIES];
  X86_VMM_PDE   PDPAGE[X86_VMM_NUMENTRIES];
}  __attribute__((packed)) StartPageTable =
  {
   .PML4PAGE =
   {
    [0] = { 
	   .present = {
		       .PRESENT = 1,   // This descriptor is valid 'present'
		       .RW      = 1,   // area is writable
		       .PWT     = 0,   // area is not write through
		       .PCD     = 0,   // area is not cache disabled
		       .A       = 0,   // clear access bit
		       .RSV0    = 0,
		       //.PDPT_ADDR_BITS = (T86_PAGETABLE_PDPT_ADDR >> 12),
		       .XD      = 0
		       }
    },
    [1 ... X86_VMM_NUMENTRIES-1] = { .notpresent =  { .PRESENT = 0, .VALUE = 0 } } 
   },
   
   .PDPTPAGE =
   {
    [0] = {
	   .ps1 = {
		   .PRESENT = 1,
		   .RW      = 1,
		   .PWT     = 0,
		   .PCD     = 0,
		   .A       = 0,
		   .D       = 0,
		   .PS      = 1,
		   .G       = 1,
		   .PAT     = 0,
		   .PADDR   = (0x00000000 >> 30)
		   }
    },
    [1] = {
	   .ps1 = {
		   .PRESENT = 1,
		   .RW      = 1,
		   .PWT     = 0,
		   .PCD     = 0,
		   .A       = 0,
		   .D       = 0,
		   .PS      = 1,
		   .G       = 1,
		   .PAT     = 0,
		   .PADDR   = (0x40000000 >> 30) 
		   }
    },
    [2] = {
	   .ps1 = {
		   .PRESENT = 1,
		   .RW      = 1,
		   .PWT     = 0,
		   .PCD     = 0,
		   .A       = 0,
		   .D       = 0,
		   .PS      = 1,
		   .G       = 1,
		   .PAT     = 0,
		   .PADDR   = (0x80000000 >> 30)
		   }
    },
    [3] = {
	   .ps1 = {
		   .PRESENT = 1,
		   .RW      = 1,
		   .PWT     = 0,
		   .PCD     = 0,
		   .A       = 0,
		   .D       = 0,
		   .PS      = 1,
		   .G       = 1,
		   .PAT     = 0,
		   .PADDR   = (0xc0000000 >> 30)
		   }
    },
    /*    [4] = {
	   .ps1 = {
		   .PRESENT = 1,
		   .RW      = 1,
		   .PWT     = 0,
		   .PCD     = 0,
		   .A       = 0,
		   .D       = 0,
		   .PS      = 1,
		   .G       = 1,
		   .PAT     = 0,
		   .PADDR   = (0x100000000 >> 30)
		   }
		   },*/
    
    [4] = {
	   .ps0 = {
		       .PRESENT = 1,   // This descriptor is valid 'present'
		       .RW      = 1,   // area is writable
		       .PWT     = 0,   // area is not write through
		       .PCD     = 0,   // area is not cache disabled
		       .A       = 0,   // clear access bit
		       .PS      = 0,   // PS=0 to indicate we are pointing to pd
		       //		       .PD_ADDR_BITS = (T86_PAGETABLE_PD_ADDR >> 12),
		       .XD      = 0
		       }	   
    },
    [5 ... X86_VMM_NUMENTRIES-5] = { .notpresent =  { .PRESENT = 0, .VALUE = 0 } } 
   },
   .PDPAGE =
   {
    [0] =
    {
            // 2Meg video memory
     	   .ps1 = {
		   .PRESENT = 1,           // Present
		   .RW      = 1,           // area
		   .US      = 0,
		   .PWT     = 0,
		   
		   .PCD     = 0,           // 
		   .A       = 0,           // Accessed
		   .D       = 0,           // Dirty
		   .PS      = 1,           // PS=0  2Meg Page
		   
		   .G       = 1,
		   
		   .PAT     = 0,
		   .RSVD0   = 0,
		   
		   .PADDR   = (0x100000000ULL >> 21)
		   }
    },
    [1 ... X86_VMM_NUMENTRIES-1] = { .notpresent =  { .PRESENT = 0, .VALUE = 0 } }  
   }
  };

void connectPageTableEntries(void)
{
    StartPageTable.PML4PAGE[0].present.PDPT_ADDR_BITS = ((uint64_t)&(StartPageTable.PDPTPAGE[0]) >> 12);
    StartPageTable.PDPTPAGE[4].ps0.PD_ADDR_BITS = ((uint64_t)&(StartPageTable.PDPAGE[0]) >> 12);
}

int main(int argc, char **argv)
{

  connectPageTableEntries();
  
  sym_elevate();
  uint64_t ptbr = CR3_READ_PML4_ADDR();
  printf("ptbr:%"PRIx64"\n", ptbr);
  sym_lower();
  return 0;
}
