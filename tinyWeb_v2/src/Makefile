all: server

clean:
	rm http.o main.o server 
server:main.o http.o
	gcc main.o http.o -o server
main.o:main.c
	gcc -g -O3 -c main.c -o main.o 
http.o:http.c
	gcc -g -O3 -c http.c -o http.o
