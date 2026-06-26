#ifndef ID_H
#define ID_H
typedef struct {
	char *bootloader;
	int major;
	int minor;
	int patch;
	char *additional_user;
	char *additional_makefile;
} kernel_id_t;
extern kernel_id_t *kernel_id;
#endif
