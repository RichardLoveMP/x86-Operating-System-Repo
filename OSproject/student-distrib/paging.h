
#ifndef _PAGEING_H
#define _PAGEING_H

#define PROGRAM_IMAGE 0x08048000
#define ADDRESS_KERNEL_BEGIN 0x00400000
#define ADDRESS_VIDEO_BEGIN 0xB8000
#define PTE_VIDEO_INDEX 0xB8
#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE 1024        //TODO:
#define PAGE_SIZE_ONE_PAGE 4096
#define VIRTOINDEX 0x400000     //  just 4MB for one

#define NUM_MAX_PROCESS 6

#define PAGE33 33

#define ALIGN_RESERVE_SPACE   0

// the offset is 12 because only the upper 20 bits serves as PDBR
// (page directory base register).
#define OFFSET_TWELVE 12

/* It's OK if here we just define int_t as pde_t and pte_t,
   but i think writing them out explicitly is better to understand. */
typedef union pde_t {
    // ref: intel manual 3.24
	uint32_t val;
	struct {
		uint32_t present							: 1;
		uint32_t read_or_write 						: 1;
		uint32_t user_or_supervisor 				: 1;
		uint32_t write_through 					: 1;
		uint32_t cache_disabled : 1;
		uint32_t accessed : 1;
		uint32_t reserved : 1;
		uint32_t page_size : 1;
		uint32_t global_page : 1;
		uint32_t avail_for_system_programmer_use : 3;
		uint32_t page_base_address : 20;
	} __attribute__((packed));
} pde_t;

typedef union pte_t {
    // ref: intel manual 3.24
	uint32_t val;
	struct {
		uint32_t present: 1;
		uint32_t read_or_write : 1;
		uint32_t user_or_supervisor : 1;
		uint32_t write_through : 1;
		uint32_t cache_disabled : 1;
		uint32_t accessed : 1;
		uint32_t dirty : 1;
		uint32_t page_table_attribute_index : 1;
		uint32_t global_page : 1;
		uint32_t avail_for_system_programmer_use : 3;
		uint32_t page_base_address : 20;
	} __attribute__((packed));
} pte_t;


//pde_t page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned (PAGE_SIZE_ONE_PAGE)));
//pte_t page_table[PAGE_TABLE_SIZE] __attribute__((aligned (PAGE_SIZE_ONE_PAGE)));

typedef struct page_directory_array_t{
	pde_t page_directory[PAGE_DIRECTORY_SIZE];  

} page_directory_array_t;

typedef struct page_table_array_t{
	pte_t page_table[PAGE_TABLE_SIZE];  

} page_table_array_t;

page_directory_array_t page_directory_array[NUM_MAX_PROCESS] __attribute__((aligned (PAGE_SIZE_ONE_PAGE)));
page_table_array_t page_table_array[NUM_MAX_PROCESS * 2] __attribute__((aligned (PAGE_SIZE_ONE_PAGE)));

void initialize_paging();

void CR3_setting(uint32_t* page_directory);

void clear_page(void* ptr);

void flush_TLB();

extern void SETPAGE(int pid);

void set_page_directory_entry(int pid, int idx, uint32_t pba, uint32_t psize, uint32_t userORsup);

void set_page_table_entry(int pid,uint32_t pba);

extern void paging_video_memory_switch(int cpid);

#endif
