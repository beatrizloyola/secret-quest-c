#ifndef TYPES_H
#define TYPES_H

<<<<<<< HEAD
#include <stdbool.h>

typedef struct {
    int x, y;
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
    char arquivo[64];
} Sala;

=======
#include <raylib.h>

//define posição
typedef struct {
    int x;  //coluna
    int y;  //linha
} Vec2i;

//define tipo de entidade
>>>>>>> a5dfd53c4e4b5a041be6f704f4557e11e336f6c2
typedef enum {
    ENT_JOGADOR,
    ENT_INIMIGO,
    ENT_ITEM
} EntTipo;

<<<<<<< HEAD
=======
//define direção da entidade
>>>>>>> a5dfd53c4e4b5a041be6f704f4557e11e336f6c2
typedef enum {
    DIR_CIMA,
    DIR_BAIXO,
    DIR_ESQUERDA,
    DIR_DIREITA
} Direcao;

typedef struct Entidade {
<<<<<<< HEAD
    EntTipo tipo;
    Vec2i pos;
    int hp, max_hp;
    int ataque;
    bool vivo;
    Direcao direcao;
    float timer_ataque;
    struct Entidade* prox;
=======
    EntTipo tipo; //saber o tipo
    Vec2i pos; //saber a posição
    int hp; //quanto de hp tem agr
    int max_hp; //quanto de hp maximo tem
    int ataque; //quanto de dano dá
    Direcao direcao; //saber a direção (NÃO IMPLEMENTADO)
    struct Entidade* next; //próxima entidade da lista
>>>>>>> a5dfd53c4e4b5a041be6f704f4557e11e336f6c2
} Entidade;

typedef struct {
    Entidade* cabeca;
<<<<<<< HEAD
    int contagem;
} ListaEntidades;

typedef struct {
    char nome[16];
    int score;
} Recorde;

typedef enum {
    ESTADO_MENU,
    ESTADO_JOGANDO,
    ESTADO_PAUSA,
    ESTADO_GAME_OVER,
    ESTADO_RANKING,
    ESTADO_VITORIA
} EstadoJogo;

typedef struct {
    Sala* sala_atual;
    ListaEntidades* entidades;
    Entidade* jogador;
    int score;
    EstadoJogo estado;
    int cam_x, cam_y;
} Jogo;

#endif
=======
} ListaEntidades;

#endif
>>>>>>> a5dfd53c4e4b5a041be6f704f4557e11e336f6c2
