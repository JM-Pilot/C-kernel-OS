#include <fs/vfs.h>
#include <stdint.h>
#include <string.h>
#include <drivers/video/vga.h>

#define MAX_FS 4
#define MAX_MOUNT 8
#define MAX_FD 256

static filesystem_t filesystems[MAX_FS];
static int fs_count = 0;
static mount_t mounts[MAX_MOUNT];
static int mount_count = 0;
static file_t fds[MAX_FD];
static int fd_count = 0;
static file_t null_file = {.offset = 0, .data = (void*)0, .size = 0, .name = (char*)0};
static char root_str[256] = "/";

char *resolve_path(char *path, char *cwd) {
	if (*path == '/') return path;
	int cwd_len = strlen(cwd);
	char *cwd_use = cwd;
	if (cwd_use[cwd_len] != '/') cwd_use = strcat(cwd_use, "/");
	path = strcat(cwd_use, path);
	return path;
}

void register_fs(filesystem_t *fs) {
	if (fs->unmount && fs->mount && fs->write && fs->read) filesystems[fs_count++] = *fs;
	mount_t mountpoint;
	mountpoint.fs = (struct filesystem*)&filesystems[fs_count-1];
	mountpoint.path[0] = '/';
	mountpoint.path[1] = 0;
	printk(4, "vfs: %s at %x", mountpoint.path, mountpoint.fs);
	mounts[mount_count++] = mountpoint;
}

file_t read(char *path, uint32_t size) {
	file_t file_obj = {0};
	file_obj.offset = 0;
	for (int i = 0; i < mount_count; i++) {
		if (mounts[i].fs->read) {
			printk(4, "vfs: prepare to read %dth mount", i+1);
			char *filepath = path;
			filepath += strlen(mounts[i].path);
			if (*filepath != '/') filepath = strcat(root_str, filepath);
			file_t file_read = mounts[i].fs->read(filepath);
			printk(4, "vfs: final file path: %s", filepath);
			if (file_read.offset == 0 && file_read.data == (void*)0 && file_read.size == 0 && file_read.name == (char*)0) continue;
			return file_read;
		}
	}
	return file_obj;
}
