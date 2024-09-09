#include <stdio.h>
#include "types.h"

char* memlod(char* dest, char* src, char predicate) {
	i32 pos = 0;
	while (src[pos] != predicate) {
		dest[pos] = src[pos];
		pos++;
	}
	return dest;
}
