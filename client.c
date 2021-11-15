#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct Client{
    struct sockaddr_storage cstorage;
    struct sockaddr *caddr;
    socklen_t caddrlen;
    char caddrstr[BUFSZ];
    int socket;
};

void usage(int argc, char **argv) {
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

void inicia_client(struct Client* client, int argc, char **argv){
	//o sockaddr_storage é um sockaddr muito grande que, eventualmente, vira um in ou in6
	struct sockaddr_storage storage;
	//Faz o parse do que passado como argumento para ipv4 ou ipv6 se possível
	if (0 != addrparse(argv[1], argv[2], &(client->cstorage))) {
		usage(argc, argv);
	}

	//Criando um socket depois de realizar o parse do endereço passado como argumento
	client->socket = socket(client->cstorage.ss_family, SOCK_STREAM, 0);

	//verifica o retorno e sai caso deu erro
	if (client->socket == -1) {
		logexit("socket");
	}

	//Converte o storage para a struct mais abstrata sockaddr
	client->caddr = (struct sockaddr *)(&client->cstorage);

	if (0 != connect(client->socket, client->caddr, sizeof(client->cstorage))) {
		logexit("connect");
	}

	addrtostr(client->caddr, client->caddrstr, BUFSZ);

	printf("connected to %s\n", client->caddrstr);

}

void captura_mensagem_teclado(char* buf, unsigned int tamanho_buffer){
	printf("> ");
	//Lê, do teclado, uma mensagem. Já adiciona o \n no final
	setbuf(stdin, NULL);
	fgets(buf, tamanho_buffer-1, stdin);
}

void envia_mensagem_server(struct Client* cliente, char* buf, unsigned int tamanho_buffer){
	size_t bytes_recebidos_pacote = send(cliente->socket, buf, strlen(buf)+1, 0);

	//Se enviou um número de bytes diferente do próprio tamanho da mensagem, deu erro
	if (bytes_recebidos_pacote != strlen(buf)+1) {
		logexit("send");
	}
}

bool recebe_mensagem_servidor(char* buf, unsigned int tamanho_buffer, struct Client* cliente){
	memset(buf, 0, tamanho_buffer);

	bool deuErro = le_msg_socket(&cliente->socket, buf);
	
	printf("< ");
	fputs(buf,stdout);
	
	return deuErro;
}

bool conversa_com_servidor(char* buf, unsigned int tamanho_buffer, struct Client* cliente){
	bool terminouConexao = false;
	memset(buf, 0, tamanho_buffer);
	
	captura_mensagem_teclado(buf, tamanho_buffer);

	envia_mensagem_server(cliente, buf, tamanho_buffer);
	
	return recebe_mensagem_servidor(buf, tamanho_buffer, cliente);;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		usage(argc, argv);
	}

	struct Client cliente;
	inicia_client(&cliente, argc, argv);

	//Inicializa uma string de tamanho BUFSZ que irá conter a mensagem a ser enviada
	char buf[TAM_MAX_MSG];
	bool terminouConexao=false;
	while(!terminouConexao){
		terminouConexao = conversa_com_servidor(buf, TAM_MAX_MSG, &cliente);
	}
	
	close(cliente.socket);
	exit(EXIT_SUCCESS);
}