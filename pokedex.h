#ifndef POKEDEX_H
#define POKEDEX_H

#define TAM_MAX_NOME_POKEMON 10
#define TAM_MAX_POKEDEX 40

struct Pokemon{
    char nome[TAM_MAX_NOME_POKEMON+1];
};

struct Pokedex{
    struct Pokemon pokemons[TAM_MAX_POKEDEX];
};

enum operacoes_enum {ADD, REMOVE, LIST, UPDATE};

short int addPokemon(const struct Pokemon pokemon);

#endif //Pokedex