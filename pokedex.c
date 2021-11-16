#include "pokedex.h"

struct Node* searchInPokedex(struct Pokedex *pokedex, const char *nome){
    if(pokedex->quantidadePokemons == 0){
        return NULL;
    }

    struct Node* noAtual = pokedex->head;

    while(noAtual != NULL){
        if(strcmp(noAtual->pokemon.nome, nome)==0){
            return noAtual;
        }
        noAtual = noAtual->prox;
    }

    return NULL;
}



enum ops_pokedex_enum adicionarPokemon(struct Pokedex *pokedex, const char *nome){
    if(pokedex->quantidadePokemons == pokedex->maxPokemons){
        return MAX_LIMIT;
    }

    if(searchInPokedex(pokedex, nome)==NULL){
        if(strlen(nome)>10){
            return INVALID;
        }
        struct Node* novoPokemon = malloc(sizeof(struct Node));
        strcpy(novoPokemon->pokemon.nome, nome);
        novoPokemon->prox= NULL;
        novoPokemon->before = NULL;

        if(pokedex->head == NULL){
            printf("Adicionou no HEAD\n");
            pokedex->head = novoPokemon;
            pokedex->tail = novoPokemon;
        }else{
            novoPokemon->before = pokedex->tail;
            pokedex->tail->prox = novoPokemon;
            pokedex->tail = novoPokemon;
        }
        pokedex->quantidadePokemons += 1;
        return OK;
    }else{
        return ALREADY_EXISTS;
    }
}

enum ops_pokedex_enum removerPokemon(struct Pokedex *pokedex, const char *nome){
    printf("1\n");
    if(pokedex->quantidadePokemons == 0){
        return DOESNT_EXISTS;
    }
    printf("2\n");
    struct Node* noPokemon = searchInPokedex(pokedex, nome);
    if(noPokemon == NULL){
        return DOESNT_EXISTS;
        printf("3\n");
    }else{
        printf("4\n");
        if(noPokemon->before != NULL){
            noPokemon->before->prox = noPokemon->prox;
        }else{
            pokedex->head = noPokemon->prox;
        }
        printf("5\n");
        
        if(noPokemon->prox == NULL){
            pokedex->tail = noPokemon->before;
        }
        printf("6\n");
        noPokemon->prox = NULL;
        noPokemon->before = NULL;
        printf("7\n");
        free(noPokemon);
        pokedex->quantidadePokemons -= 1;
        return OK;
    }
}

enum ops_pokedex_enum trocarPokemon(struct Pokedex *pokedex, const char *nome1, const char *nome2){
    if(pokedex->quantidadePokemons == 0){
        return DOESNT_EXISTS;
    }

    struct Node* noPokemon1 = searchInPokedex(pokedex, nome1);
    if(noPokemon1 != NULL){
        struct Node* noPokemon2 = searchInPokedex(pokedex, nome2);
        if(noPokemon2 == NULL){
            strcpy(noPokemon1->pokemon.nome, nome2);
            return OK;
        }else{
            return ALREADY_EXISTS;
        }
    }else{
        return DOESNT_EXISTS;
    }
}

char* listarPokemons(struct Pokedex *pokedex){
    printf("1\n");
    if(pokedex->quantidadePokemons == 0){
        return "none";
    }
    printf("2\n");

    char* nomesPokemons;
    struct Node* noAtual = pokedex->head;
    printf("3\n");
    while(noAtual != NULL){
        if(strlen(nomesPokemons)!=0){
            strcat(nomesPokemons, " ");
        }
        strcat(nomesPokemons, noAtual->pokemon.nome);
        noAtual = noAtual->prox;
        printf("4\n");
    }
    printf("5\n");
    return nomesPokemons;
}

void limparPokedex(struct Pokedex *pokedex){
    if(pokedex->quantidadePokemons != 0){
        struct Node* noAtual = pokedex->head;
        while (noAtual != NULL) {
            struct Node *proxNo = noAtual->prox;
            free(noAtual);
            noAtual = proxNo;
        }
    }
    free(pokedex);
}