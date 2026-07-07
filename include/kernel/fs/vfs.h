#include <stdint.h>
#ifndef VFS_H
#define VFS_H
char *resolve_path(char *path, char *cwd);
typedef struct mount {
	char path[256];
	struct filesystem *fs;
} mount_t;
typedef struct file {
	uint32_t offset; // reserved for seeking
	void *data;
	uint32_t size;
	char *name;
} file_t;
typedef struct filesystem {
	file_t(*read)(char*);
	int(*write)(file_t*, void*, int);
	int(*mount)(struct filesystem);
	int(*unmount)(struct filesystem);
	struct mount *mountpoint;
} filesystem_t;
void register_fs(filesystem_t *fs);
file_t read(char *path, uint32_t size);
#endif
