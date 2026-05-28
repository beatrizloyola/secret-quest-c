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
    TILE_PAREDE,
    TILE_PORTA
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
    char saida_norte[64];
    char saida_sul[64];
    char saida_leste[64];
    char saida_oeste[64];
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
    EntTipo tipo; //saber o tipo da entidade (jogador, inimigo ou item)
    Vec2i pos; //saber a posição em tile aproximado
    float x; //posição real/fracionada em tiles no eixo x, usada pra movimento fluido
    float y; //posição real/fracionada em tiles no eixo y, usada pra movimento fluido
    float velocidade; //velocidade da entidade em tiles por segundo
    int hp; //quanto de hp tem agr
    int max_hp; //quanto de hp maximo tem
    int ataque; //quanto de dano dá
    Direcao direcao; //saber a direção
    bool vivo;
    float timer_ataque; //timer_ataque;
    float timer_movimento; //timer pra controlar a patrulha do inimigo
    float timer_mudar_direcao; //timer pra IA mudar de direção voluntariamente enquanto anda
    float timer_debug_ataque; //timer pra desenhar a hitbox do ataque durante alguns frames
    int debug_ataque_x; //tile x onde o ataque começou, usado só pro debug visual
    int debug_ataque_y; //tile y onde o ataque começou, usado só pro debug visual
    int debug_ataque_alcance; //quantos tiles o debug do ataque deve desenhar
    Direcao debug_ataque_direcao; //direção do ataque no momento em que o jogador apertou espaço
    bool ataque_ja_acertou; //evita que o mesmo golpe dê dano várias vezes
    struct Entidade* next; //próxima entidade da lista
} Entidade;

typedef struct {
    Entidade* head;
    int contagem;
} ListaEntidades;

// SISTEMA

// Diferentes estados de jogo
typedef enum {
    ESTADO_MENU,        // tela inicial
    ESTADO_JOGANDO,     // tela de jogo
    ESTADO_PAUSE,      // tela de menu/pause
    ESTADO_RANKING,     // informação de rankin
    ESTADO_GAME_OVER,   // fim do jogo (morreu)
    ESTADO_VITORIA      // fim do jogo (ganhou)
} EstadoJogo;

// Template de Recorde
typedef struct {
    char nome[16];
    int score;
} Recorde;

// Estado de jogo
typedef struct {
    Sala * sala_atual;
    ListaEntidades * entidades;
    Entidade * jogador;
    int score;
    EstadoJogo estado;
    int cam_x;
    int cam_y;
} Jogo;

#endif