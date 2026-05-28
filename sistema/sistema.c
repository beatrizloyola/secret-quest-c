#include "sistema.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

// Menu Principal
static int opcao_selecionada = 0;
static const char* opcoes[] = { "Iniciar Jogo", "Ranking", "Sair" };
static const int num_opcoes = 3;
void menu_inicializar(void) {
    opcao_selecionada = 0;
}
void menu_finalizar(void) {
    // vazio por enquanto
}

void menu_atualizar(EstadoJogo* estado) {
    if (IsKeyPressed(KEY_DOWN)) {
        opcao_selecionada++;
        if (opcao_selecionada >= num_opcoes) opcao_selecionada = 0;
    }
    if (IsKeyPressed(KEY_UP)) {
        opcao_selecionada--;
        if (opcao_selecionada < 0) opcao_selecionada = num_opcoes - 1;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        switch (opcao_selecionada) {
            case 0: sistema_transicao(estado, ESTADO_JOGANDO); break;
            case 1: sistema_transicao(estado, ESTADO_RANKING); break;
            case 2: CloseWindow(); break;
        }
    }
}
void menu_desenhar(void) {
    ClearBackground(BLACK);
    DrawText("SECRET QUEST", 100, 80, 40, YELLOW);
    for (int i = 0; i < num_opcoes; i++) {
        Color cor = (i == opcao_selecionada) ? RED : WHITE;
        DrawText(opcoes[i], 100, 160 + i * 40, 20, cor);
    }
    DrawText("Use CIMA/BAIXO + ENTER", 100, 360, 10, GRAY);
}

// Menu Pause
void tela_pause_atualizar(EstadoJogo* estado) {
    if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_ENTER)) {
        sistema_transicao(estado, ESTADO_JOGANDO);
    }
}
void tela_pause_desenhar(const char* nome_sala, int hp, int score, int oxigenio) {
    ClearBackground(DARKBLUE);
    DrawText("STATUS DO ASTRONAUTA", 80, 60, 30, YELLOW);
    // Placeholders com layout limpo
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Sala Atual: %s", nome_sala ? nome_sala : "Desconhecida");
    DrawText(buffer, 80, 120, 20, WHITE);
    snprintf(buffer, sizeof(buffer), "Energia: %d / %d", hp, 10);
    DrawText(buffer, 80, 160, 20, WHITE);
    snprintf(buffer, sizeof(buffer), "Oxigenio: %d / %d", oxigenio, 100);
    DrawText(buffer, 80, 200, 20, WHITE);
    snprintf(buffer, sizeof(buffer), "Pontuacao: %d", score);
    DrawText(buffer, 80, 240, 20, WHITE);
    DrawText("Pressione TAB ou ENTER para voltar", 80, 360, 15, GRAY);
}

// Menu Ranking
void tela_ranking_atualizar(EstadoJogo* estado) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        sistema_transicao(estado, ESTADO_MENU);
    }
}
void tela_ranking_desenhar(void) {
    ClearBackground(BLACK);
    DrawText("RANKING", 100, 80, 40, YELLOW);
    DrawText("--- Em breve ---", 100, 160, 20, GRAY);
    DrawText("Pressione ESC para voltar", 100, 360, 15, GRAY);
}

// Mudança de Telas
void sistema_transicao(EstadoJogo* estado_atual, EstadoJogo novo_estado) {
    *estado_atual = novo_estado;
}

// HUD
void hud_desenhar(int score, int hp) {
    (void)score;
    (void)hp;
    // placeholder para dps
}

// Sistema Ranking (placeholder)
void ranking_carregar(Recorde* lista, int max) {
    (void)lista;
    (void)max;
}
void ranking_salvar(Recorde* lista, int count) {
    (void)lista;
    (void)count;
}
bool ranking_adicionar(Recorde* lista, int* count, const char* nome, int score) {
    (void)lista;
    (void)count;
    (void)nome;
    (void)score;
    return false;
}


// Sistema Save (placeholder)
void save_salvar(const char* arquivo_sala, int player_x, int player_y, int hp, int score) {
    (void)arquivo_sala;
    (void)player_x;
    (void)player_y;
    (void)hp;
    (void)score;
}
bool save_carregar(char* arquivo_sala_out, int* player_x, int* player_y, int* hp, int* score) {
    (void)arquivo_sala_out;
    (void)player_x;
    (void)player_y;
    (void)hp;
    (void)score;
    return false;
}