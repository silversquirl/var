#include <stdio.h>

#define VAR_IMPLEMENTATION
#include "var.h"

int main() {
	char arch[512];
	int len = sizeof arch;
	char *p = arch;

	int tmp;
	tmp = var_add("foo", p, len, "Hello", 5);
	if (!tmp) return 1;
	len -= tmp;
	p += tmp;

	tmp = var_add("bar", p, len, "World", 5);
	if (!tmp) return 1;
	len -= tmp;
	p += tmp;

	len = sizeof arch - len;
	printf("Archive is %d bytes long\n", len);

	p = arch;
	while (p - arch < len) {
		char *name;
		size_t data_len;
		p = var_next(&name, &data_len, p, len - (p - arch));
		if (!p) return 2;
		printf("\nName: %s\n", name);
		printf("Data length: %d\n", data_len);
		printf("Data: %.*s\n", data_len, p);
		p += data_len;
		free(name);
	}
	return 0;
}
