build:
	gcc -o build/maze src/main.c

test:
	./build/maze 3 7 1 resources/bludiste.txt

.PHONY: build test
