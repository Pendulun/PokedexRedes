#include "pokedex.h"

bool caracterEspecial(const char *c){
    return !((*c >= 'a' && *c <= 'z') || (*c >= '0' && *c <= '9'));
}

bool nomeInvalido(const char* nome){

    if(strlen(nome) > 10){
        return true;
    }

    for(unsigned int idxLetra = 0; idxLetra < strlen(nome); idxLetra++){
        if(caracterEspecial(&nome[idxLetra])){
            return true;
        }
    }

    return false;
}


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

bool estaNaPokedex(const struct Pokedex *pokedex, const char* nome){
    return searchInPokedex(pokedex, nome) != NULL;
}

void adicionarPokemons(struct Pokedex *pokedex, const char* nomes[], const unsigned int numNomes, enum ops_pokedex_enum *results){
    for(unsigned int idxPokemonAdd=0; idxPokemonAdd<numNomes; idxPokemonAdd++){

        if(nomeInvalido(nomes[idxPokemonAdd])){
            results[idxPokemonAdd] = INVALID;
            continue;
        }

        if(pokedex->quantidadePokemons == pokedex->maxPokemons){
            results[idxPokemonAdd] = MAX_LIMIT;
            continue;
        }


        if(estaNaPokedex(pokedex, nomes[idxPokemonAdd])){
            results[idxPokemonAdd] = ALREADY_EXISTS;
            continue;
        }
        
        //Não existem impedimentos
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
}

enum ops_pokedex_enum removerPokemon(struct Pokedex *pokedex, const char *nome){
    if(nomeInvalido(nome)){
        return INVALID;
    }

    if(pokedex->quantidadePokemons == 0){
        return DOESNT_EXISTS;
    }

    struct Node* noPokemon = searchInPokedex(pokedex, nome);
    if(noPokemon == NULL){
        return DOESNT_EXISTS;
    }
        
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

enum ops_pokedex_enum trocarPokemon(struct Pokedex *pokedex, const char *nome1, const char *nome2){
    if(nomeInvalido(nome1) || nomeInvalido(nome2)){
        return INVALID;
    }

    if(pokedex->quantidadePokemons == 0){
        return DOESNT_EXISTS;
    }

    struct Node* noPokemon1 = searchInPokedex(pokedex, nome1);
    if(noPokemon1 == NULL){
        return DOESNT_EXISTS;
    }

    struct Node* noPokemon2 = searchInPokedex(pokedex, nome2);
    if(noPokemon2 != NULL){
        return ALREADY_EXISTS;
    }

    //Não existem impedimentos
    strcpy(noPokemon1->pokemon.nome, nome2);
    return OK;

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