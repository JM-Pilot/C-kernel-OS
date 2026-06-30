#include <drivers/video/vga.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
	char c_magic[6];
	char c_ino[8];
	char c_mode[8];
	char c_uid[8];
	char c_gid[8];
	char c_nlink[8];
	char c_mtime[8];
	char c_filesize[8];
	char c_devmajor[8];
	char c_devminor[8];
	char c_rdevmajor[8];
	char c_rdevminor[8];
	char c_namesize[8];
	char c_check[8];
} cpio_head_t;

uint32_t atoh(const char *s, int len) {
	uint32_t n = 0;
	while (len--) {
		n <<= 4;
		if (*s >= '0' && *s <= '9') {
			n |= *s - '0';
		} else if (*s >= 'A' && *s <= 'F') {
			n |= *s - 'A' + 10;
		} else if (*s >= 'a' && *s <= 'f') {
			n |= *s - 'a' + 10;
		}
		s++;
	}
	return n;
}

static size_t __align_4b(size_t x) { return (x+3) & ~3; }

extern uint8_t _binary_bin_initrd_cpio_start[];
char list_files_cpio() {
	uint8_t *archive = _binary_bin_initrd_cpio_start;
	for(;;) {
		cpio_head_t *header = (cpio_head_t*)archive;
		if (memcmp(header->c_magic, "070701", 6) != 0) return 1; // invalid magic
		int namesize = atoh(header->c_namesize, 8);
		int filesize = atoh(header->c_filesize, 8);
		printk(4, "cpio: namesize %d, filesize %d", namesize, filesize);
		archive += sizeof(cpio_head_t);
		uint8_t *name = archive;
		if (!strcmp((const char *)name, "TRAILER!!!")) break; // TRAILER!!! is a file in a cpio archive that determines the
			// end-of-archive because __binary_bin_initrd_cpio_end is not always present
		int header_skip = __align_4b(namesize+sizeof(cpio_head_t));
		archive -= sizeof(cpio_head_t);
		archive += header_skip;
		printk(4, "cpio: first 4B in file %s at offset %x: %x", name, archive, reverse(*(uint32_t*)archive));
		archive += __align_4b(filesize) + header_skip;
	}
	return 0;
}
