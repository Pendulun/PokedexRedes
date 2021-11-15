#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define BUFSZ 1024
#define TAM_MAX_MSG 500

void logexit(const char *msg);

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage);

bool checaJaLeuMensagem(char* buf, unsigned int total_bytes_recebido, size_t bytes_recebidos_pacote);

bool le_msg_socket(int *socket, char *buffer_msg);

#endif // COMMON_H