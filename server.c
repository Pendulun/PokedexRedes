#include "common.h"
#include "pokedex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/types.h>

#define MAX_CONN_QUEUE 10

struct Server{
    struct sockaddr *addr;
    struct sockaddr_storage storage;
    int my_socket;
    char addrstr[BUFSZ];
};

struct Client{
    struct sockaddr_storage cstorage;
    struct sockaddr *caddr;
    socklen_t caddrlen;
    char caddrstr[BUFSZ];
    int socket;
};

enum ops_server_enum{DISCONNECT_CLIENT, DIE, ADD, REMOVE, LIST, EXCHANGE};

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

void iniciar_servidor(struct Server *my_server, int argc, char **argv){
    if (0 != server_sockaddr_init(argv[1], argv[2], &(my_server->storage))) {
        usage(argc, argv);
    }

    my_server->my_socket = socket(my_server->storage.ss_family, SOCK_STREAM, 0);
    if (my_server->my_socket == -1) {
        logexit("socket");
    }

    //Reutiliza uma porta caso ela já tenha sido utilizada
    int enable = 1;
    if (0 != setsockopt(my_server->my_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    //Liga o server a uma porta
    my_server->addr = (struct sockaddr *)(&my_server->storage);
    if (0 != bind(my_server->my_socket, my_server->addr, sizeof(my_server->storage))) {
        logexit("bind");
    }

    //O MAX_CONN_QUEUE é a quantidade de conexões que podem estar pendentes para tratamento
    if (0 != listen(my_server->my_socket, MAX_CONN_QUEUE)) {
        logexit("listen");
    }

    //Mensagem para indicar que estamos esperando
    addrtostr(my_server->addr, my_server->addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", my_server->addrstr);

    return;
}

void iniciar_client(struct Client *my_client, struct Server *my_server){
    my_client->caddr = (struct sockaddr *)(&(my_client->cstorage));
    my_client->caddrlen = sizeof(my_client->cstorage);

    //Retorna um novo socket
    my_client->socket = accept(my_server->my_socket, my_client->caddr, &(my_client->caddrlen));

    if (my_client->socket == -1) {
        logexit("accept");
    }

    //Printa mensagem de conexão
    addrtostr(my_client->caddr, my_client->caddrstr, BUFSZ);
    printf("[log] connection from %s\n", my_client->caddrstr);
}

bool get_msg_client(struct Client *my_client, char* buffer_msg, unsigned int tamanho_buffer){
    bool deuErro = le_msg_socket(&my_client->socket, buffer_msg);
    //printf(deuErro);
    printf("< ");
    fputs(buffer_msg, stdout);
    return deuErro;
}

void send_msg_client(struct Client *my_client, char* buffer_msg, unsigned int tamanho_buffer){
    //Envia uma resposta
    size_t bytes_recebidos_pacote;
    bytes_recebidos_pacote = send(my_client->socket, buffer_msg, strlen(buffer_msg) + 1, 0);

    //Se enviou um número de bytes diferente do próprio tamanho da mensagem, deu erro
	if (bytes_recebidos_pacote != strlen(buffer_msg)+1) {
		logexit("send");
	}
}

enum ops_server_enum getOpMensagem(const char* buf_msg){
    if(strcmp(buf_msg,"kill\0")==0){
        return DIE;
    }else if(strcmp(buf_msg,"add\0")==0){
        return ADD;
    }else if(strcmp(buf_msg,"remove\0")==0){
        return REMOVE;
    }else if(strcmp(buf_msg,"list\0")==0){
        return LIST;
    }else if(strcmp(buf_msg,"exchange\0")==0){
        return EXCHANGE;
    }else{
        return DISCONNECT_CLIENT;
    }
}

void addMensagemComDado(char* msgOriginal, const char* dado, const char* msgAdd){
    strcat(msgOriginal, dado);
    strcat(msgOriginal, msgAdd);
}

void addMensagem(char* msgOriginal, const char* msgAdd){
    strcat(msgOriginal, msgAdd);
}

void realizarOpPokedex(enum ops_server_enum operacao, struct Pokedex* minhaPokedex, char* msgParaCliente){
    enum ops_pokedex_enum resultAcao;
    const char delimiter[]  = " \t\r\n\v\f";

    if(strlen(msgParaCliente) > 0){
        strcat(msgParaCliente," ");
    }

    if(operacao == ADD){
        char* dados[NUM_MAX_POKEMON_ADDED];
        unsigned int numPokemonsAdded = 0;
        //Assumindo que sempre terá pelo menos um pokemon a ser add e que só existe uma operação a ser realizada por vez
        char* token = strtok(NULL, delimiter);
        while (token != NULL){
            if(numPokemonsAdded == NUM_MAX_POKEMON_ADDED){
                //Ignora outros pokemons passados além do NUM_MAX_POKEMON_ADDED
                break;
            }else{    
                dados[numPokemonsAdded] = token;
                //strcpy(dados[numPokemonsAdded], token);
                token = strtok(NULL, delimiter);
                numPokemonsAdded++;
            }
        }
        
        enum ops_pokedex_enum results[numPokemonsAdded];
        adicionarPokemons(minhaPokedex, dados, numPokemonsAdded, results);

        for(unsigned int idxResult = 0; idxResult<numPokemonsAdded; idxResult++){
            if(strlen(msgParaCliente) > 0){
                strcat(msgParaCliente," ");
            }

            if(results[idxResult] == OK){
                addMensagemComDado(msgParaCliente, dados[idxResult], " added");
            }else if(results[idxResult] == ALREADY_EXISTS){
                addMensagemComDado(msgParaCliente, dados[idxResult], " already exists");
            }else if(results[idxResult] == MAX_LIMIT){
                addMensagem(msgParaCliente, "limit exceeded");
            }else if(results[idxResult] == INVALID){
                addMensagem(msgParaCliente, "invalid message");
            }   
        }
    }else if(operacao == REMOVE){
        char *dado = strtok(NULL, delimiter);

        resultAcao = removerPokemon(minhaPokedex, dado);

        if(resultAcao == OK){
            addMensagemComDado(msgParaCliente, dado, " removed");
        }else if(resultAcao == DOESNT_EXISTS){
            addMensagemComDado(msgParaCliente, dado, " does not exist");
        }else if(resultAcao == INVALID){
            addMensagem(msgParaCliente, "invalid message");
        }

    }else if(operacao == LIST){
        char nomesPokemons[TAM_MAX_MSG] = "";
        listarPokemons(minhaPokedex, nomesPokemons);

        if(strlen(nomesPokemons)==0){
            addMensagem(msgParaCliente, "none");
        }else{
            addMensagem(msgParaCliente, nomesPokemons);
        }

        resultAcao = OK;
    }else if(operacao == EXCHANGE){
        char *dado1 = strtok(NULL, delimiter);
        char *dado2 = strtok(NULL, delimiter);
        resultAcao = trocarPokemon(minhaPokedex, dado1, dado2);

        if(resultAcao == OK){
            addMensagemComDado(msgParaCliente, dado1, " exchanged");
        }else if(resultAcao == DOESNT_EXISTS){
             addMensagemComDado(msgParaCliente, dado1, " does not exist");
        }else if(resultAcao == ALREADY_EXISTS){
            addMensagemComDado(msgParaCliente, dado2, " already exists");
        }else if(resultAcao == INVALID){
            addMensagem(msgParaCliente, "invalid message");
        }
    }
}

bool conversa_client_server(struct Client *my_client, struct Pokedex* minhaPokedex){
    bool matar_server = false;
    bool cliente_desconectou = false;
    char buffer_msg[TAM_MAX_MSG];
    while(!cliente_desconectou && !matar_server){
        memset(buffer_msg, 0, TAM_MAX_MSG);
        cliente_desconectou = get_msg_client(my_client, buffer_msg, TAM_MAX_MSG);

        if(cliente_desconectou){
            printf("Client se desconectou repentinamente!\n");
        }else{
            const char delimiter[] = " \t\r\n\v\f";
            char *token = strtok(buffer_msg, delimiter);

            char msgParaCliente[TAM_MAX_MSG];
            memset(msgParaCliente, 0, TAM_MAX_MSG);

            while (token != NULL){
                enum ops_server_enum operacao = getOpMensagem(token);

                if(operacao == DISCONNECT_CLIENT){
                    cliente_desconectou=true;
                    close(my_client->socket);
                    break;
                }else if(operacao == DIE){
                    matar_server = true;
                    close(my_client->socket);
                    break;
                }else{
                    realizarOpPokedex(operacao, minhaPokedex, msgParaCliente); 
                    break;              
                }

                token = strtok(NULL, delimiter);
            }
            strcat(msgParaCliente,"\n");

            if(!matar_server && !cliente_desconectou){
                //Enviar mensagem de acordo com a resposta da operação pokedex
                send_msg_client(my_client, msgParaCliente, strlen(msgParaCliente));
            }
        }
    }

    return matar_server;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    }

    struct Server my_server;
    iniciar_servidor(&my_server, argc, argv);

    struct Pokedex* minhaPokedex = malloc(sizeof(struct Pokedex));
    minhaPokedex->head = NULL;
    minhaPokedex->tail = NULL;
    minhaPokedex->maxPokemons = TAM_MAX_POKEDEX;
    minhaPokedex->quantidadePokemons = 0;

    bool matar_server = false;
    while (!matar_server) {
        printf("Esperando conexao\n");

        struct Client my_client;
        iniciar_client(&my_client, &my_server);

        matar_server = conversa_client_server(&my_client, minhaPokedex);
    }
    limparPokedex(minhaPokedex);
    exit(EXIT_SUCCESS);
}