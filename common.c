#include "common.h"

void logexit(const char *msg) {
    //Imprime a msg e adiciona o erro automaticamente na sua frente
    //Exemplo msg="socket", imprime: "socket + alguma coisa"
	perror(msg);
	exit(EXIT_FAILURE);
}

//converte um endereço char para ipv4 ou ipv6
int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage) {
    if (addrstr == NULL || portstr == NULL) {
        return -1;
    }

    //uint16_t == unsigned short
    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0) {
        return -1;
    }

    //htons == host to network short
    //converte short para rede
    //existe o contrário ntohs == network to host short
    port = htons(port); // host to network short

    //Vamos ver se o endereço é ipv4 ou ipv6 na marra

    struct in_addr inaddr4; // 32-bit IP address
    //tenta converter o ip para IPv4
    if (inet_pton(AF_INET, addrstr, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    //tenta converter o endereço para ipv6
    struct in6_addr inaddr6; // 128-bit IPv6 address
    if (inet_pton(AF_INET6, addrstr, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        //addr6->sin6_addr = inaddr6;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

//Printa um endereço (ipv4 ou ipv6) na tela
void addrtostr(const struct sockaddr *addr, char *str, size_t strsize) {
    int version;
    char addrstr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port; // a porta tem 16 bits mesmo

    //se for ipv4
    if (addr->sa_family == AF_INET) {
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;

        //passa da representação de rede (binário) para a textual
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr,
                       INET_ADDRSTRLEN + 1)) {
            logexit("ntop");
        }
        port = ntohs(addr4->sin_port); // network to host short

        //Se for ipv6
    } else if (addr->sa_family == AF_INET6) {
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        if (!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr,
                       INET6_ADDRSTRLEN + 1)) {
            logexit("ntop");
        }
        port = ntohs(addr6->sin6_port); // network to host short
    //Se não for nenhum dos dois
    } else {
        logexit("unknown protocol family.");
    }
    if (str) {
        snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
    }
}

//Faz o parse da porta
int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage) {
    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0) {
        return -1;
    }
    port = htons(port); // host to network short

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4")) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY; //Qualquer endereço disponível no servidor poderá receber
        addr4->sin_port = port;
        return 0;
    } else if (0 == strcmp(proto, "v6")) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any; //O equivalente do INADDR_ANY para o ipv6
        addr6->sin6_port = port;
        return 0;
    } else {
        return -1;
    }
}

bool checaJaLeuMensagem(char* buf, unsigned int total_bytes_recebido, size_t bytes_recebidos_pacote){
	for(int i=total_bytes_recebido;i<(total_bytes_recebido+bytes_recebidos_pacote);i++){
        char barra_n[2] = "\n";
        if(strcmp(&buf[i],barra_n)==0){
            return true;
        }
    }
    return false;
}

bool le_msg_socket(int *socket, char *buffer_msg){
    bool fim_msg_detectado = false;
    unsigned int total_bytes_recebido = 0;
    size_t bytes_recebidos_pacote;

    while(!fim_msg_detectado){
        bytes_recebidos_pacote = recv(*socket, buffer_msg + total_bytes_recebido, TAM_MAX_MSG - total_bytes_recebido, 0);
        if(bytes_recebidos_pacote <= 0){
            return true;
        }else{
            fim_msg_detectado = checaJaLeuMensagem(buffer_msg, total_bytes_recebido, bytes_recebidos_pacote);
            total_bytes_recebido += bytes_recebidos_pacote;
        }
    }
    return false;
}