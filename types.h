#ifndef TYPES_H
#define TYPES_H

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

typedef enum {
    ENT_JOGADOR,
    ENT_INIMIGO,
    ENT_ITEM
} EntTipo;

typedef enum {
    DIR_CIMA,
    DIR_BAIXO,
    DIR_ESQUERDA,
    DIR_DIREITA
} Direcao;

typedef struct Entidade {
    EntTipo tipo;
    Vec2i pos;
    int hp, max_hp;
    int ataque;
    bool vivo;
    Direcao direcao;
    float timer_ataque;
    struct Entidade* prox;
} Entidade;

typedef struct {
    Entidade* cabeca;
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
