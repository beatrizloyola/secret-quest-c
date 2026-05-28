#include "raylib.h"
#include <stddef.h>
#include "types.h"
#include "mapa/mapa.h"
#include "entidade/entidade.h"
#include "sistema/sistema.h"

#define TAMANHO_TILE  80
#define CAMERA_LARGURA 12
#define CAMERA_ALTURA  10

int main(void) {
    InitWindow(CAMERA_LARGURA * TAMANHO_TILE, CAMERA_ALTURA * TAMANHO_TILE, "Secret Quest");
    SetTargetFPS(60);

    menu_inicializar();
    EstadoJogo estado = ESTADO_MENU;
    Sala* sala = NULL;
    int playerX = 0, playerY = 0;
    int score = 0;
    int player_hp = 10;        // placeholder para integracao com lista de entidades
    int player_oxigenio = 100; // placeholder
    int player_energia = 100; //placeholder


    while (!WindowShouldClose()) {

        switch (estado) {

            case ESTADO_MENU:
                menu_atualizar(&estado);
                BeginDrawing();
                menu_desenhar();
                EndDrawing();
                break;

            case ESTADO_JOGANDO: {
                if (sala == NULL) {
                    sala = sala_carregar("mapa/salas/sala1.txt");
                    if (sala) {
                        playerX = sala->spawn_jogador.x;
                        playerY = sala->spawn_jogador.y;
                    }
                }

                if (!sala) break;

                if (IsKeyPressed(KEY_ESCAPE)) {
                    sistema_transicao(&estado, ESTADO_PAUSE);
                    break;
                }

                // Limites do mapa
                if (playerX < 0) playerX = 0;
                if (playerY < 0) playerY = 0;
                if (playerX >= sala->largura)  playerX = sala->largura  - 1;
                if (playerY >= sala->altura) playerY = sala->altura - 1;

                // Input
                int mx = 0, my = 0;
                if (IsKeyPressed(KEY_RIGHT)) mx++;
                if (IsKeyPressed(KEY_LEFT))  mx--;
                if (IsKeyPressed(KEY_DOWN))  my++;
                if (IsKeyPressed(KEY_UP))    my--;

                if (!sala_colisao(sala, playerX + mx, playerY + my)) {
                    playerX += mx;
                    playerY += my;
                }

                // Troca de sala ao pisar em porta
                if (sala->grid[playerY][playerX].tipo == TILE_PORTA) {
                    Direcao dir    = sala_direcao_porta(sala, playerX, playerY);
                    const char* proxima = sala_saida_em(sala, playerX, playerY);
                    if (proxima) {
                        Sala* nova = sala_carregar(proxima);
                        if (nova) {
                            sala_descarregar(sala);
                            sala = nova;
                            switch (dir) {
                                case DIR_CIMA:     playerY = sala->altura - 2; break;
                                case DIR_BAIXO:    playerY = 1;                break;
                                case DIR_DIREITA:  playerX = 1;                break;
                                case DIR_ESQUERDA: playerX = sala->largura - 2; break;
                            }
                        }
                    }
                }

                // Camera trava por sala
                int cameraX = (playerX / CAMERA_LARGURA) * CAMERA_LARGURA;
                int cameraY = (playerY / CAMERA_ALTURA)  * CAMERA_ALTURA;
                if (cameraX > sala->largura  - CAMERA_LARGURA) cameraX = sala->largura  - CAMERA_LARGURA;
                if (cameraY > sala->altura - CAMERA_ALTURA)  cameraY = sala->altura - CAMERA_ALTURA;
                
                // Renderizacao
                BeginDrawing();
                    ClearBackground(DARKGRAY);
                    sala_desenhar(sala, cameraX, cameraY, TAMANHO_TILE, CAMERA_LARGURA, CAMERA_ALTURA);
                    DrawRectangle(
                        (playerX - cameraX) * TAMANHO_TILE,
                        (playerY - cameraY) * TAMANHO_TILE,
                        TAMANHO_TILE, TAMANHO_TILE,
                        BLUE
                    );
                EndDrawing();
                break;
            }

            case ESTADO_PAUSE:
                tela_pause_atualizar(&estado);
                BeginDrawing();
                tela_pause_desenhar(
                    sala ? sala->arquivo : NULL,
                    player_hp,
                    score,
                    player_oxigenio
                );
                EndDrawing();
                break;

            case ESTADO_RANKING:
                tela_ranking_atualizar(&estado);
                BeginDrawing();
                tela_ranking_desenhar();
                EndDrawing();
                break;
                
            default:
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
                break;
        }
    }
    if (sala) sala_descarregar(sala);
    menu_finalizar();
    CloseWindow();
    return 0;
}