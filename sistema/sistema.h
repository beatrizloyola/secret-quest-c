#ifndef SISTEMA_H
#define SISTEMA_H

#include "../types.h"

// Tela de menu principal
void menu_inicializar(void);
void menu_finalizar(void);
void menu_atualizar(EstadoJogo* estado);
void menu_desenhar(void);

// Tela de pause
void tela_pause_atualizar(EstadoJogo* estado);
void tela_pause_desenhar(const char* nome_sala, int hp, int score, int oxigenio);

// Tela de ranking
void tela_ranking_atualizar(EstadoJogo* estado);
void tela_ranking_desenhar(void);

// Tela de Game Over
void tela_gameover_desenhar(void);

// Mudança de telas
void sistema_transicao(EstadoJogo* estado_atual, EstadoJogo novo_estado);

// HUD
void hud_desenhar(int score, int oxigenio);

// Sistema Ranking
void ranking_carregar(Recorde* lista, int max);
void ranking_salvar(Recorde* lista, int count);
bool ranking_adicionar(Recorde* lista, int* count, const char* nome, int score);

// Sistema Save
void save_salvar(const char* arquivo_sala, int player_x, int player_y, int hp, int score);
bool save_carregar(char* arquivo_sala_out, int* player_x, int* player_y, int* hp, int* score);

// Assets
void sistema_carregar_assets(void);
void sistema_descarregar_assets(void);

void sistema_desenhar_background(void);
void sistema_desenhar_background_gameover(void);

#endif