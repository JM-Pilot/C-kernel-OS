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
// for the future
//static file_t fds[MAX_FD];
//static int fd_count = 0;
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
}

int mount(struct filesystem *fs, char *path) {
	mount_t mountpoint;
	if (fs->read && fs->write && fs->mount && fs->unmount && mount_count < MAX_MOUNT) {
		mountpoint.fs = fs;
		if (strlen(path) > 255) {
			printk(2, "vfs: error: mountpoint path too large. %d > 255", strlen(path));
			return 1;
		}
		strcpy(mountpoint.path, path);
		mounts[mount_count] = mountpoint;
		fs->mountpoint = &mounts[mount_count++];
		return fs->mount(fs);
	}
	return 1;
}

file_t read(char *path, uint32_t size) {
	file_t file_obj = {0};
	file_obj.offset = 0;
	for (int i = 0; i < mount_count; i++) {
		if (mounts[i].fs->read) {
			char *filepath = path;
			filepath += strlen(mounts[i].path);
			if (*filepath != '/') filepath = strcat(root_str, filepath);
			file_obj.name = filepath;
			file_obj.size = size;
			file_t file_read = mounts[i].fs->read(&file_obj);
			if (file_read.offset == 0 && file_read.data == (void*)0 && file_read.size == 0 && file_read.name == (char*)0) continue;
			for (int j = 0; j < (int)sizeof(root_str); j++) root_str[j] = 0;
			return file_read;
		}
	}
	return file_obj;
}

int write(file_t *file_obj, void *data, int size) {
	for (int i = 0; i < mount_count; i++) {
		if (mounts[i].fs->write) {
			char *filepath = file_obj->name;
			filepath += strlen(mounts[i].path);
			if (*filepath != '/') filepath = strcat(root_str, filepath);
			int file_write = mounts[i].fs->write(file_obj, data, size);
			for (int j = 0; j < (int)sizeof(root_str); j++) root_str[j] = 0;
			return file_write;
		}
	}
	return 1;
}
