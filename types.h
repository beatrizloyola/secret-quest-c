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
    TILE_PORTA,
    TILE_TERMINAL, // T: terminal de autodestruição, jogador interage pra ativar a contagem
    TILE_SAIDA     // X: saída da estação, jogador precisa chegar aqui depois de ativar o terminal
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
    bool inimigos_mortos[16];
    Vec2i spawn_fragmentos[4]; // posições dos fragmentos do código nessa sala
    int   fragmento_ids[4];    // qual dos 4 fragmentos (0-3) cada spawn corresponde
    int   num_fragmentos;      // quantos fragmentos tem nessa sala (geralmente 0 ou 1)
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
    ENT_ITEM,
    ENT_FRAGMENTO // fragmento do código de autodestruição, coletado ao encostar
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
    int hp; //energia: quanto de hp tem agora (reduzido por inimigos)
    int max_hp; //energia máxima
    int oxigenio; //oxigênio: reduzido pelo timer de tempo; chegar a 0 = game over
    int max_oxigenio; //oxigênio máximo
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
    int spawn_idx; //índice em sala->spawn_inimigos, -1 se não for inimigo
    int sprite_idx; //índice da sprite (0-4 para inimigos, -1 se não usar)
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
    ESTADO_VITORIA,     // fim do jogo (ganhou)
    ESTADO_CUTSCENE     // cutscenes antes do jogo
} EstadoJogo;

// Controla o objetivo principal da missão
typedef struct {
    int   codigo_autodestruicao; // código de 4 dígitos que o jogador precisa digitar no terminal
    int   digitos_coletados;     // bitmask: bit N ligado = fragmento N já foi pego pelo jogador
    int   codigo_ativado;        // vira 1 depois que o jogador digitou o código certo no terminal
    float tempo_restante;        // segundos que sobram pra chegar na saída antes de explodir
    int   ultimo_fragmento;      // id (0-3) do fragmento recém coletado; -1 se nenhum novo. main.c lê e reseta
} Fase;

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