all: run 

run: build
	./wordle

build: main.c
	gcc -o wordle main.c

clean: 
	rm -f wordle