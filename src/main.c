#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// 4KB of memory allocated as default
i32 MEMSIZE = 4096;

i32 main(i32 argc, char** argv) {
	char* filename = argv[1];
	for (char i = 2; i < argc; i++) {
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-?") || !strcmp(argv[i], "--help")) {
			printf("SYNTAX: bvim <file> [options]\n");
			printf("  Options:\n");
			printf("    -MB 4096         Set memory size (bytes)\n");
			printf("    -MK 8            Set memory size (kilobytes)\n");
			printf("    -MM 64           Set memory size (megabytes)\n");
			printf("    -h, -?, --help   Show help\n");
			return 2;
		}
		else if (!strcmp(argv[i], "-MB")) {
			MEMSIZE = atoi(argv[i+1]);
			i += 2;
		}
		else if (!strcmp(argv[i], "-MK")) {
			MEMSIZE = atoi(argv[i+1])*1024;
			i += 2;
		}
		else if (!strcmp(argv[i], "-MM")) {
			MEMSIZE = atoi(argv[i+1])*1048576;
			i += 2;
		}
		else {
			printf("%s\n", argv[i]);
		}
	}

	FILE* srcfile = fopen(filename, "rb");
	char src[READSZ];

	char memory[4096];
	char modal = 0x00;
	i32 regs[0x30];

	fread(src, 1, READSZ, srcfile); // Load the file
	fclose(srcfile);
	memlod(memory, src, 0x7F); // Move to memory until 0x7F

	while (true) {
		if (regs[IP] >= MEMSIZE) {
			fprintf(stderr, "Memory end\n");
			return 4;
		}
		if (modal == 0x00) { // If executing code
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
				case 0x08:
					regs[memory[regs[IP]+1]] = (memory[regs[IP]+2] << 24) + (memory[regs[IP]+3] << 16) + (memory[regs[IP]+4] << 8) + (memory[regs[IP]+5]);
					regs[IP] += 5;
					break;
				case 0x20:
					switch (regs[0x07]) {
						case 0x00:
							return regs[0x08];
							break;
						case 0x01:
							if (regs[0x08] == 1) {
								putc(regs[0x09], stdout);
							}
							else if (regs[0x08] == 2) {
								putc(regs[0x09], stderr);
							}
							break;
					default:
						fprintf(stderr, "Unkown system call %02x", regs[0x07]);
					}
					break;
				case 0x7F:
					goto progEOF;
			}
		}
		regs[IP]++;
	}
	progEOF:
		printf("\nFFFh\n");

	return 0;
}
