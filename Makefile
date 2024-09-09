build/bvim: src/main.c
	gcc src/main.c -o build/bvim

here: build/bvim
	mv build/bvim bvim

