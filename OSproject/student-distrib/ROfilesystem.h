//  Interrupt_Descriptor_Table.h
//  Created by kaiwei on 2019/10/18.
//  Copyright © 2019 kaiwei. All rights reserved.
//

#ifndef ROFILESYSTEM_H
#define ROFILESYSTEM_H


#include "types.h"
#define RO8 8
#define length_32B_to_int8 32   // totally can have up to 32 char
#define length_24B_to_uint32 6 // totally can have up to 6 reserved bits
#define length_52B_to_uint32 13 // totally can have up to 13 reserved bits
#define dentrysize 63 // the file system can hold up to 63 files/// except the directory itself// can only get 62 others
#define datablocks_in_inodes_size 1023 //  totaly 4KB=1024 *4B  execpt the length  we will have 1023 for n th data block #  
#define datablocks_size 4096 // totaly 4 kb here
// define the structure of dir. entries
typedef struct dentry{
	int8_t file_name[length_32B_to_int8];    // total can be 32B    it /1  == 32 int8 
	uint32_t file_type;    // file type use 4B 
	uint32_t inode_num;     // number of inodes use 4B 
	uint32_t reserved_bits[length_24B_to_uint32];  //left 24B, it /4   ==6  
} dentry_t;

struct bootblock{
	uint32_t dentry_num;// number of dir.entry use 4B 
	uint32_t inode_num;// number of inodes use 4B 
	uint32_t data_block_num;// number of data blocks use 4B 
	uint32_t reserved_bits[length_52B_to_uint32];  // just 52B/4B ==13
	dentry_t dir_entry[dentrysize];   /// used 64B
};

typedef struct {
	uint32_t length;  // length in B
	uint32_t nthdatablock_num[datablocks_in_inodes_size];    //  totaly 4KB=1024 *4B  execpt the length  we will have 1023 for n th data block #   
}inode_t;

typedef struct {
	uint32_t file_OTptr;  // file operations table pointer
	uint32_t inode;    // num of inode
    uint32_t fileposition;
    uint32_t flag;
}filed_t;

/*
typedef struct {
    //filed_t stdin;
    //filed_t stdout;
	filed_t filr_descriptor[8];  
}filedtable_t;


filedtable_t* file_descriptor_table;
*/
struct bootblock* bootblockptr;
dentry_t tempdentry;  // used for read data  in cp 2


//reads dir. entry into the file system by name
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry );

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry );
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length );

void copy_dentry(dentry_t* d1,dentry_t* d2);//copy the dentry
void getbootblockptr(uint32_t addr);//get the bootblock's ptr and store it it in bootblockptr
//void getfiledescriptor(); // get file descriptor from pcb
// read only file system
int32_t file_open(const uint8_t* fname);// open the file
int32_t file_close(int32_t fd);// close the file
//int32_t file_read(filed_t* file, void *buf, int32_t nbyte);// read the file
int32_t file_read(int32_t fd,void *buf, int32_t nbyte);
//int32_t file_write(filed_t* file, void *buf, int32_t nbyte);// write the file
int32_t file_write(int32_t fd,void *buf, int32_t nbyte);

int32_t directory_open(const uint8_t* fname);// open the dir
int32_t directory_close(int32_t fd);// close the file
int32_t directory_read(int32_t fd, void *buf, int32_t nbyte);
//int32_t directory_read(int32_t fd, void *buf, int32_t nbyte,int32_t index);// read the file
int32_t directory_write(int32_t fd, void *buf, int32_t nbyte);// write the file

#endif /* ROFILESYSTEM_H */
