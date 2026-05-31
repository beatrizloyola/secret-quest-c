#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "raylib.h"
#include "types.h"
#include "mapa/mapa.h"
#include "entidade/entidade.h"
#include "sistema/sistema.h"

#define TAMANHO_TILE  80
#define CAMERA_LARGURA 12
#define CAMERA_ALTURA  10
#define SALA_CACHE_MAX 32

static Sala* sala_cache[SALA_CACHE_MAX]; //salas ficam em memória durante toda a partida pra preservar inimigos_mortos
static int   sala_cache_n = 0;

static Sala* sala_obter(const char* arquivo) { //retorna sala do cache se já foi carregada, senão carrega do disco e guarda
    for (int i = 0; i < sala_cache_n; i++) {
        if (strncmp(sala_cache[i]->arquivo, arquivo, 63) == 0) {
            return sala_cache[i];
        }
    }
    if (sala_cache_n >= SALA_CACHE_MAX) return NULL;
    Sala* s = sala_carregar(arquivo);
    if (s) sala_cache[sala_cache_n++] = s;
    return s;
}

static void sala_cache_limpar(void) { //libera todas as salas do cache; chamado no game over e ao fechar janela
    for (int i = 0; i < sala_cache_n; i++) {
        sala_descarregar(sala_cache[i]);
    }
    sala_cache_n = 0;
}

/*
Mudanças levemente drásticas no main.c pra permitir que o jogador e os inimigos se movam de maneira fluida, 
além de integrar todo o sistema de lista encadeada e as trocentas funções q eu escrevi.
Se tem um trecho q eu apaguei é pq tem uma função no entidades.c que faz a mesma coisa so q mais dificil.

se eu quebrei alguma outra coisa q ja tava funcionando peço perdao
*/

static bool tile_livre_spawn(Sala* sala, int x, int y) { //verifica se uma tile pode receber o jogador depois de trocar de sala
    if (sala == NULL) { //resenha ou morte! - dom pedro Iº
        return false;
    }

    if (x < 0 || y < 0 || x >= sala->largura || y >= sala->altura) { //se saiu do mapa, não serve
        return false;
    }

    if (sala->grid[y][x].tipo == TILE_PORTA) { //não spawna em cima da porta pra evitar loop infinito entre salas
        return false;
    }

    return !sala_colisao(sala, x, y); //se não for parede, serve
}

static Direcao lado_entrada_sala_nova(Direcao dir_saida) { //converte a porta por onde saiu no lado por onde entra na sala nova
    switch (dir_saida) {
        case DIR_CIMA:     return DIR_BAIXO;
        case DIR_BAIXO:    return DIR_CIMA;
        case DIR_ESQUERDA: return DIR_DIREITA;
        case DIR_DIREITA:  return DIR_ESQUERDA;
    }

    return DIR_BAIXO;
}

static bool procurar_spawn_na_entrada(Sala* sala, Direcao lado_entrada, int* out_x, int* out_y) { //procura uma tile livre logo depois da porta da sala nova
    if (sala == NULL || out_x == NULL || out_y == NULL) { //tdah ou vagabundagem?
        return false;
    }

    if (lado_entrada == DIR_ESQUERDA) { //entrou pelo lado esquerdo, então procura portas na coluna 0 e spawna pra dentro da sala
        for (int y = 0; y < sala->altura; y++) {
            if (sala->grid[y][0].tipo == TILE_PORTA) {
                for (int x = 1; x < sala->largura; x++) {
                    if (tile_livre_spawn(sala, x, y)) {
                        *out_x = x;
                        *out_y = y;
                        return true;
                    }
                }
            }
        }
    }
    else if (lado_entrada == DIR_DIREITA) { //entrou pelo lado direito, então procura portas na última coluna e spawna pra dentro da sala
        int porta_x = sala->largura - 1;
        for (int y = 0; y < sala->altura; y++) {
            if (sala->grid[y][porta_x].tipo == TILE_PORTA) {
                for (int x = sala->largura - 2; x >= 0; x--) {
                    if (tile_livre_spawn(sala, x, y)) {
                        *out_x = x;
                        *out_y = y;
                        return true;
                    }
                }
            }
        }
    }
    else if (lado_entrada == DIR_CIMA) { //entrou por cima, então procura portas na linha 0 e spawna pra dentro da sala
        for (int x = 0; x < sala->largura; x++) {
            if (sala->grid[0][x].tipo == TILE_PORTA) {
                for (int y = 1; y < sala->altura; y++) {
                    if (tile_livre_spawn(sala, x, y)) {
                        *out_x = x;
                        *out_y = y;
                        return true;
                    }
                }
            }
        }
    }
    else if (lado_entrada == DIR_BAIXO) { //entrou por baixo, então procura portas na última linha e spawna pra dentro da sala
        int porta_y = sala->altura - 1;
        for (int x = 0; x < sala->largura; x++) {
            if (sala->grid[porta_y][x].tipo == TILE_PORTA) {
                for (int y = sala->altura - 2; y >= 0; y--) {
                    if (tile_livre_spawn(sala, x, y)) {
                        *out_x = x;
                        *out_y = y;
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

static bool procurar_spawn_fallback(Sala* sala, int* out_x, int* out_y) { //plano B caso alguma sala esteja sem porta do lado esperado
    if (sala == NULL || out_x == NULL || out_y == NULL) { //ai dento
        return false;
    }

    if (tile_livre_spawn(sala, sala->spawn_jogador.x, sala->spawn_jogador.y)) { //se o S da sala for válido, usa ele
        *out_x = sala->spawn_jogador.x;
        *out_y = sala->spawn_jogador.y;
        return true;
    }

    for (int y = 0; y < sala->altura; y++) { //se der tudo errado ele procura qualquer chão livre na sala
        for (int x = 0; x < sala->largura; x++) {
            if (tile_livre_spawn(sala, x, y)) {
                *out_x = x;
                *out_y = y;
                return true;
            }
        }
    }

    return false;
}

static void posicionar_jogador_apos_porta(Sala* sala, Entidade* jogador, Direcao dir_saida) { //posiciona o jogador na entrada correta da sala nova, sem alterar o desenho das salas
    if (sala == NULL || jogador == NULL) { //computador ligue a maquina de averiguar resenha
        return;
    }

    int novo_x = jogador->pos.x;
    int novo_y = jogador->pos.y;
    Direcao lado_entrada = lado_entrada_sala_nova(dir_saida);

    if (!procurar_spawn_na_entrada(sala, lado_entrada, &novo_x, &novo_y)) { //se não achou a porta esperada, usa fallback seguro
        procurar_spawn_fallback(sala, &novo_x, &novo_y);
    }

    jogador->pos.x = novo_x;
    jogador->pos.y = novo_y;
    jogador->x = jogador->pos.x + 0.5f; //sincroniza posição real no centro da tile depois de trocar de sala
    jogador->y = jogador->pos.y + 0.5f; //ler comentario acima
}


int main(void) {
    InitWindow(CAMERA_LARGURA * TAMANHO_TILE, CAMERA_ALTURA * TAMANHO_TILE, "Secret Quest");
    SetTargetFPS(60);

    menu_inicializar();
    EstadoJogo estado = ESTADO_MENU;
    Sala* sala = NULL;
    ListaEntidades* entidades = NULL;
    Entidade* jogador = NULL;
    int score = 0;
    int player_oxigenio = 100; // placeholder
    bool bloquear_enter_menu = false; //evita que o ENTER do game over já aperte "iniciar jogo" no menu

    Recorde ranking[5];
    int ranking_count = 0;
    char nome_input[16] = {0};
    int nome_input_len = 0;

    ranking_carregar(ranking, 5);
    for (int i = 0; i < 5; i++) {
        if (ranking[i].score > 0) ranking_count++;
        else break;
    }


    while (!WindowShouldClose()) {

        switch (estado) {

            case ESTADO_MENU:
                if (bloquear_enter_menu) { //se veio do game over, espera soltar ENTER antes do menu aceitar input (tava bugado antes)
                    if (!IsKeyDown(KEY_ENTER)) {
                        bloquear_enter_menu = false;
                    }
                }
                else {
                    menu_atualizar(&estado);
                }

                BeginDrawing();
                menu_desenhar();
                EndDrawing();
                break;

            case ESTADO_JOGANDO: {
                if (sala == NULL) {
                    sala = sala_obter("mapa/salas/sala1.txt");
                    if (sala) {
                        entidades = lista_criar();
                        lista_spawnar_sala(entidades, sala, &jogador);
                        score = 0;
                        nome_input[0] = '\0';
                        nome_input_len = 0;
                    }
                }

                if (!sala || !entidades || !jogador) break;

                if (IsKeyPressed(KEY_TAB)) {
                    sistema_transicao(&estado, ESTADO_PAUSE);
                    break;
                }

                float dt = GetFrameTime();

                entidade_mover_jogador(jogador, sala, dt); //movimentação

                if (IsKeyPressed(KEY_SPACE)) { //porrada 💥💥
                    entidade_atacar(jogador, entidades, sala, 1, &score);
                }

                lista_atualizar(entidades, sala, jogador, dt, &score, &estado); //atualiza IA, dano, cooldown e coleta de item

                if (estado != ESTADO_JOGANDO) {
                    break;
                }

                // Troca de sala ao pisar em porta
                //mudou pouca coisa, ele pega o "centro" aproximado do jogador (jogador->pos.x/.y) e verifica se ta numa tile que seria a porta
                if (sala->grid[jogador->pos.y][jogador->pos.x].tipo == TILE_PORTA) {
                    Direcao dir    = sala_direcao_porta(sala, jogador->pos.x, jogador->pos.y);
                    const char* proxima = sala_saida_em(sala, jogador->pos.x, jogador->pos.y);
                    if (proxima) {
                        Sala* nova = sala_obter(proxima);
                        if (nova) {
                            int hp_anterior = jogador->hp; //guarda a vida antes de destruir a lista da sala antiga

                            if (entidades) { //limpa as entidades da sala q vai ser descarregada pra n quebrar a rebimboca da parafuseta, mesmo comentario se aplica pra todas as vezes q isso aparecer
                                lista_limpar(entidades);
                            }
                            sala = nova;

                            entidades = lista_criar(); //cria novas entidades na sala nova
                            lista_spawnar_sala(entidades, sala, &jogador);

                            jogador->hp = hp_anterior; //mantém a vida do jogador ao trocar de sala
                            posicionar_jogador_apos_porta(sala, jogador, dir); //evita q o bichinho apareça dentro da parede
                        }
                    }
                }

                // Camera trava por sala
                int cameraX = (jogador->pos.x / CAMERA_LARGURA) * CAMERA_LARGURA;
                int cameraY = (jogador->pos.y / CAMERA_ALTURA)  * CAMERA_ALTURA;
                if (cameraX > sala->largura  - CAMERA_LARGURA) {
                    cameraX = sala->largura  - CAMERA_LARGURA;
                }
                if (cameraY > sala->altura - CAMERA_ALTURA)  {
                    cameraY = sala->altura - CAMERA_ALTURA;
                }
                
                // Renderizacao
                BeginDrawing();
                    ClearBackground(DARKGRAY);
                    sala_desenhar(sala, cameraX, cameraY, TAMANHO_TILE, CAMERA_LARGURA, CAMERA_ALTURA);
                    lista_desenhar(entidades, sala, cameraX, cameraY, TAMANHO_TILE);
                EndDrawing();
                break;
            }

            case ESTADO_PAUSE:
                tela_pause_atualizar(&estado);
                BeginDrawing();
                tela_pause_desenhar(
                    sala ? sala->arquivo : NULL,
                    jogador ? jogador->hp : 0,
                    score,
                    player_oxigenio
                );
                EndDrawing();
                break;

            case ESTADO_RANKING:
                tela_ranking_atualizar(&estado);
                BeginDrawing();
                tela_ranking_desenhar(ranking, ranking_count);
                EndDrawing();
                break;

            case ESTADO_GAME_OVER: {
                int c = GetCharPressed();
                while (c > 0) {
                    if (c >= 32 && c < 127 && nome_input_len < 15) {
                        nome_input[nome_input_len++] = (char)c;
                        nome_input[nome_input_len] = '\0';
                    }
                    c = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && nome_input_len > 0) {
                    nome_input[--nome_input_len] = '\0';
                }

                if (IsKeyPressed(KEY_ENTER) && nome_input_len > 0) {
                    ranking_adicionar(ranking, &ranking_count, nome_input, score);
                    ranking_salvar(ranking, ranking_count);

                    if (entidades) {
                        lista_limpar(entidades);
                        entidades = NULL;
                    }
                    sala_cache_limpar();
                    sala = NULL;
                    jogador = NULL;
                    bloquear_enter_menu = true;
                    sistema_transicao(&estado, ESTADO_MENU);
                }

                char buf[64];
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("GAME OVER", 260, 180, 40, RED);
                snprintf(buf, sizeof(buf), "Pontuacao: %d", score);
                DrawText(buf, 260, 240, 20, WHITE);
                DrawText("Digite seu nome:", 260, 290, 20, GRAY);
                DrawText(nome_input, 260, 320, 24, YELLOW);
                DrawText("ENTER para confirmar", 260, 380, 15, GRAY);
                EndDrawing();
                break;
            }
                
            default:
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
                break;
        }
    }
    if (entidades) {
        lista_limpar(entidades);
    }
    sala_cache_limpar();
    menu_finalizar();
    CloseWindow();
    return 0;
}
