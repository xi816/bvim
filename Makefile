build/bvim: src/main.c always
	gcc src/main.c -o build/bvim

always:
	mkdir -p build/

here: build/bvim
	mv build/bvim bvim

clean: build/
	rm -rf build/
