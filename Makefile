build:
	gcc -std=c11 -Wall -Wextra -Werror -o build/maze src/main.c

test: test.lpath test.rpath test.edge

test.lpath:
	./build/maze --lpath 6 1 resources/bludiste.txt

test.rpath:
	./build/maze --rpath 6 1 resources/bludiste.txt

test.edge:
	./build/maze --rpath 6 7 resources/bludiste.txt

.PHONY: build test
