//  Interrupt_Descriptor_Table.h
//  Created by kaiwei on 2019/10/18.
//  Copyright © 2019 kaiwei. All rights reserved.
//
#include "ROfilesystem.h"
#include "types.h"
#include "lib.h"
#include "system_call.h"
static int32_t globalindex=0;


/*
 * read_dentry_by_name
 *   DESCRIPTION: reads dir. entry in the file system by name
 *   INPUTS: file name and empty dir. entry
 *   OUTPUTS: dir. entry with data 
 *   RETURN VALUE: 0 for  success, -1 for failure
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    int i;
    unsigned int len;
    
    len =strlen((int8_t*)fname);//get the length of file name
    if(len==0||len>length_32B_to_int8){   // if the length of file name wrong return -1 for failure
        return -1;    
    }
    for(i=0;i<dentrysize;i++){
        if(!strncmp((int8_t*)fname,bootblockptr->dir_entry[i].file_name,length_32B_to_int8)){  // compare the string with name if same then get the data to dentry
            copy_dentry(dentry,&(bootblockptr->dir_entry[i]));
            return 0;   
        }
    }
    return -1; // if no same return -1 for fail 
}
/*
 * read_dentry_by_index
 *   DESCRIPTION: reads dir. entry in the file system by index
 *   INPUTS: index and empty dir. entry
 *   OUTPUTS: dir. entry with data 
 *   RETURN VALUE: 0 for  success, -1 for failure
 *   SIDE EFFECTS: none
 */

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry ){
    // if bigger than the total num of dentries , return -1 for failure
    if(index==bootblockptr->dentry_num){ // this is used to deal with the ls command
        globalindex=-1;
        return 0;
    }
    if(index>bootblockptr->dentry_num){
        return -1;
    }
    copy_dentry(dentry,&(bootblockptr->dir_entry[index]));  //copy the data directly
    return 0;
}
/*
 * read_data
 *   DESCRIPTION: reads the file data from file system 
 *   INPUTS: num of inodes; offset; buf; length
 *   OUTPUTS: buf with the data
 *   RETURN VALUE: bytes read or -1 for fail 
 *   SIDE EFFECTS: none
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length ){
    uint32_t i,num_datablock,bufadd;
    uint32_t lengthhalfway; //used as a length counter for 
    inode_t* index_node;
    uint32_t index_datablocks;
    uint8_t*  datablocks_start;
    uint8_t* datablocks_ptr;
    // if bigger than the total num of inodes , return -1 for failure
    if(inode>=bootblockptr->inode_num){
        return -1;
    }
    // to be clear , the bootclock and inodes are store in ptr so only need to add one for next inode to get the ptr
    index_node = (inode_t*) bootblockptr + (1 + inode);// because of the bootblock we need a 1, get the inode
    datablocks_start=((uint8_t* )bootblockptr + (1 + bootblockptr->inode_num)*datablocks_size); // get the starting addr of datablocks
    index_datablocks=index_node->nthdatablock_num[offset/datablocks_size];  // get the index of datablocks 
    datablocks_ptr=((uint8_t*)(datablocks_start))+index_datablocks*datablocks_size+offset%datablocks_size; // get the real starting addr
    

    bufadd=0;
     //memcpy(buf,datablocks_ptr,length);
    if(length<=datablocks_size-offset%datablocks_size){
         memcpy(buf,datablocks_ptr,length);
    }else{    
        lengthhalfway=datablocks_size-(offset%datablocks_size); // get the first copy length
        num_datablock=(length-lengthhalfway)/datablocks_size;// get the datablocks left      4096 is the size of datablock

        num_datablock+=1;   //  we need to add one for the first block
        for (i=0;i<num_datablock;i++){
            memcpy(buf+bufadd,datablocks_ptr,lengthhalfway);
            bufadd+=lengthhalfway;
            lengthhalfway=datablocks_size;  // change the read byte to 4096
            index_datablocks=index_node->nthdatablock_num[offset/datablocks_size+i+1];
            if (bufadd+offset>=index_node->length){  // if the is no datablock after , beturn the read num of byte
                return bufadd;
            }
            datablocks_ptr=((uint8_t*)datablocks_start)+index_datablocks*datablocks_size;
        }

       memcpy(buf+bufadd,datablocks_ptr,length-bufadd);//copy the last part of data blok
    }
    
    return length;


}





// fd means file descriptor

/*
 * file_open
 *   DESCRIPTION: opens the file 
 *   INPUTS: fname 
 *   OUTPUTS: none
 *   RETURN VALUE:  0 for  success, -1 for failure
 *   SIDE EFFECTS: none
 */
int32_t file_open(const uint8_t* fname){
   // if can not find the file return -1 for fail  
   // return  read_dentry_by_name((uint8_t*)fname, &tempdentry);
   return 0;
}

/*
 * file_close
 *   DESCRIPTION: close the file 
 *   INPUTS: fname 
 *   OUTPUTS: none
 *   RETURN VALUE:  0 for  success, -1 for failure
 *   SIDE EFFECTS: none
 */
int32_t file_close(int32_t fd){
    /*
    int i;
    for(i=0;i<length_32B_to_int8;i++){
        tempdentry.file_name[i]=0;  // clear the dentry's filename
    }// clear the file name
    tempdentry.file_type=0; // set file type to 0
    tempdentry.inode_num=0; // set inode num to 0
    for(i=0;i<length_24B_to_uint32;i++){// clear the reserved bits
        tempdentry.reserved_bits[i]=0;
    }
    */
    //clean the temp dentry
    return 0;
}






/*
 * file_write
 *   DESCRIPTION: write the file 
 *   INPUTS: filed_t* file, void *buf, int32_t nbyte
 *   OUTPUTS: none
 *   RETURN VALUE:  0 for  success, -1 for failure
 *   SIDE EFFECTS: none
 */
int32_t file_write(int32_t fd,void *buf, int32_t nbyte){
    return -1;//  read only file system
}


// ps. for the write and read, we can only give a ptr to filed_t because we can not get fd now from pcb
// after done PCB we may only need give a index to replace the  filed_t* 


/*
 * file_read
 *   DESCRIPTION: read the file 
 *   INPUTS: int32_t fd, void *buf, int32_t nbyte
 *   OUTPUTS: none
 *   RETURN VALUE:  0 for  success, -1 for failure
 *   SIDE EFFECTS: none
 */
int32_t file_read(int32_t fd,void *buf, int32_t nbyte){
    uint32_t offset;
    uint32_t inodes;
    inode_t*  index_node;
    int32_t retvalue=0;
    pcb_t* current_pcb_ptr1 = get_PCB();
   
    if(fd<=0 || fd>RO8)
		return -1;
    index_node = (inode_t*) bootblockptr + (1 + current_pcb_ptr1->fds[fd].inode);
    uint32_t total_length= index_node->length;
    offset= current_pcb_ptr1->fds[fd].location;
    inodes= current_pcb_ptr1->fds[fd].inode;
    retvalue=read_data(inodes,offset,buf,nbyte);
    if(retvalue>0){
        if ( (total_length-current_pcb_ptr1->fds[fd].location)<retvalue){
            retvalue=total_length-current_pcb_ptr1->fds[fd].location;
            current_pcb_ptr1->fds[fd].location=total_length;
            return retvalue;
            
        }
        current_pcb_ptr1->fds[fd].location+= retvalue; // add the num of read byte to offset
        //return retvalue;  // if >0 means we read byte into bufsuccefully
        return retvalue;
    }else{
        return -1;  
    } //  read only file system

}

/*
int32_t file_read(filed_t* file, void *buf, int32_t nbyte){
    
    return read_data(file->inode,file->fileposition,buf,nbyte);//  read only file system
}
*/

/*
 * directory_open
 *   DESCRIPTION: open the dir 
 *   INPUTS: fname
 *   OUTPUTS: none
 *   RETURN VALUE:  0 for  success, -1 for failure
 *   SIDE EFFECTS: none
 */

int32_t directory_open(const uint8_t* fname){
    return read_dentry_by_name((uint8_t*)fname, &tempdentry);
}// open the dir


/*
 * directory_close
 *   DESCRIPTION: close the dir 
 *   INPUTS: fname
 *   OUTPUTS: none
 *   RETURN VALUE:  0 for  success, -1 for failure
 *   SIDE EFFECTS: none
 */


int32_t directory_close(int32_t fd){
    
    int i;
    for(i=0;i<length_32B_to_int8;i++){
        tempdentry.file_name[i]=0;  // clear the dentry's filename
    }// clear the file name
    tempdentry.file_type=0; // set file type to 0
    tempdentry.inode_num=0; // set inode num to 0
    for(i=0;i<length_24B_to_uint32;i++){// clear the reserved bits
        tempdentry.reserved_bits[i]=0;
    }
    
    //clean the temp dentry
    return 0;
}// close the file



/*
 * directory_read
 *   DESCRIPTION: read the dir 's fname 
 *   INPUTS: filed_t* file, void *buf, int32_t nbyte
 *   OUTPUTS: none
 *   RETURN VALUE:  0 for  success, -1 for failure
 *   SIDE EFFECTS: none
 */
int32_t directory_read(int32_t fd,void *buf, int32_t nbyte){
    int i; 

    //globalindex=globalindex%bootblockptr->dentry_num;  
    for(i=0;i<length_32B_to_int8;i++){
        tempdentry.file_name[i]=0;  // clear the dentry's filename
    }
    
    if(read_dentry_by_index(globalindex, &tempdentry)==-1){  // read the fname by index
        return -1;
    }
    if(nbyte>32){// 32 is the max length of file name
        nbyte=32;
    }
    
    memcpy((uint8_t*)buf,tempdentry.file_name,nbyte);   // copy the fname to buf
    globalindex++;
    if (strlen(tempdentry.file_name)>=length_32B_to_int8){
        return length_32B_to_int8;
    }
	return strlen(tempdentry.file_name);	
}// read the file // not deal with the dentry yet for we don't know where to store it 

/*
 * directory_write
 *   DESCRIPTION: write the dir 
 *   INPUTS: filed_t* file, void *buf, int32_t nbyte
 *   OUTPUTS: none
 *   RETURN VALUE:  0 for  success, -1 for failure
 *   SIDE EFFECTS: none
 */
int32_t directory_write( int32_t fd,void *buf, int32_t nbyte){
    return -1;  //read only file system
}




/*
 * getfiledescriptor
 *   DESCRIPTION: get the filedescriptor by process control block(PCB)
 *   INPUTS: none
 *   OUTPUTS: a filedescriptor
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
/*
void getfiledescriptor(){
    file_descriptor=NULL; // get data stored in PCB  which should be done in  6.3.5
}
*/      

/*
 * getbootblockptr
 *   DESCRIPTION: get the bootblock's ptr and store it it in bootblockptr
 *   INPUTS: start addr of file system 
 *   OUTPUTS: bootblockptr
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void getbootblockptr(uint32_t addr){
    bootblockptr=(struct bootblock*) addr;   
}

/*
 * getbootblockptr
 *   DESCRIPTION: copy data from src to dest
 *   INPUTS: two dentries
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void copy_dentry(dentry_t* dest,dentry_t* src){ //copy dentry from d1 to d2
    int i;
    strcpy(dest->file_name,src->file_name);// copy the file name
    dest->file_type=src->file_type;
    dest->inode_num=src->inode_num;
    for(i=0;i<length_24B_to_uint32;i++){// copy the reserved bits
        dest->reserved_bits[i]=src->reserved_bits[i];
    }
}





