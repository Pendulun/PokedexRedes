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



void adicionarPokemons(struct Pokedex *pokedex, const char* nomes[], const unsigned int numNomes, enum ops_pokedex_enum *results){
    for(unsigned int idxPokemonAdd=0; idxPokemonAdd<numNomes; idxPokemonAdd++){
        //Se atingiu o limite
        if(pokedex->quantidadePokemons == pokedex->maxPokemons){
            results[idxPokemonAdd] = MAX_LIMIT;
        }else{
            //Se já não existe na pokedex
            if(searchInPokedex(pokedex, nomes[idxPokemonAdd])==NULL){
                //Se o nome passa do limite
                printf("Nome do pokemon a ser add: %s\n", nomes[idxPokemonAdd]);
                if(strlen(nomes[idxPokemonAdd])>TAM_MAX_NOME_POKEMON){
                    results[idxPokemonAdd] = INVALID;
                }else{
                    struct Node* novoPokemon = malloc(sizeof(struct Node));
                    strcpy(novoPokemon->pokemon.nome, nomes[idxPokemonAdd]);
                    novoPokemon->prox = NULL;
                    novoPokemon->before = NULL;

                    if(pokedex->head == NULL){
                        pokedex->head = novoPokemon;
                        pokedex->tail = novoPokemon;
                    }else{
                        novoPokemon->before = pokedex->tail;
                        pokedex->tail->prox = novoPokemon;
                        pokedex->tail = novoPokemon;
                    }
                    pokedex->quantidadePokemons += 1;
                    results[idxPokemonAdd] = OK;
                }
            }else{
                results[idxPokemonAdd] = ALREADY_EXISTS;
            }
        }
    }
}

enum ops_pokedex_enum removerPokemon(struct Pokedex *pokedex, const char *nome){
    if(pokedex->quantidadePokemons == 0){
        return DOESNT_EXISTS;
    }
    struct Node* noPokemon = searchInPokedex(pokedex, nome);
    if(noPokemon == NULL){
        return DOESNT_EXISTS;
    }else{
        if(noPokemon->before != NULL){
            noPokemon->before->prox = noPokemon->prox;
        }else{
            pokedex->head = noPokemon->prox;
        }
        
        if(noPokemon->prox == NULL){
            pokedex->tail = noPokemon->before;
        }
        noPokemon->prox = NULL;
        noPokemon->before = NULL;
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
        if(strlen(nome2) > 10){
            return INVALID;
        }

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

char* listarPokemons(struct Pokedex *pokedex, char* nomesPokemons){
    if(pokedex->quantidadePokemons == 0){
        return "none";
    }
    struct Node* noAtual = pokedex->head;
    while(noAtual != NULL){
        if(strlen(nomesPokemons)!=0){
            strcat(nomesPokemons, " ");
        }
        const char* nomeAtual = noAtual->pokemon.nome;
        strcat(nomesPokemons, nomeAtual);
        noAtual = noAtual->prox;
    }
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