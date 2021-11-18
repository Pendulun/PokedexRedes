#ifndef POKEDEX_H
#define POKEDEX_H

#define TAM_MAX_NOME_POKEMON 10
#define TAM_MAX_POKEDEX 40
#define NUM_MAX_POKEMON_ADDED 4
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct Pokemon{
    char nome[TAM_MAX_NOME_POKEMON+1];
};

struct Node{
    struct Pokemon pokemon;
    struct Node* prox;
    struct Node* before;
};

struct Pokedex{
    int quantidadePokemons;
    int maxPokemons;
    struct Node* head;
    struct Node* tail;
};

enum ops_pokedex_enum{OK, MAX_LIMIT, ALREADY_EXISTS, DOESNT_EXISTS, INVALID};

bool isInPokedex(struct Pokedex *pokedex, const char *nome);
void adicionarPokemons(struct Pokedex *pokedex, const char* nomes[], const unsigned int numNomes, enum ops_pokedex_enum* results);
enum ops_pokedex_enum removerPokemon(struct Pokedex *pokedex, const char *nome);
enum ops_pokedex_enum trocarPokemon(struct Pokedex *pokedex, const char *nome1, const char *nome2);
char* listarPokemons(struct Pokedex *pokedex, char* nomesPokemons);
void limparPokedex(struct Pokedex *pokedex);

#endif //Pokedex