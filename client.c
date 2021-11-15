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
	int my_socket;
	my_socket = socket(storage.ss_family, SOCK_STREAM, 0);

	//verifica o retorno e sai caso deu erro
	if (my_socket == -1) {
		logexit("socket");
	}

	//Converte o storage para a struct mais abstrata sockaddr
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(my_socket, addr, sizeof(storage))) {
		logexit("connect");
	}

	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);

	printf("connected to %s\n", addrstr);

	//Inicializa uma string de tamanho BUFSZ que irá conter a mensagem a ser enviada
	char buf[TAM_MAX_MSG];
	short int terminouConexao=0;
	while(1){
		//Zera o buf
		memset(buf, 0, TAM_MAX_MSG);
		
		printf("> ");
		//Lê, do teclado, uma mensagem. Já adiciona o \n no final
		setbuf(stdin, NULL);
		fgets(buf, TAM_MAX_MSG-1, stdin);
		printf("Tamanho mensagem: %d\n",strlen(buf));

		for(int i=0;i<TAM_MAX_MSG;i++){
			//printf("Testando no Cliente %d \n",i);
			char barra_n[2] = "\n";
			char fim_string[2] = "\0";
			/*if(strcmp(&buf[i],fim_string)==0){
				printf("Possui \\0 em %d\n",i);
			}*/
			if(strcmp(&buf[i],barra_n)==0){
				printf("Possui \\n em %d\n",i);
				break;
			}
		}

		//Envia a mensagem passando o socket "s" e retorna quantos bytes foram enviados de fato
		//printf("Aqui 1 CL\n");
		size_t bytes_recebidos_pacote = send(my_socket, buf, strlen(buf)+1, 0);
		//Se enviou um número de bytes diferente do próprio tamanho da mensagem, deu erro
		if (bytes_recebidos_pacote != strlen(buf)+1) {
			logexit("send");
		}
		//printf("Aqui 2 CL\n");
		//Parte de recebimento de mensagem
		memset(buf, 0, TAM_MAX_MSG);
		unsigned total_bytes_recebido = 0;
		//printf("Aqui 3 CL\n");

		while(1) {
			printf("Esperando resposta\n");
			//O recv pode enviar a mesma mensagem em partes
			//Dessa forma, temos que ficar recebendo até que ele envie 0 bytes
			bytes_recebidos_pacote = recv(my_socket, buf + total_bytes_recebido, TAM_MAX_MSG - total_bytes_recebido, 0);
			short int jaLeu = 0;
            for(int i=total_bytes_recebido;i<(TAM_MAX_MSG-total_bytes_recebido);i++){
                char barra_n[2] = "\n";
                char fim_string[2] = "\0";
			    /*if(strcmp(&buf[i],fim_string)==0){
			    	printf("Possui \\0 em %d\n",i);
		    	}*/
                if(strcmp(&buf[i],barra_n)==0){
                    jaLeu = 1;
                   	//printf("Possui \\n em %d\n",i);
                    break;
                }
            }
			if (bytes_recebidos_pacote == 0) {
				// Connection terminated.
				terminouConexao=1;
				break;
			}
			
			total_bytes_recebido += bytes_recebidos_pacote;
			if(jaLeu==1){
				break;
			}
		}

		printf("received %u bytes\n", total_bytes_recebido);
		puts(buf);
		if(terminouConexao == 1){
			break;
		}
	}
	
	close(my_socket);
	exit(EXIT_SUCCESS);
}