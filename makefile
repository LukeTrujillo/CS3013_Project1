all:
	gcc -o multi multi.c
	gcc -o custom custom.c
	gcc -o boring boring.c
multi: multi.c
	gcc -o multi multi.c
custom: custom.c
	gcc -o custom custom.c
boring: boring.c
	gcc -o boring boring.c
clean:
	rm multi boring custom
