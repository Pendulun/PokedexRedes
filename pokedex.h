#ifndef POKEDEX_H
#define POKEDEX_H

#define TAM_MAX_NOME_POKEMON 10
#define TAM_MAX_POKEDEX 40

struct Pokemon{
    char nome[TAM_MAX_NOME_POKEMON+1];
};

struct Node{
    struct Pokemon pokemon;
    struct Node* prox;
};

struct Pokedex{
    unsigned int quantidadePokemons;
    unsigned int maxPokemons;
    struct Node* head;
};

enum ops_pokedex_enum{OK, MAX_LIMIT, ALREADY_EXISTS, DOESNT_EXISTS, INVALID};

enum ops_pokedex_enum adicionarPokemon(struct Pokedex *pokedex, const char *nome);
enum ops_pokedex_enum removerPokemon(struct Pokedex *pokedex, const char *nome);
enum ops_pokedex_enum trocarPokemon(struct Pokedex *pokedex, const char *nome1, const char *nome2);
char* listarPokemons(struct Pokedex *pokedex);

#endif //Pokedex