#include "x86_desc.h"
#include "lib.h"

#include "paging.h"


#define TWEL 12 

#define MB8 0x800000
#define SIZE128MB 0x8000000
#define KB8 0x2000
#define SIZE4KB 0x1000
#define KERNEL_PHYSICAL 0x400000
#define VIDEO_BASE 0xB8
#define VIDEO 0xB8000
#define SIZE4MB 0x400000
#define THIRTYTWO 32
#define PAGE_DISPATCH_SIZE 4
#define MEMSET_PARA (1024*4)
#define PAGING_THREE 3

uint32_t page_dispatch_array[PAGE_DISPATCH_SIZE] = {0,1,THIRTYTWO,VIDEO_BASE};

/**
 * initialize_paging
 *   DESCRIPTION: initialze the paging. 
 *                Kernal code is already loaded at 4MB. so we need only map
 *                virtual memory 4-8MB to physical memory at 4-8MB. This 
 *                kernal page should be a single 4MB page, whiereas the first
 *                4MB of memory should broken down into 4kB pages.
 *                Also, 8MB to 4GB being marked not present, here should Also
 *                set any unused pages to not present as well.
 *                In this layout everything in the first 4MB, that isn't the
 *                page for video memory, should be marked not present.
 *                Make sure to align the pages/page directory/page table on
 *                4kB boundaries.
 *                We can assume that there is more than 16MB of memory.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: noen
 *   SIDE EFFECTS: create 1 page directory and 1 page table, map the 1st entry
 *                 of the directory to the table.
*/
void initialize_paging(){
    int i;

    for (i=0; i<PAGE_DIRECTORY_SIZE; i++){
        page_directory_array[0].page_directory[i].val = 0;
        page_directory_array[0].page_directory[i].read_or_write = 1;
    }

    for (i=0; i<PAGE_TABLE_SIZE; i++){
        page_table_array[0].page_table[i].val = 0;
        page_table_array[0].page_table[i].read_or_write = 1;
    }
   // page_table[0].present = 1;

    // get page_table's entrance 
  //  pt_temp = (uint32_t*) page_table;

    // initialze the 1st and second entry of page_directory.
    // i.e., build the 4MB and 4kB page.

    // set [0] to point to video memory's pt. 
    page_directory_array[0].page_directory[0].present = 1;
    page_directory_array[0].page_directory[0].page_base_address=((uint32_t) page_table_array[0].page_table >> TWEL);

    // set the 4MB page
    page_directory_array[0].page_directory[1].present = 1;
    page_directory_array[0].page_directory[1].page_size = 1;
    //page_directory_array[0].page_directory[1].user_or_supervisor = 0;
    page_directory_array[0].page_directory[1].global_page = 1;// virtual to physical translations that use this page are visible to all processes
    page_directory_array[0].page_directory[1].page_base_address = ADDRESS_KERNEL_BEGIN >> TWEL;

    // set the 4kB video memory page

    page_table_array[0].page_table[PTE_VIDEO_INDEX].present = 1;
    //page_table_array[0].page_table[PTE_VIDEO_INDEX].user_or_supervisor = 0;
        page_table_array[0].page_table[PTE_VIDEO_INDEX].page_base_address = ADDRESS_VIDEO_BEGIN >> TWEL;

    
    asm volatile(
        // load PD address to CR3
        "movl   %0, %%eax;"
        "movl   %%eax, %%cr3;"
        // enable 4MB paging (bit 4 for CR4)
        "movl   %%cr4, %%eax;"
        "orl    $0x00000010, %%eax;"
        "movl   %%eax, %%cr4;"
        // enable paging (bit 31 for CR0)
        "movl   %%cr0, %%eax;"
        "orl    $0x80000000, %%eax;"
        "movl   %%eax, %%cr0;"
        :
        :"r"((uint32_t*)page_directory_array[0].page_directory)
        :"eax", "cc"
    );

}



/* 
 * CR3_setting  TODO:
 *   DESCRIPTION: set CR3 helper function. 
 *   INPUTS: page_directory
 *   OUTPUTS: None
 *   RETURN VALUE: None
 *   SIDE EFFECTS: set the CR3 to page_directory.
 */

void CR3_setting(uint32_t* page_directory) {
    
    asm volatile(
	// load PD address to CR3
        "movl   %0, %%cr3;"

        :
        :"a" (page_directory)
    //	:"eax", "cc"
        );
}


/* 
 * clear_page  
 *   DESCRIPTION: clean a page. 
 *   INPUTS: ptr
 *   OUTPUTS: None
 *   RETURN VALUE: None
 *   SIDE EFFECTS: clear a page
 */


void clear_page(void* ptr){
    if (ptr==NULL){
        return;
    }
    memset(ptr, 0, MEMSET_PARA);
}






/* 
 * flush_TLB
 *   DESCRIPTION: flush TLB after swapping page
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: none 
 */
void flush_TLB(){
    asm volatile(
        // reload PD address to CR3
        "mov   %%cr3, %%eax;"
        "mov   %%eax, %%cr3;"
        

        :                 
        :
        :"%eax"                  // may influence the eax
    );

}

/* 
 * SETPAGE
 *   DESCRIPTION: convert virtual to physics
 *   INPUTS: pid
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: none 
 */
void SETPAGE(int pid){
    int i = 0;
	// Enabling paging


    /* set the entry */
    for(i = 0; i < PAGING_THREE; i++){
        int idx = page_dispatch_array[i];
        if(idx == 0)    // set the page table 
            set_page_directory_entry(pid,idx,((uint32_t) &page_table_array[pid].page_table >> TWEL),0,0);
        if(idx == 1)
            set_page_directory_entry(pid,idx,KERNEL_PHYSICAL >> TWEL, 1,0);
        if(idx == THIRTYTWO)    //page entry
            set_page_directory_entry(pid,idx,(MB8 + pid*SIZE4MB) >> TWEL,1,1);
    }

    /* set the table */
    set_page_table_entry(pid,VIDEO >> TWEL);


}


/* 
 * set_page_directory_entry
 *   DESCRIPTION: set the page directory entry to the given value
 *   INPUTS: int pid, int idx, uint32_t pba, uint32_t psize, uint32_t userORsup
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: modify the page structure 
 */
void set_page_directory_entry(int pid, int idx, uint32_t pba, uint32_t psize, uint32_t userORsup){
    // set page size to the psize
    page_directory_array[pid].page_directory[idx].page_size = psize;

    // set the userorsup
    page_directory_array[pid].page_directory[idx].user_or_supervisor = userORsup;
   
    // set R&W
    page_directory_array[pid].page_directory[idx].read_or_write = 1;

    // set the page to address
    page_directory_array[pid].page_directory[idx].page_base_address = pba;
    // set the present address
    page_directory_array[pid].page_directory[idx].present = 1;
    //page_directory_array[pid].page_directory[idx].page_size = psize;

    return ;
}

/* 
 * set_page_table_entry
 *   DESCRIPTION: set the page table entry to the given value
 *   INPUTS: int pid, int idx, uint32_t pba, uint32_t psize, uint32_t userORsup
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: modify the page structure 
 */
void set_page_table_entry(int pid,uint32_t pba){

    // set the user or supervisor
    page_table_array[pid].page_table[VIDEO_BASE].user_or_supervisor = 0;
	
    // set the R&W
    page_table_array[pid].page_table[VIDEO_BASE].read_or_write = 1;
    // set the base address
    page_table_array[pid].page_table[VIDEO_BASE].page_base_address = pba;
	
    // set the present
    page_table_array[pid].page_table[VIDEO_BASE].present = 1;
	
  
    return ;
}

/* 
 * paging_video_memory_switch
 *   DESCRIPTION: change the video memory: we only show the video memory of the 
 *                current terminal. Also remember to backup the not-shown terminal's
 *                video mem.
 *   INPUTS: int pid, int idx, uint32_t pba, uint32_t psize, uint32_t userORsup
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: modify the page structure 
 */
void paging_video_memory_switch(int cpid){
    page_table_array[cpid].page_table[VIDEO_BASE].page_base_address = 
		page_table_array[cpid].page_table[VIDEO_BASE_CP].page_base_address;

	if(page_directory_array[cpid].page_directory[PAGE33].present == 1){
		page_table_array[MAX_PROCESSES + cpid].page_table[0].page_base_address =
			page_table_array[cpid].page_table[VIDEO_BASE_CP].page_base_address;
	}
    return ;
}



