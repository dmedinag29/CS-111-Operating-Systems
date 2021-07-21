#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "ext2_fs.h"
#include <math.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#define SUPERBLOCK_OFFSET 1024
struct ext2_super_block super_block;
char err[] = "Incorrect usage: ./lab3a [FILENAME]"; 
int fd_img;
unsigned int block_size;
unsigned int group_count;

void calc_blocksize(){
	block_size = 1024 << super_block.s_log_block_size;
}

void process_superblock(){
	calc_blocksize();
	pread(fd_img, &super_block, sizeof(super_block), SUPERBLOCK_OFFSET);
	printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n",
		super_block.s_blocks_count,
		super_block.s_inodes_count,
		block_size,
		super_block.s_inode_size,
		super_block.s_blocks_per_group,
		super_block.s_inodes_per_group,
		super_block.s_first_ino
	);	
}

void process_freeblocks(unsigned int i, unsigned int block_bitmap, int blocks_in_group){
        unsigned int bitmap_block_offset = block_bitmap * block_size;
	char* bitmap = (char*) malloc(blocks_in_group);
        pread(fd_img, bitmap, blocks_in_group, bitmap_block_offset);
        int byte_i, bit_i;
        unsigned int block_i = super_block.s_first_data_block + (i * super_block.s_blocks_per_group);
	//reading to far in fix this
	for(byte_i = 0; byte_i < (blocks_in_group/8); byte_i++){
                char bite = bitmap[byte_i];
                for(bit_i = 0; bit_i < 8; bit_i++){
                        int bit = bite & 1;
                        if(bit == 0){
                                printf("BFREE,%d\n", block_i);
                        }
			bite >>= 1;
			block_i++;
                }
        }
        free(bitmap);
}

void process_directory(unsigned int block_i, unsigned int inode_i){
	struct ext2_dir_entry dir_entry;
	unsigned offset = block_i * block_size;
	unsigned int entry = 0;
	while(entry < block_size){
		 pread(fd_img, &dir_entry, sizeof(dir_entry), offset + entry);
		 if(dir_entry.inode != 0){
		 	printf("DIRENT,%d,%d,%d,%d,%d,'%s'\n",
				inode_i,
				entry,
				dir_entry.inode,
				dir_entry.rec_len,
				dir_entry.name_len,
				dir_entry.name
			);
		 }
		 entry += dir_entry.rec_len;
	}
}

void process_indirect_block(unsigned int inode_i, unsigned int indirection, unsigned int block_i, unsigned int logical_offset, bool directory){
	unsigned int offset = block_size * block_i;
	unsigned int pointers = block_size/sizeof(__u32);
	unsigned int *direct_blocks = malloc(block_size);
	pread(fd_img,direct_blocks,block_size,offset);
	if(indirection == 2 || indirection == 3){
		for(unsigned int k = 0; k < pointers; k++){
                        process_indirect_block(inode_i, indirection-1, direct_blocks[k], logical_offset, directory);
                }
	}
	for(unsigned int i = 0; i < pointers; i++){
		if(direct_blocks[i] != 0){
			if(indirection == 1 && directory == true){
				process_directory(direct_blocks[i], inode_i);
			} 
		printf("INDIRECT,%d,%d,%d,%d,%d\n",inode_i,indirection,(i * indirection) + logical_offset,block_i,direct_blocks[i]);
		}
	}
	free(direct_blocks);
}

void process_inode_summary(unsigned int inode_i, unsigned int inode_table, unsigned int inodetable_i){
	unsigned int offset = (inode_table * block_size) + (inodetable_i * super_block.s_inode_size);
	struct ext2_inode inode;
	pread(fd_img, &inode, super_block.s_inode_size, offset);
	if(inode.i_mode == 0 || inode.i_links_count == 0){
		return;
	}
	/* check file type*/
	char type = ' ';
	if(S_ISDIR(inode.i_mode)){
		type = 'd';
	}
	else if(S_ISREG(inode.i_mode)){
		type = 'f';
	}
	else if(S_ISLNK(inode.i_mode)){
		type = 's';
	}
	/* get time */
	time_t chg_time = inode.i_ctime;
	time_t mod_time = inode.i_mtime;
	time_t acc_time = inode.i_atime;
	char ctime[20];
	char mtime[20];
	char atime[20];
	struct tm* gmt_time = gmtime(&chg_time);
	strftime(ctime, 20, "%m/%d/%y %H:%M:%S", gmt_time);
	gmt_time = gmtime(&mod_time);
	strftime(mtime, 20, "%m/%d/%y %H:%M:%S", gmt_time); 
	gmt_time = gmtime(&acc_time);
	strftime(atime, 20, "%m/%d/%y %H:%M:%S", gmt_time);

	printf("INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d",
		inode_i,
		type,
		inode.i_mode & 0xFFF,
		inode.i_uid,
		inode.i_gid,
		inode.i_links_count,
		ctime,
		mtime,
		atime,
		inode.i_size,
		inode.i_blocks);
	if(type == 'd' || type == 'f' || (type == 's' && (inode.i_size > 60))){
		for(int w = 0; w < 15; w++){
			printf(",%d",inode.i_block[w]);
		}
	}
	printf("\n");
	if(type == 'd'){
		for(int m = 0; m < 12; m++){
			if(inode.i_block[m] != 0){
				process_directory(inode.i_block[m], inode_i);
			}
		}
	}
	if(type != 's'){
		bool directory = false;
		if(type == 'd'){
			directory = true;
		}
		if(inode.i_block[12] != 0){
			process_indirect_block(inode_i, 1, inode.i_block[12], 12, directory);
		}
		if(inode.i_block[13] != 0){
			process_indirect_block(inode_i, 2, inode.i_block[13], 268, directory);
		}
		if(inode.i_block[14] != 0){
			process_indirect_block(inode_i, 3, inode.i_block[14], 65804, directory);
		}
	}	
}


void process_freeinodes(unsigned int i, unsigned int inode_bitmap, int inode_table){
	unsigned int bitmap_block_offset = inode_bitmap * block_size;
	int inodebit_bytes = super_block.s_inodes_per_group/8;
	char* bitmap = (char*) malloc(inodebit_bytes);
	pread(fd_img, bitmap, inodebit_bytes, bitmap_block_offset);
	int byte_i, bit_i;
	unsigned int inode_i = 1 + (i * super_block.s_inodes_per_group);
	unsigned int inode_start = inode_i;
	for(byte_i = 0; byte_i < inodebit_bytes; byte_i++){
		char bite = bitmap[byte_i];
		for(bit_i = 0; bit_i < 8; bit_i++){
			int bit = bite & 1;
			if(bit == 0){
				printf("IFREE,%d\n", inode_i);	
			}else{
				process_inode_summary(inode_i, inode_table, inode_i - inode_start);
			}
			bite >>= 1;
			inode_i++;
		}
	}
	free(bitmap); 
}

void process_groups(){
	struct ext2_group_desc group_desc;
	group_count = ceil((double) super_block.s_blocks_count / (double) super_block.s_blocks_per_group);
	//check this data structure if corrrect
	u_int32_t grpdsc_block = 0;
	if(block_size == 1024){
		grpdsc_block = 2;
	} else{
		grpdsc_block = 1;
	}
	int grpdsc_offset = block_size * grpdsc_block;
	unsigned int i;
	for(i = 0; i < group_count; i++){
		pread(fd_img, &group_desc, sizeof(group_desc), grpdsc_offset + (sizeof(group_desc)*i));
		int blocks_in_group = super_block.s_blocks_per_group;
		int inodes_in_group = super_block.s_inodes_per_group;
		if(i == group_count - 1){
			blocks_in_group = super_block.s_blocks_count - (super_block.s_blocks_per_group * (group_count - 1));
			inodes_in_group = super_block.s_inodes_count - (super_block.s_inodes_per_group * (group_count -1));
		}
		printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
			i,
			blocks_in_group,
			inodes_in_group,
			group_desc.bg_free_blocks_count,
			group_desc.bg_free_inodes_count,
			group_desc.bg_block_bitmap,
			group_desc.bg_inode_bitmap,
			group_desc.bg_inode_table
		);
		unsigned int block_bitmap = group_desc.bg_block_bitmap;
		process_freeblocks(i, block_bitmap, blocks_in_group);	//process blocks in this group			
		unsigned int inode_bitmap = group_desc.bg_inode_bitmap;
		unsigned int inode_table = group_desc.bg_inode_table;
		process_freeinodes(i, inode_bitmap, inode_table);

	}
}

int main(int argc, char* argv[]){
	if(argc != 2){
		fprintf(stderr, err);
		exit(1);
	}
	if((fd_img = open(argv[1], O_RDONLY)) == -1){
		fprintf(stderr, "There was an error opening the file");
		exit(1);
	}
	process_superblock();
	process_groups();
	close(fd_img);
	return 0;
}

