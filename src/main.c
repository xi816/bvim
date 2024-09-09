#include <stdio.h>
#include "lib/types.h"
#include "lib/memory.h"

#define READSZ 65536 // 64KB file limit
#define true 1
#define false 0

// Register names
#define SP      0x10
#define BP      0x11
#define IP      0x12
#define ZFLAG   0x13
#define OFFLAG  0x14
#define RFLAG   0x15
#define DS      0x16
#define CS      0x17

i32 main(i32 argc, char** argv) {
	if (argc > 3) {
		fprintf(stderr, "ERROR: expected only 1 argument\n");
		fprintf(stderr, "SYNTAX: bvim <file>");
		return 1;
	}

	FILE* srcfile = fopen(argv[1], "rb");
	char src[READSZ];

	char memory[4096];
	i32 regs[0x30];

	fread(src, 1, READSZ, srcfile); // Load the file
	fclose(srcfile);
	memlod(memory, src, 0x7F); // Move to memory until 0x7F

	while (true) {
		switch (memory[regs[IP]]) {
			case 0x00:
				printf("NOP\n");
				break;
			case 0x01:
				regs[memory[regs[IP]+1]] += regs[memory[regs[IP]+2]];
				regs[IP] += 2;
				break;
			case 0x02:
				regs[memory[regs[IP]+1]] -= regs[memory[regs[IP]+2]];
				regs[IP] += 2;
				break;
			case 0x03:
				regs[memory[regs[IP]+1]] *= regs[memory[regs[IP]+2]];
				regs[IP] += 2;
				break;
			case 0x04:
				regs[memory[regs[IP]+3]] = regs[memory[regs[IP]+1]] *= regs[memory[regs[IP]+2]];
				regs[IP] += 3;
				break;
			case 0x05:
				regs[memory[regs[IP]+3]] = regs[memory[regs[IP]+1]] /= regs[memory[regs[IP]+2]];
				regs[IP] += 3;
				break;
			case 0x06:
				regs[memory[regs[IP]+1]] += regs[memory[regs[IP]+2]];
				regs[IP] += 2;
				break;
			case 0x07:
				regs[memory[regs[IP]+1]] = (memory[regs[IP]+2] << 8) + (memory[regs[IP]+3]);
				regs[IP] += 3;
				break;
			case 0x20:
				if (regs[0x07] == 0) {
					return regs[0x08];
				}
				else {
					fprintf(stderr, "Unkown system call %02x", regs[7]);
				}
			case 0x7F:
				printf("EOF\n");
				goto progEOF;
		}
		regs[IP]++;
	}
	progEOF:
		printf("EOF\n");

	return 0;
}
