#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

void usage(int argc, char **argv) {
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

//Tamanho máximo por mensagem
#define BUFSZ 500

int main(int argc, char **argv) {
	if (argc < 3) {
		usage(argc, argv);
	}

	//o sockaddr_storage é um sockaddr muito grande que, eventualmente, vira um in ou in6
	struct sockaddr_storage storage;
	//Faz o parse do que passado como argumento para ipv4 ou ipv6 se possível
	if (0 != addrparse(argv[1], argv[2], &storage)) {
		usage(argc, argv);
	}

	//Criando um socket depois de realizar o parse do endereço passado como argumento
	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	//verifica o retorno e sai caso deu erro
	if (s == -1) {
		logexit("socket");
	}

	//Converte o storage para a struct mais abstrata sockaddr
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage))) {
		logexit("connect");
	}

	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);

	printf("connected to %s\n", addrstr);

	//Inicializa uma string de tamanho BUFSZ que irá conter a mensagem a ser enviada
	char buf[BUFSZ];
	//Zera o buf
	memset(buf, 0, BUFSZ);
	
	printf("> ");
	//Lê, do teclado, uma mensagem. Já adiciona o \n no final
	setbuf(stdin, NULL);
	fgets(buf, BUFSZ-1, stdin);

	//Envia a mensagem passando o socket "s" e retorna quantos bytes foram enviados de fato
	size_t count = send(s, buf, strlen(buf)+1, 0);
	//Se enviou um número de bytes diferente do próprio tamanho da mensagem, deu erro
	if (count != strlen(buf)+1) {
		logexit("send");
	}

	//Parte de recebimento de mensagem
	memset(buf, 0, BUFSZ);
	unsigned total = 0;

	while(1) {
		//O recv pode enviar a mesma mensagem em partes
		//Dessa forma, temos que ficar recebendo até que ele envie 0 bytes
		count = recv(s, buf + total, BUFSZ - total, 0);
		if (count == 0) {
			// Connection terminated.
			break;
		}
		total += count;
	}
	close(s);

	printf("received %u bytes\n", total);
	puts(buf);

	exit(EXIT_SUCCESS);
}