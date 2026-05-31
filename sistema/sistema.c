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
static Texture2D texPlayerUp;
static Texture2D texPlayerDown;
static Texture2D texPlayerLeft;
static Texture2D texPlayerRight;
static Texture2D texEnemy[5];
static Texture2D texAtaque[5];
static Texture2D texChao;
static Texture2D texParede;
static Texture2D texMoeda;
static Texture2D texPilha;
static Texture2D texOxigenioItem;
static Texture2D texTerminal;
static Texture2D texSaida;
static Texture2D texFragmento;
static Texture2D texBackWIN;

void sistema_carregar_assets(void) {
    background = LoadTexture("assets/background.png");
    backHUD = LoadTexture("assets/backHUD.png");
    backOVER = LoadTexture("assets/backOVER.png");
    texPlayerUp    = LoadTexture("assets/player_up.png");
    texPlayerDown  = LoadTexture("assets/player_down.png");
    texPlayerLeft  = LoadTexture("assets/player_left.png");
    texPlayerRight = LoadTexture("assets/player_right.png");

    // Dar load em cada textura para seu respectivo indice
    texEnemy[0] = LoadTexture("assets/enemy_0.png");
    texEnemy[1] = LoadTexture("assets/enemy_1.png");
    texEnemy[2] = LoadTexture("assets/enemy_2.png");
    texEnemy[3] = LoadTexture("assets/enemy_3.png");
    texEnemy[4] = LoadTexture("assets/enemy_4.png");
    texAtaque[0] = LoadTexture("assets/ataque/SFX301_01.png");
    texAtaque[1] = LoadTexture("assets/ataque/SFX301_02.png");
    texAtaque[2] = LoadTexture("assets/ataque/SFX301_03.png");
    texAtaque[3] = LoadTexture("assets/ataque/SFX301_04.png");
    texAtaque[4] = LoadTexture("assets/ataque/SFX301_05.png");
    texChao   = LoadTexture("assets/chao.png");
    texParede = LoadTexture("assets/parede.png");
    texMoeda        = LoadTexture("assets/moeda.png");
    texPilha        = LoadTexture("assets/pilha.png");
    texOxigenioItem = LoadTexture("assets/oxigenio.png");
    texTerminal     = LoadTexture("assets/terminal.png");
    texSaida        = LoadTexture("assets/saida.png");
    texFragmento    = LoadTexture("assets/fragmento.png");
    texBackWIN      = LoadTexture("assets/backWIN.png");
}

void sistema_descarregar_assets(void) {
    UnloadTexture(background);
    UnloadTexture(backHUD);
    UnloadTexture(backOVER);
    UnloadTexture(texPlayerUp);
    UnloadTexture(texPlayerDown);
    UnloadTexture(texPlayerLeft);
    UnloadTexture(texPlayerRight);
    
    // Pode usar for para fazer unload
    for (int i = 0; i < 5; i++) {
        UnloadTexture(texEnemy[i]);
    }
    for (int i = 0; i < 5; i++) {
        UnloadTexture(texAtaque[i]);
    }
    UnloadTexture(texChao);
    UnloadTexture(texParede);
    UnloadTexture(texMoeda);
    UnloadTexture(texPilha);
    UnloadTexture(texOxigenioItem);
    UnloadTexture(texTerminal);
    UnloadTexture(texSaida);
    UnloadTexture(texFragmento);
    UnloadTexture(texBackWIN);
}

// Movimentação player
Texture2D sistema_get_player_texture(Direcao direcao) {
    switch (direcao) {
        case DIR_CIMA:     return texPlayerUp;
        case DIR_BAIXO:    return texPlayerDown;
        case DIR_ESQUERDA: return texPlayerLeft;
        case DIR_DIREITA:  return texPlayerRight;
    }
    return texPlayerDown;
}

// Assets inimigos
Texture2D sistema_get_enemy_texture(int sprite_idx) {
    if (sprite_idx >= 0 && sprite_idx < 5) {
        return texEnemy[sprite_idx];
    }
    return texEnemy[0];
}

// Assets ataque
Texture2D sistema_get_ataque_texture(int frame) {
    if (frame >= 0 && frame < 5) {
        return texAtaque[frame];
    }
    return texAtaque[0];
}

// Assets mapa
Texture2D sistema_get_chao_texture(void)  { return texChao; }
Texture2D sistema_get_parede_texture(void) { return texParede; }
Texture2D sistema_get_moeda_texture(void)        { return texMoeda; }
Texture2D sistema_get_pilha_texture(void)        { return texPilha; }
Texture2D sistema_get_oxigenio_item_texture(void) { return texOxigenioItem; }
Texture2D sistema_get_terminal_texture(void)     { return texTerminal; }
Texture2D sistema_get_saida_texture(void)        { return texSaida; }
Texture2D sistema_get_fragmento_texture(void)   { return texFragmento; }

// Tela background
void sistema_desenhar_background(void) {
    DrawTexture(background, 0, 0, WHITE);
}
// Tela Game Over
void sistema_desenhar_background_gameover(void) {
    DrawTexture(backOVER, 0, 0, WHITE);
}

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
    int largura_tela = GetScreenWidth();
    int altura_tela = GetScreenHeight();
    float tempo = (float)GetTime();

    sistema_desenhar_background();

    // Configuração layout
    const char* titulo = "SECRET QUEST";
    int titulo_fonte = 64;
    int opcao_fonte = 26;
    int instr_fonte = 14;
    int esp_titulo_linha = 30;
    int esp_linha_opcoes = 40;
    int esp_opcoes = 50;
    int esp_opcoes_instr = 50;

    // Calculos para centralização no y
    int titulo_altura = titulo_fonte;
    int opcoes_altura_total = (num_opcoes * opcao_fonte) + ((num_opcoes - 1) * esp_opcoes);
    int instr_altura = instr_fonte;
    int altura_total = titulo_altura + esp_titulo_linha + 2 + esp_linha_opcoes + opcoes_altura_total + esp_opcoes_instr + instr_altura;
    int inicio_y = (altura_tela - altura_total) / 2;

    // Titulo com sombra
    int titulo_largura = MeasureText(titulo, titulo_fonte);
    int titulo_x = (largura_tela - titulo_largura) / 2;
    int titulo_y = inicio_y;
    Color sombra1 = (Color){ 0, 20, 60, 120 };
    Color sombra2 = (Color){ 0, 50, 100, 160 };
    Color sombra3 = (Color){ 0, 80, 140, 200 };
    Color sombra4 = (Color){ 0, 110, 180, 240 };
    DrawText(titulo, titulo_x + 8, titulo_y + 8, titulo_fonte, sombra1);
    DrawText(titulo, titulo_x + 6, titulo_y + 6, titulo_fonte, sombra2);
    DrawText(titulo, titulo_x + 4, titulo_y + 4, titulo_fonte, sombra3);
    DrawText(titulo, titulo_x + 2, titulo_y + 2, titulo_fonte, sombra4);

    DrawText(titulo, titulo_x, titulo_y, titulo_fonte, (Color){ 0, 230, 255, 255 });

    // Divisão título e opções
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
            // Indicador
            float pulso = sinf(tempo * 5.0f) * 4.0f;
            const char* seta = ">";
            DrawText(seta, (int)(opcao_x - 25 - pulso), opcao_y, opcao_fonte, (Color){ 0, 230, 255, 255 });

            // Opção selecionada
            DrawText(opcoes[i], opcao_x + 1, opcao_y + 1, opcao_fonte, (Color){ 150, 130, 20, 180 });
            DrawText(opcoes[i], opcao_x, opcao_y, opcao_fonte, cor_selecionada);
        } else {
            DrawText(opcoes[i], opcao_x, opcao_y, opcao_fonte, cor_normal);
        }
    }

    // Divisão opções e instruções
    int linha2_y = opcoes_inicio_y + opcoes_altura_total + (esp_opcoes_instr / 2);
    int linha2_largura = linha_largura;
    int linha2_x = linha_x;
    DrawLine(linha2_x, linha2_y, linha2_x + linha2_largura, linha2_y, (Color){ 255, 255, 255, 180 });
    DrawLine(linha2_x, linha2_y + 1, linha2_x + linha2_largura, linha2_y + 1, (Color){ 200, 200, 200, 120 });

    // Instruções centralizadas
    const char* instrucoes = "CIMA / BAIXO para navegar. ENTER para confirmar";
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
void tela_pause_desenhar(const char* nome_sala, int hp, int score, int oxigenio, int digitos_coletados) {
    int largura_tela = GetScreenWidth();
    int altura_tela = GetScreenHeight();

    sistema_desenhar_background();
    int titulo_fonte = 64;
    int subtitulo_fonte = 32;
    int ret_largura = 560;
    int ret_altura = 380;
    int info_fonte = 28;
    int instr_fonte = 20;
    int esp_titulo_sub = 15;
    int esp_sub_ret = 30;
    int esp_ret_instr = 30;

    // Calcula altura do bloco para centralizar no y
    int altura_total = titulo_fonte + esp_titulo_sub + subtitulo_fonte + esp_sub_ret + ret_altura + esp_ret_instr + instr_fonte;
    int inicio_y = (altura_tela - altura_total) / 2;

    // Titulo "PAUSE"
    const char* titulo = "PAUSE";
    int titulo_largura = MeasureText(titulo, titulo_fonte);
    int titulo_x = (largura_tela - titulo_largura) / 2;
    int titulo_y = inicio_y;

    Color sombra1 = (Color){ 0, 20, 60, 120 };
    Color sombra2 = (Color){ 0, 50, 100, 160 };
    Color sombra3 = (Color){ 0, 80, 140, 200 };
    Color sombra4 = (Color){ 0, 110, 180, 240 };
    DrawText(titulo, titulo_x + 8, titulo_y + 8, titulo_fonte, sombra1);
    DrawText(titulo, titulo_x + 6, titulo_y + 6, titulo_fonte, sombra2);
    DrawText(titulo, titulo_x + 4, titulo_y + 4, titulo_fonte, sombra3);
    DrawText(titulo, titulo_x + 2, titulo_y + 2, titulo_fonte, sombra4);
    DrawText(titulo, titulo_x, titulo_y, titulo_fonte, YELLOW);

    // Subtitulo
    const char* subtitulo = "Status do Astronauta";
    int subtitulo_largura = MeasureText(subtitulo, subtitulo_fonte);
    int subtitulo_x = (largura_tela - subtitulo_largura) / 2;
    int subtitulo_y = titulo_y + titulo_fonte + esp_titulo_sub;
    DrawText(subtitulo, subtitulo_x, subtitulo_y, subtitulo_fonte, (Color){ 180, 180, 180, 255 });

    // Bloco
    int ret_x = (largura_tela - ret_largura) / 2;
    int ret_y = subtitulo_y + subtitulo_fonte + esp_sub_ret;
    DrawRectangle(ret_x, ret_y, ret_largura, ret_altura, (Color){ 0, 0, 0, 255 });
    DrawRectangleLines(ret_x, ret_y, ret_largura, ret_altura, WHITE);

    // Status dentro do bloco
    char buffer[128];
    int info_x = ret_x + 40;
    int info_inicio_y = ret_y + 45;
    int info_espaco = 70;

    snprintf(buffer, sizeof(buffer), "Sala Atual: %s", nome_sala ? nome_sala : "Desconhecida");
    DrawText(buffer, info_x, info_inicio_y, info_fonte, WHITE);

    snprintf(buffer, sizeof(buffer), "Energia: %d / %d", hp, 15);
    DrawText(buffer, info_x, info_inicio_y + info_espaco, info_fonte, WHITE);

    snprintf(buffer, sizeof(buffer), "Oxigenio: %d / %d", oxigenio, 15);
    DrawText(buffer, info_x, info_inicio_y + 2 * info_espaco, info_fonte, WHITE);

    snprintf(buffer, sizeof(buffer), "Pontuacao: %d", score);
    DrawText(buffer, info_x, info_inicio_y + 3 * info_espaco, info_fonte, WHITE);

    // Fragmentos coletados
    int frags = 0;
    for (int i = 0; i < 4; i++) if ((digitos_coletados >> i) & 1) frags++;
    snprintf(buffer, sizeof(buffer), "Fragmentos: %d/4", frags);
    DrawText(buffer, info_x, info_inicio_y + 4 * info_espaco, info_fonte, frags == 4 ? ORANGE : WHITE);

    // Instruções embaixo do status
    const char* instrucao = "Pressione ENTER para voltar";
    int instr_largura = MeasureText(instrucao, instr_fonte);
    int instr_x = (largura_tela - instr_largura) / 2;
    int instr_y = ret_y + ret_altura + esp_ret_instr;
    DrawText(instrucao, instr_x, instr_y, instr_fonte, (Color){ 200, 200, 200, 255 });
}

// Menu Ranking
void tela_ranking_atualizar(EstadoJogo* estado) {
    if (IsKeyPressed(KEY_ENTER)) {
        sistema_transicao(estado, ESTADO_MENU);
    }
}
void tela_ranking_desenhar(Recorde* lista, int count) {
    int largura_tela = GetScreenWidth();
    int altura_tela = GetScreenHeight();

    sistema_desenhar_background();
    int titulo_fonte = 72;
    int ret_largura = 560;
    int ret_altura = 420;
    int esp_titulo_ret = 40;
    int esp_ret_instr = 40;
    int instr_fonte = 20;
    int slot_fonte = 24;

    int altura_total = titulo_fonte + esp_titulo_ret + ret_altura + esp_ret_instr + instr_fonte;
    int inicio_y = (altura_tela - altura_total) / 2;

    const char* titulo = "RANKING";
    int titulo_largura = MeasureText(titulo, titulo_fonte);
    int titulo_x = (largura_tela - titulo_largura) / 2;
    int titulo_y = inicio_y;

    Color sombra1 = (Color){ 0, 20, 60, 120 };
    Color sombra2 = (Color){ 0, 50, 100, 160 };
    Color sombra3 = (Color){ 0, 80, 140, 200 };
    Color sombra4 = (Color){ 0, 110, 180, 240 };
    DrawText(titulo, titulo_x + 8, titulo_y + 8, titulo_fonte, sombra1);
    DrawText(titulo, titulo_x + 6, titulo_y + 6, titulo_fonte, sombra2);
    DrawText(titulo, titulo_x + 4, titulo_y + 4, titulo_fonte, sombra3);
    DrawText(titulo, titulo_x + 2, titulo_y + 2, titulo_fonte, sombra4);
    DrawText(titulo, titulo_x, titulo_y, titulo_fonte, YELLOW);

    int ret_x = (largura_tela - ret_largura) / 2;
    int ret_y = titulo_y + titulo_fonte + esp_titulo_ret;
    DrawRectangle(ret_x, ret_y, ret_largura, ret_altura, (Color){ 60, 60, 70, 255 });
    DrawRectangleLines(ret_x, ret_y, ret_largura, ret_altura, WHITE);

    int slot_x = ret_x + 40;
    int slot_inicio_y = ret_y + 35;
    int slot_espaco = 55;

    if (count == 0) {
        DrawText("Nenhum recorde ainda.", slot_x, slot_inicio_y, slot_fonte, (Color){ 180, 180, 180, 255 });
    } else {
        for (int i = 0; i < count && i < 5; i++) {
            char slot_texto[64];
            snprintf(slot_texto, sizeof(slot_texto), "%d.  %-12s  %d", i + 1, lista[i].nome, lista[i].score);
            Color cor_slot = (i == 0) ? GOLD : (Color){ 180, 180, 180, 255 };
            DrawText(slot_texto, slot_x, slot_inicio_y + i * slot_espaco, slot_fonte, cor_slot);
        }
    }

    const char* instrucao = "Pressione ENTER para voltar";
    int instr_largura = MeasureText(instrucao, instr_fonte);
    int instr_x = (largura_tela - instr_largura) / 2;
    int instr_y = ret_y + ret_altura + esp_ret_instr;
    DrawText(instrucao, instr_x, instr_y, instr_fonte, (Color){ 200, 200, 200, 255 });
}

// Menu Game Over
void tela_gameover_desenhar(int score, const char* nome_input) {
    int largura_tela = GetScreenWidth();
    int altura_tela = GetScreenHeight();
    float tempo = (float)GetTime();

    sistema_desenhar_background_gameover();

    const char* titulo = "GAME OVER";
    int titulo_fonte = 72;

    int titulo_largura = MeasureText(titulo, titulo_fonte);
    int titulo_x = (largura_tela - titulo_largura) / 2;
    int titulo_y = (altura_tela / 2) - 220;

    int alpha = (int)(220.0f + 35.0f * sinf(tempo * 2.0f));
    if (alpha > 255) alpha = 255;
    if (alpha < 0) alpha = 0;
    Color cor_principal = (Color){ 255, 40, 40, (unsigned char)alpha };

    Color sombra1 = (Color){ 80, 0, 0, 120 };
    Color sombra2 = (Color){ 120, 0, 0, 160 };
    Color sombra3 = (Color){ 160, 0, 0, 200 };
    Color sombra4 = (Color){ 200, 0, 0, 240 };
    DrawText(titulo, titulo_x + 8, titulo_y + 8, titulo_fonte, sombra1);
    DrawText(titulo, titulo_x + 6, titulo_y + 6, titulo_fonte, sombra2);
    DrawText(titulo, titulo_x + 4, titulo_y + 4, titulo_fonte, sombra3);
    DrawText(titulo, titulo_x + 2, titulo_y + 2, titulo_fonte, sombra4);
    DrawText(titulo, titulo_x, titulo_y, titulo_fonte, cor_principal);

    int linha_y = titulo_y + titulo_fonte + 20;
    int linha_largura = titulo_largura + 40;
    int linha_x = (largura_tela - linha_largura) / 2;
    DrawLine(linha_x, linha_y, linha_x + linha_largura, linha_y, (Color){ 255, 255, 255, 180 });

    char buf[64];
    snprintf(buf, sizeof(buf), "Pontuacao: %d", score);
    int score_largura = MeasureText(buf, 24);
    DrawText(buf, (largura_tela - score_largura) / 2, linha_y + 20, 24, WHITE);

    DrawText("Digite seu nome:", linha_x, linha_y + 68, 20, GRAY);
    DrawText(nome_input, linha_x, linha_y + 96, 28, YELLOW);
    DrawText("ENTER para confirmar", linha_x, linha_y + 148, 16, (Color){ 200, 200, 200, 200 });
}

// Tela background vitória
void sistema_desenhar_background_vitoria(void) {
    DrawTexture(texBackWIN, 0, 0, WHITE);
}

// Tela de Vitória
void tela_vitoria_atualizar(EstadoJogo* estado) {
    if (IsKeyPressed(KEY_ENTER)) { //ENTER volta pro menu depois de vencer
        sistema_transicao(estado, ESTADO_MENU);
    }
}

void tela_vitoria_desenhar(int score, const char* nome_input) {
    int largura_tela = GetScreenWidth();
    int altura_tela = GetScreenHeight();
    float tempo = (float)GetTime();

    sistema_desenhar_background_vitoria();

    const char* titulo = "Você Ganhou!";
    int titulo_fonte = 72;

    int titulo_largura = MeasureText(titulo, titulo_fonte);
    int titulo_x = (largura_tela - titulo_largura) / 2;
    int titulo_y = (altura_tela / 2) - 220;

    int alpha = (int)(220.0f + 35.0f * sinf(tempo * 2.0f));
    if (alpha > 255) alpha = 255;
    if (alpha < 0) alpha = 0;
    Color cor_principal = (Color){ 255, 230, 50, (unsigned char)alpha };

    Color sombra1 = (Color){ 60, 50, 0, 120 };
    Color sombra2 = (Color){ 100, 80, 0, 160 };
    Color sombra3 = (Color){ 140, 110, 0, 200 };
    Color sombra4 = (Color){ 180, 140, 0, 240 };
    DrawText(titulo, titulo_x + 8, titulo_y + 8, titulo_fonte, sombra1);
    DrawText(titulo, titulo_x + 6, titulo_y + 6, titulo_fonte, sombra2);
    DrawText(titulo, titulo_x + 4, titulo_y + 4, titulo_fonte, sombra3);
    DrawText(titulo, titulo_x + 2, titulo_y + 2, titulo_fonte, sombra4);
    DrawText(titulo, titulo_x, titulo_y, titulo_fonte, cor_principal);

    int linha_y = titulo_y + titulo_fonte + 20;
    int linha_largura = titulo_largura + 40;
    int linha_x = (largura_tela - linha_largura) / 2;
    DrawLine(linha_x, linha_y, linha_x + linha_largura, linha_y, (Color){ 255, 255, 255, 180 });

    char buf[64];
    snprintf(buf, sizeof(buf), "Pontuacao final: %d", score);
    int score_largura = MeasureText(buf, 24);
    DrawText(buf, (largura_tela - score_largura) / 2, linha_y + 20, 24, WHITE);

    DrawText("Digite seu nome:", linha_x, linha_y + 68, 20, GRAY);
    DrawText(nome_input, linha_x, linha_y + 96, 28, YELLOW);
    DrawText("ENTER para confirmar", linha_x, linha_y + 148, 16, (Color){ 200, 200, 200, 200 });
}

// Mudança de Telas
void sistema_transicao(EstadoJogo* estado_atual, EstadoJogo novo_estado) {
    *estado_atual = novo_estado;
}

// HUD
void hud_desenhar(int score, int oxigenio, int energia, int digitos_coletados) {
    // Config
    int altura_hud  = 140;
    int y_hud       = GetScreenHeight() - altura_hud;
    DrawTexture(backHUD, 0, y_hud, GRAY);

    // Score (parte superior do HUD)
    char texto[128];
    snprintf(texto, sizeof(texto), "SCORE: %d", score);
    DrawText(texto, 310, y_hud + 18, 36, WHITE);

    // Fragmentos (parte inferior do HUD, mesma posição X que Score)
    int frags = 0;
    for (int i = 0; i < 4; i++) if ((digitos_coletados >> i) & 1) frags++;
    snprintf(texto, sizeof(texto), "Fragmentos: %d/4", frags);
    DrawText(texto, 310, y_hud + 92, 22, frags == 4 ? ORANGE : WHITE);

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

    // Preenchimento barra de oxigênio
    float proporcao = (float)oxigenio / 15.0f;
    if (proporcao < 0.0f) proporcao = 0.0f;
    if (proporcao > 1.0f) proporcao = 1.0f;

    int margem = 3;
    int preenchimento_w = (int)((cw - 2 * margem) * proporcao);
    int preenchimento_x = cx + margem;
    int preenchimento_y = cy + margem;
    int preenchimento_h = ch - 2 * margem;

    // Mudança de cor de oxigênio
    Color cor;
    if (oxigenio >= 11)      cor = (Color){ 0, 200, 255, 255 };
    else if (oxigenio >= 6)  cor = (Color){ 0, 150, 200, 255 };
    else                     cor = (Color){ 0, 80,  120, 255 };

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
    int largura_texto = MeasureText("Energia", 20);
    int tag_x = px + (pw / 2) - (largura_texto / 2);
    DrawText("Energia", tag_x, y_hud + 82, 20, GRAY);
}

// Sistema Ranking
#define RANKING_ARQUIVO "highscores.dat"

void ranking_carregar(Recorde* lista, int max) {
    memset(lista, 0, sizeof(Recorde) * max);
    FILE* f = fopen(RANKING_ARQUIVO, "rb");
    if (!f) return;
    fread(lista, sizeof(Recorde), max, f);
    fclose(f);
}

void ranking_salvar(Recorde* lista, int count) {
    FILE* f = fopen(RANKING_ARQUIVO, "wb");
    if (!f) return;
    fwrite(lista, sizeof(Recorde), count, f);
    fclose(f);
}

bool ranking_adicionar(Recorde* lista, int* count, const char* nome, int score) {
    int n = *count;
    if (n >= 5 && score <= lista[n - 1].score) return false;

    if (n < 5) n++;
    lista[n - 1].score = score;
    strncpy(lista[n - 1].nome, nome, 15);
    lista[n - 1].nome[15] = '\0';

    for (int i = n - 1; i > 0 && lista[i].score > lista[i - 1].score; i--) {
        Recorde tmp = lista[i];
        lista[i]     = lista[i - 1];
        lista[i - 1] = tmp;
    }

    *count = n;
    return true;
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