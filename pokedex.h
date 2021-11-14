#ifndef POKEDEX_H
#define POKEDEX_H

#define TAM_MAX_NOME_POKEMON 10

struct Pokemon{
    char nome[TAM_MAX_NOME_POKEMON+1];
};

short int addPokemon(const char nome[11]);

#endif //Pokedex