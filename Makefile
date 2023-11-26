build:
	gcc -std=c11 -Wall -Wextra -Werror -o build/maze src/main.c

test:
	./build/maze --rpath 6 1 resources/bludiste.txt

.PHONY: build test
