#ifndef CPIO_H
#define CPIO_H
typedef struct {                                                                                                                                                                                                                                                              
	void *file;
	uint32_t size;
} cpio_inode_t;
char list_files_cpio();
cpio_inode_t read_file_cpio(char *filename);
#endif
