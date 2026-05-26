#ifndef TYPES_H
#define TYPES_H

#include <raylib.h>
#include <stdbool.h>

typedef struct {
    int x;
    int y;
} Vec2i;

typedef enum {
    TILE_CHAO,
    TILE_PAREDE
} TileType;

typedef struct {
    TileType tipo;
    bool solido;
} Tile;

typedef struct {
    int largura, altura;
    Tile** grid;
    Vec2i spawn_jogador;
    Vec2i spawn_inimigos[16];
    int num_inimigos;
    Vec2i spawn_itens[16];
    int num_itens;
    char arquivo[64];
} Sala;

//define tipo de entidade

typedef enum {
    ENT_JOGADOR,
    ENT_INIMIGO,
    ENT_ITEM
} EntTipo;

//define direção da entidade
typedef enum {
    DIR_CIMA,
    DIR_BAIXO,
    DIR_ESQUERDA,
    DIR_DIREITA
} Direcao;

typedef struct Entidade {
    EntTipo tipo; //saber o tipo
    Vec2i pos; //saber a posição
    int hp; //quanto de hp tem agr
    int max_hp; //quanto de hp maximo tem
    int ataque; //quanto de dano dá
    Direcao direcao; //saber a direção (NÃO IMPLEMENTADO)
    struct Entidade* next; //próxima entidade da lista
} Entidade;

typedef struct {
    Entidade* cabeca;
} ListaEntidades;

#endif
