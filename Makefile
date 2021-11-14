all:
	gcc -Wall -c common.c
	gcc -Wall -c pokedex.c
	gcc -Wall client.c common.o pokedex.o -o client
	gcc -Wall server.c common.o pokedex.o -o server
	gcc -Wall server-mt.c common.o -lpthread -o server-mt

clean:
	rm common.o client server server-mt
