#ifndef TYPES_H
#define TYPES_H

#include <raylib.h>

//define posição
typedef struct {
    int x;  //coluna
    int y;  //linha
} Vec2i;

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