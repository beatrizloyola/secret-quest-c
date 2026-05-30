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
    if (IsKeyPressed(KEY_ENTER)) {
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
    snprintf(buffer, sizeof(buffer), "Oxigenio: %d / %d", oxigenio, 15);
    DrawText(buffer, 80, 200, 20, WHITE);
    snprintf(buffer, sizeof(buffer), "Pontuacao: %d", score);
    DrawText(buffer, 80, 240, 20, WHITE);
    DrawText("Pressione ENTER para voltar", 80, 360, 15, GRAY);
}

// Menu Ranking
void tela_ranking_atualizar(EstadoJogo* estado) {
    if (IsKeyPressed(KEY_ENTER)) {
        sistema_transicao(estado, ESTADO_MENU);
    }
}
void tela_ranking_desenhar(void) {
    ClearBackground(BLACK);
    DrawText("RANKING", 100, 80, 40, YELLOW);
    DrawText("--- Em breve ---", 100, 160, 20, GRAY);
    DrawText("Pressione ENTER para voltar", 100, 360, 15, GRAY);
}

// Mudança de Telas
void sistema_transicao(EstadoJogo* estado_atual, EstadoJogo novo_estado) {
    *estado_atual = novo_estado;
}

// HUD
void hud_desenhar(int score, int hp) {
    // Config
    int altura_hud  = 80;
    int largura     = GetScreenWidth();   // 960
    int y_hud       = GetScreenHeight() - altura_hud;  // 880 - 80 = 800
    int energia     = hp;                  // placeholder

    DrawRectangle(0, y_hud, largura, altura_hud, BLACK);

    // Score
    char texto[128];
    snprintf(texto, sizeof(texto), "SCORE: %d", score);
    DrawText(texto, 20, y_hud + 20, 30, WHITE);

    // Cilindro oxigênio

    int cx = 360;
    int cy = y_hud + 20;
    int cw = 140;
    int ch = 26;

    // Bico na esquerda
    int bico_w = 10;
    int bico_h = 16;
    int bico_x = cx - bico_w;
    int bico_y = cy + (ch / 2) - (bico_h / 2);
    DrawRectangle(bico_x, bico_y, bico_w, bico_h, GRAY);

    // Borda
    DrawRectangleLines(cx, cy, cw, ch, WHITE);

    // Preenchimento barra
    float proporcao = (float)hp / 15.0f;
    if (proporcao < 0.0f) proporcao = 0.0f;
    if (proporcao > 1.0f) proporcao = 1.0f;

    int margem = 2;
    int preenchimento_w = (int)((cw - 2 * margem) * proporcao);
    int preenchimento_x = cx + margem;
    int preenchimento_y = cy + margem;
    int preenchimento_h = ch - 2 * margem;

    // Mudança de cor de oxigênio
    Color cor;
    if (hp >= 11)       cor = (Color){ 0, 200, 255, 255 };
    else if (hp >= 6)  cor = (Color){ 0, 150, 200, 255 };
    else                     cor = (Color){ 0, 80, 120, 255 };

    // preenchimento barra de oxigenio
    if (preenchimento_w > 0) {
        DrawRectangle(preenchimento_x, preenchimento_y, preenchimento_w, preenchimento_h, cor);
    }

    // texto "oxigênio" centralizado
    int largura_oxg = MeasureText("Oxigênio", 16);
    int o2_x = cx + (cw / 2) - (largura_oxg / 2);
    DrawText("Oxigênio", o2_x, y_hud + 54, 16, GRAY);

    // Pilha | Barra de Energia
    
    // Pilha
    int px = 800;           //posição horizontal pilha
    int py = y_hud + 20;    // posição vertical pilha
    int pw = 120;           // largura pilha
    int ph = 24;            // altura pilha
    
    // Cabeça pilha
    int cabeca_w = 6;
    int cabeca_h = 12;
    int cabeca_x = px + pw;
    int cabeca_y = py + (ph / 2) - (cabeca_h / 2);
    DrawRectangle(cabeca_x, cabeca_y, cabeca_w, cabeca_h, GRAY);
    
    // Borda da pilha
    DrawRectangleLines(px, py, pw, ph, WHITE);
    
    // Preenchimento pilha
    float proporcaoPilha = (float)energia / 15.0f;
    if (proporcaoPilha < 0.0f) proporcaoPilha = 0.0f;
    if (proporcaoPilha > 1.0f) proporcaoPilha = 1.0f;
    
    int margemPilha = 3;
    int preenchimentoPilha_w = (int)((pw - 2 * margemPilha) * proporcaoPilha);
    int preenchimentoPilha_x = px + margemPilha;
    int preenchimentoPilha_y = py + margemPilha;
    int preenchimentoPilha_h = ph - 2 * margemPilha;
    
    // mudança de cor de energia
    Color corPilha;
    if (energia >= 11)       corPilha = GREEN;
    else if (energia >= 6)  corPilha = YELLOW;
    else                    corPilha = RED;
    
    // preenchimento da barra
    if (preenchimentoPilha_w > 0) {
        DrawRectangle(preenchimentoPilha_x, preenchimentoPilha_y, preenchimentoPilha_w, preenchimentoPilha_h, corPilha);
    }
    
    // texto "energia" centralizado
    int largura_texto = MeasureText("ENERGIA", 14);
    int tag_x = px + (pw / 2) - (largura_texto / 2);
    DrawText("ENERGIA", tag_x, y_hud + 54, 14, GRAY);
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