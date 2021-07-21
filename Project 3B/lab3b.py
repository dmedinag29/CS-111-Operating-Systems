#!/usr/bin/python

# Name: Daniel Medina Garate
# Email: dmedinag@g.ucla.edu
# ID: 204971333

import sys
import csv

def process_data(file_data):
	total_blocks = 0
	total_inodes = 0
	b_free = set()
	i_free = set()
	block_dict = {}
	inode_dict = {}
	inodecount_dict = {}
	dirent_dict = {}
	for i in file_data: #goes through rows in data
		if i[0] == 'SUPERBLOCK':
			total_blocks = int(i[1])
			total_inodes = int(i[2])
			block_size = int(i[3])
			inode_size = int(i[4])
			first_inode = int(i[7])
		elif i[0] == 'GROUP':
			bitmap_block_i = int(i[6])
			bitmap_inode_i = int(i[7])
			inode_block_i = int(i[8])
	datablock_i = inode_block_i + ((inode_size * total_inodes) / block_size)
	for i in file_data:
		if i[0] == 'BFREE':
			b_free.add(int(i[1]))
		
		elif i[0] == 'IFREE':
			i_free.add(int(i[1]))
		
		elif i[0] == 'INODE':
			inode_i = int(i[1])
			if inode_i not in inode_dict:
				inode_dict[inode_i] = int(i[6])
			for col in range(12, 27): #scans block adresses
				block_i = int(i[col])
				if block_i != 0:
					if col == 24:
						offset = 12
						indirect = "INDIRECT "
						intindir = 1
					elif col == 25:
						offset = 268
						indirect = "DOUBLE INDIRECT "
						intindir = 2
					elif col == 26:
						offset = 65804
						indirect = "TRIPLE INDIRECT "
						intindir = 3
					else:	
						offset = 0
						indirect = ""
						intindir = 0
					if block_i > (total_blocks - 1) or block_i < 0:
						print('INVALID '+indirect+'BLOCK '+str(block_i)+' IN INODE '+str(i[1])+' AT OFFSET '+str(offset))	
						exit_code = 1
					elif block_i < datablock_i:
						print('RESERVED '+indirect+'BLOCK '+str(block_i)+' IN INODE '+str(i[1])+' AT OFFSET '+str(offset))
						exit_code = 1
					elif block_i not in block_dict:
						block_dict[block_i] = [[int(i[1]), intindir, offset]]
					else:
						block_dict[block_i].append([int(i[1]), intindir, offset])
		elif i[0] == 'DIRENT':
			inode_i = int(i[3])
			direntinode_i = int(i[1])
			name = i[6]
			if (name != '.') and (name != '..'):
				if inode_i not in dirent_dict:
					dirent_dict[inode_i] = direntinode_i
			if inode_i not in inodecount_dict:
				inodecount_dict[inode_i] = 1
			else:
				inodecount_dict[inode_i] = inodecount_dict[inode_i] + 1
		elif i[0] == 'INDIRECT':
			if i[2] == '1':
				indirect = "INDIRECT "
			elif i[2] == '2':
				indirect = "DOUBLE INDIRECT "
			elif i[2] == '3':
				indirect = "TRIPLE INDIRECT "
			block_i = int(i[5])
			if block_i > (total_blocks -1) or block_i < 0:
				print('INVALID '+indirect+'BLOCK '+str(block_i)+' IN INODE '+str(i[1])+' AT OFFSET '+str(i[3]))
				exit_code = 1
			elif block_i < datablock_i:
				print('RESERVED '+indirect+'BLOCK '+str(block_i)+' IN INODE '+str(i[1])+' AT OFFSET '+str(i[3]))
				exit_code = 1
			elif block_i not in block_dict:
				block_dict[block_i] = [[int(i[1]), int(i[2]), int(i[3])]]
			else:
				block_dict[block_i].append([int(i[1]), int(i[2]), int(i[3])])

	for i in range(datablock_i, total_blocks):
		if i not in b_free and i not in block_dict:
			print('UNREFERENCED BLOCK '+str(i))
			exit_code = 1
		elif i in b_free and i in block_dict:
			print('ALLOCATED BLOCK '+str(i)+' ON FREELIST')
			exit_code = 1
		#checks for duplicates
		if i in block_dict:
			if len(block_dict[i]) > 1:
				for entry in block_dict[i]:
					if entry[1] == 1:
                               			indirect = "INDIRECT "
					elif entry[1] == 2:
                                		indirect = "DOUBLE INDIRECT "
					elif entry[1] == 3:
                               			indirect = "TRIPLE INDIRECT "
					elif entry[1] == 0:
						indirect = ""
					print('DUPLICATE '+indirect+'BLOCK '+str(i)+' IN INODE '+str(entry[0])+' AT OFFSET '+str(entry[2]))
					exit_code = 1
	if 2 in inode_dict and 2 in i_free:
		print('ALLOCATED INODE 2 ON FREELIST')
		exit_code = 1
	if 2 not in inode_dict and 2 not in i_free:
		print('UNALLOCATED INODE 2 NOT ON FREELIST')
		exit_code = 1
	if 2 in inode_dict:
                        if 2 not in inodecount_dict:
                                inodecount_dict[2] = 0
                        if inode_dict[2] != inodecount_dict[2]:
                                print('INODE 2 HAS '+str(inodecount_dict[2])+' LINKS BUT LINKCOUNT IS '+str(inode_dict[2]))
				exit_code = 1
	
	for i in range(first_inode, total_inodes + 1):					
		if i in inode_dict and i in i_free:
			print('ALLOCATED INODE '+str(i)+' ON FREE LIST')
			exit_code = 1
		if i not in inode_dict and i not in i_free:
			print('UNALLOCATED INODE '+str(i)+' NOT ON FREELIST')
			exit_code = 1
		if i in inode_dict:
			if i not in inodecount_dict:
				inodecount_dict[i] = 0
			if inode_dict[i] != inodecount_dict[i]:
				print('INODE '+str(i)+' HAS '+str(inodecount_dict[i])+' LINKS BUT LINKCOUNT IS '+str(inode_dict[i])) 
				exit_code = 1
	for i in file_data:		
		if i[0] == 'DIRENT':
			inode_i = int(i[3])
			direntinode_i = int(i[1])
			name = i[6] 
			if (inode_i < 1) or (inode_i > total_inodes):
				print('DIRECTORY INODE '+str(direntinode_i)+' NAME '+str(name)+' INVALID INODE '+str(inode_i))
				exit_code = 1
			elif inode_i not in inode_dict:
				print('DIRECTORY INODE '+str(direntinode_i)+' NAME '+str(name)+' UNALLOCATED INODE '+str(inode_i))
				exit_code = 1
			if name == "'.'":
				if inode_i != direntinode_i:
					print('DIRECTORY INODE '+str(direntinode_i)+' NAME '+str(name)+' LINK TO INODE '+str(inode_i)+' SHOULD BE '+str(direntinode_i))
					exit_code = 1
			if name == "'..'":
				if dirent_dict[direntinode_i] != inode_i:
					print('DIRECTORY INODE '+str(direntinode_i)+' NAME '+str(name)+' LINK TO INODE '+str(inode_i)+' SHOULD BE '+str(direntinode_i))
					exit_code = 1

def main():
	exit_code = 0
	if len(sys.argv) != 2:
		sys.stderr.write('Incorrect number of arguments \n')
		sys.exit(1)
	file_data = [] 
	try:
		with open(sys.argv[1], 'r') as file_sum:
			entries = csv.reader(file_sum)
			for i in entries:
				file_data.append(i)
	except:
		sys.stderr.write('Error opening file \n')
		sys.exit(1)	

	process_data(file_data)
	if(exit_code):
		sys.exit(2)
	else:
		sys.exit(0)

if __name__ == "__main__":
	main()	
