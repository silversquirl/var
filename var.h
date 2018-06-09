#ifndef __VAR_H
#define __VAR_H

#include <stdint.h>

/* Add a "file" to the archive
 * dest_len should be at least strlen(name) + src_len + 9
 * If dest_len is too small, 0 will be returned, otherwise the length
 * of the added data is returned
 */
size_t var_add(char *name, char *dest, size_t dest_len, char *src, size_t src_len);

/* Retrieves the metadata for the next "file" in an archive
 *
 * name is a double pointer to a buffer which will be allocated to store the name
 * data_len is a pointer to an int where the length of the "file" will be stored
 * src is a pointer to an archive buffer of length src_len
 *
 * If the return value is non-NULL, you must free *name after use
 *
 * The return value is either:
 *  a pointer to the start of the "file" data
 *  NULL (if an error occurs)
 */
char *var_next(char **name, size_t *data_len, char *src, uint64_t src_len);

#ifdef VAR_IMPLEMENTATION

#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>

size_t var_add(char *name, char *dest, size_t dest_len, char *src, uint64_t src_len) {
	size_t name_len = strlen(name) + 1; // Length including NUL terminator
	size_t len = name_len + 8 + src_len; // 8 bytes for length of file
	if (dest_len < len) return 0;

	memcpy(dest, name, name_len);
	dest += name_len;

	*dest++ = src_len >> 8*7 & 0xff;
	*dest++ = src_len >> 8*6 & 0xff;
	*dest++ = src_len >> 8*5 & 0xff;
	*dest++ = src_len >> 8*4 & 0xff;
	*dest++ = src_len >> 8*3 & 0xff;
	*dest++ = src_len >> 8*2 & 0xff;
	*dest++ = src_len >> 8*1 & 0xff;
	*dest++ = src_len >> 8*0 & 0xff;

	memcpy(dest, src, src_len);
	dest += src_len;

	return len;
}

char *var_next(char **name, size_t *data_len, char *src, uint64_t src_len) {
	size_t name_len = strnlen(src, src_len) + 1;
	if (src_len < name_len + 8) return NULL;
	*name = malloc(name_len);
	if (!*name) return NULL;
	memcpy(*name, src, name_len);
	src += name_len;

	*data_len = 0;
	*data_len |= (uint64_t)*src++ << 8*7;
	*data_len |= (uint64_t)*src++ << 8*6;
	*data_len |= (uint64_t)*src++ << 8*5;
	*data_len |= (uint64_t)*src++ << 8*4;
	*data_len |= (uint64_t)*src++ << 8*3;
	*data_len |= (uint64_t)*src++ << 8*2;
	*data_len |= (uint64_t)*src++ << 8*1;
	*data_len |= (uint64_t)*src++ << 8*0;

	return src;
}

#endif

#endif
