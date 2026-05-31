#include "sistema.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define NUM_ESTRELAS 40

// Menu Principal
static int opcao_selecionada = 0;
static const char* opcoes[] = { "Iniciar Jogo", "Ranking", "Sair" };
static const int num_opcoes = 3;
static Texture2D background;
static Texture2D backHUD;
static Texture2D backOVER;

void sistema_carregar_assets(void) {
    background = LoadTexture("assets/background.png");
    backHUD = LoadTexture("assets/backHUD.png");
    backOVER = LoadTexture("assets/backOVER.png");

}

void sistema_descarregar_assets(void) {
    UnloadTexture(background);
    UnloadTexture(backHUD);
    UnloadTexture(backOVER);
}

void sistema_desenhar_background(void) {
    DrawTexture(background, 0, 0, WHITE);
}
void sistema_desenhar_background_gameover(void) {
    DrawTexture(backOVER, 0, 0, WHITE);
}

void menu_inicializar(void) {
    opcao_selecionada = 0;
    int largura = GetScreenWidth();
    int altura = GetScreenHeight();
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
    int largura_tela = GetScreenWidth();
    int altura_tela = GetScreenHeight();
    float tempo = (float)GetTime();

    // Background do menu
    sistema_desenhar_background();

    // Configurações de layout
    const char* titulo = "SECRET QUEST";
    int titulo_fonte = 64;
    int opcao_fonte = 26;
    int instr_fonte = 14;
    int esp_titulo_linha = 30;
    int esp_linha_opcoes = 40;
    int esp_opcoes = 50;
    int esp_opcoes_instr = 50;

    // Medidas para centralização vertical
    int titulo_altura = titulo_fonte;
    int opcoes_altura_total = (num_opcoes * opcao_fonte) + ((num_opcoes - 1) * esp_opcoes);
    int instr_altura = instr_fonte;
    int altura_total = titulo_altura + esp_titulo_linha + 2 + esp_linha_opcoes + opcoes_altura_total + esp_opcoes_instr + instr_altura;
    int inicio_y = (altura_tela - altura_total) / 2;

    // Título com sombreamento em camadas
    int titulo_largura = MeasureText(titulo, titulo_fonte);
    int titulo_x = (largura_tela - titulo_largura) / 2;
    int titulo_y = inicio_y;

    // Sombra espalhada em camadas (mais visível)
    Color sombra1 = (Color){ 0, 20, 60, 120 };
    Color sombra2 = (Color){ 0, 50, 100, 160 };
    Color sombra3 = (Color){ 0, 80, 140, 200 };
    Color sombra4 = (Color){ 0, 110, 180, 240 };
    DrawText(titulo, titulo_x + 8, titulo_y + 8, titulo_fonte, sombra1);
    DrawText(titulo, titulo_x + 6, titulo_y + 6, titulo_fonte, sombra2);
    DrawText(titulo, titulo_x + 4, titulo_y + 4, titulo_fonte, sombra3);
    DrawText(titulo, titulo_x + 2, titulo_y + 2, titulo_fonte, sombra4);

    // Título principal
    DrawText(titulo, titulo_x, titulo_y, titulo_fonte, (Color){ 0, 230, 255, 255 });

    // Linha divisória entre título e opções
    int linha_y = titulo_y + titulo_altura + esp_titulo_linha;
    int linha_largura = titulo_largura + 60;
    int linha_x = (largura_tela - linha_largura) / 2;
    DrawLine(linha_x, linha_y, linha_x + linha_largura, linha_y, (Color){ 255, 255, 255, 180 });
    DrawLine(linha_x, linha_y + 1, linha_x + linha_largura, linha_y + 1, (Color){ 200, 200, 200, 120 });

    // Opções centralizadas
    int opcoes_inicio_y = linha_y + 2 + esp_linha_opcoes;
    Color cor_normal = (Color){ 170, 170, 190, 255 };
    Color cor_selecionada = (Color){ 255, 230, 50, 255 };

    for (int i = 0; i < num_opcoes; i++) {
        int opcao_largura = MeasureText(opcoes[i], opcao_fonte);
        int opcao_x = (largura_tela - opcao_largura) / 2;
        int opcao_y = opcoes_inicio_y + i * (opcao_fonte + esp_opcoes);

        if (i == opcao_selecionada) {
            // Indicador: seta pulsante à esquerda
            float pulso = sinf(tempo * 5.0f) * 4.0f;
            const char* seta = ">";
            DrawText(seta, (int)(opcao_x - 25 - pulso), opcao_y, opcao_fonte, (Color){ 0, 230, 255, 255 });

            // Texto selecionado com sombra sutil
            DrawText(opcoes[i], opcao_x + 1, opcao_y + 1, opcao_fonte, (Color){ 150, 130, 20, 180 });
            DrawText(opcoes[i], opcao_x, opcao_y, opcao_fonte, cor_selecionada);
        } else {
            DrawText(opcoes[i], opcao_x, opcao_y, opcao_fonte, cor_normal);
        }
    }

    // Linha divisória entre opções e instruções
    int linha2_y = opcoes_inicio_y + opcoes_altura_total + (esp_opcoes_instr / 2);
    int linha2_largura = linha_largura;
    int linha2_x = linha_x;
    DrawLine(linha2_x, linha2_y, linha2_x + linha2_largura, linha2_y, (Color){ 255, 255, 255, 180 });
    DrawLine(linha2_x, linha2_y + 1, linha2_x + linha2_largura, linha2_y + 1, (Color){ 200, 200, 200, 120 });

    // Instruções centralizadas na base
    const char* instrucoes = "Use ↑ / ↓ para navegar. Use ENTER para confirmar";
    int instr_largura = MeasureText(instrucoes, instr_fonte);
    int instr_x = (largura_tela - instr_largura) / 2;
    int instr_y = opcoes_inicio_y + opcoes_altura_total + esp_opcoes_instr;
    DrawText(instrucoes, instr_x, instr_y, instr_fonte, (Color){ 100, 100, 120, 255 });
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
    sistema_desenhar_background();
    DrawText("RANKING", 100, 80, 40, YELLOW);
    DrawText("--- Em breve ---", 100, 160, 20, GRAY);
    DrawText("Pressione ENTER para voltar", 100, 360, 15, GRAY);
}

// Menu Game Over
void tela_gameover_desenhar(void) {
    int largura_tela = GetScreenWidth();
    int altura_tela = GetScreenHeight();
    float tempo = (float)GetTime();

    sistema_desenhar_background_gameover();

    const char* titulo = "GAME OVER";
    int titulo_fonte = 72;
    int instr_fonte = 20;
    int esp_titulo_instr = 80;

    int titulo_largura = MeasureText(titulo, titulo_fonte);
    int titulo_x = (largura_tela - titulo_largura) / 2;
    int titulo_y = (altura_tela / 2) - 200;

    // Texto pulsando
    int alpha = (int)(220.0f + 35.0f * sinf(tempo * 2.0f));
    if (alpha > 255) alpha = 255;
    if (alpha < 0) alpha = 0;

    Color cor_principal = (Color){ 255, 40, 40, (unsigned char)alpha };

    // Sombra em camadas
    Color sombra1 = (Color){ 80, 0, 0, 120 };
    Color sombra2 = (Color){ 120, 0, 0, 160 };
    Color sombra3 = (Color){ 160, 0, 0, 200 };
    Color sombra4 = (Color){ 200, 0, 0, 240 };

    DrawText(titulo, titulo_x + 8, titulo_y + 8, titulo_fonte, sombra1);
    DrawText(titulo, titulo_x + 6, titulo_y + 6, titulo_fonte, sombra2);
    DrawText(titulo, titulo_x + 4, titulo_y + 4, titulo_fonte, sombra3);
    DrawText(titulo, titulo_x + 2, titulo_y + 2, titulo_fonte, sombra4);

    // Texto principal
    DrawText(titulo, titulo_x, titulo_y, titulo_fonte, cor_principal);

    // Linha divisória
    int linha_y = titulo_y + titulo_fonte + 20;
    int linha_largura = titulo_largura + 40;
    int linha_x = (largura_tela - linha_largura) / 2;
    DrawLine(linha_x, linha_y, linha_x + linha_largura, linha_y, (Color){ 255, 255, 255, 180 });

    // Instrução centralizada
    const char* instrucao = "Pressione ENTER para voltar ao menu";
    int instr_largura = MeasureText(instrucao, instr_fonte);
    int instr_x = (largura_tela - instr_largura) / 2;
    int instr_y = linha_y + 30;

    DrawText(instrucao, instr_x, instr_y, instr_fonte, (Color){ 255, 255, 255, 200 });
}

// Mudança de Telas
void sistema_transicao(EstadoJogo* estado_atual, EstadoJogo novo_estado) {
    *estado_atual = novo_estado;
}

// HUD
void hud_desenhar(int score, int hp) {
    // Config
    int altura_hud  = 140;
    int y_hud       = GetScreenHeight() - altura_hud;
    int energia     = hp;

    DrawTexture(backHUD, 0, y_hud, GRAY);

    // Score
    char texto[128];
    snprintf(texto, sizeof(texto), "SCORE: %d", score);
    DrawText(texto, 310, y_hud + 52, 36, WHITE);

    // Cilindro oxigênio
    int cx = 580;
    int cy = y_hud + 40;
    int cw = 160;
    int ch = 32;

    // Bico na direita
    int bico_w = 12;
    int bico_h = 20;
    int bico_x = cx + cw;
    int bico_y = cy + (ch / 2) - (bico_h / 2);
    DrawRectangle(bico_x, bico_y, bico_w, bico_h, WHITE);

    // Borda grossa (3px)
    DrawRectangle(cx, cy, cw, 3, WHITE);
    DrawRectangle(cx, cy + ch - 3, cw, 3, WHITE);
    DrawRectangle(cx, cy + 3, 3, ch - 6, WHITE);
    DrawRectangle(cx + cw - 3, cy + 3, 3, ch - 6, WHITE);

    // Preenchimento barra
    float proporcao = (float)hp / 15.0f;
    if (proporcao < 0.0f) proporcao = 0.0f;
    if (proporcao > 1.0f) proporcao = 1.0f;

    int margem = 3;
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
    int largura_oxg = MeasureText("Oxigênio", 20);
    int o2_x = cx + (cw / 2) - (largura_oxg / 2);
    DrawText("Oxigênio", o2_x, y_hud + 82, 20, GRAY);

    // Pilha | Barra de Energia
    int px = 770;
    int py = y_hud + 40;
    int pw = 160;
    int ph = 32;

    // Cabeça pilha
    int cabeca_w = 12;
    int cabeca_h = 20;
    int cabeca_x = px + pw;
    int cabeca_y = py + (ph / 2) - (cabeca_h / 2);
    DrawRectangle(cabeca_x, cabeca_y, cabeca_w, cabeca_h, WHITE);

    // Borda grossa (3px)
    DrawRectangle(px, py, pw, 3, WHITE);
    DrawRectangle(px, py + ph - 3, pw, 3, WHITE);
    DrawRectangle(px, py + 3, 3, ph - 6, WHITE);
    DrawRectangle(px + pw - 3, py + 3, 3, ph - 6, WHITE);

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
    int largura_texto = MeasureText("ENERGIA", 20);
    int tag_x = px + (pw / 2) - (largura_texto / 2);
    DrawText("ENERGIA", tag_x, y_hud + 82, 20, GRAY);
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