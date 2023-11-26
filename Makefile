build:
	gcc -std=c11 -Wall -Wextra -Werror -o build/maze src/main.c

test:
	./build/maze 3 7 1 resources/bludiste.txt

.PHONY: build test
