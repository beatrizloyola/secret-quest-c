#include "raylib.h"
#include <stddef.h>
#include "types.h"
#include "mapa/mapa.h"
#include "entidade/entidade.h"
#include "sistema/sistema.h"

#define TAMANHO_TILE  80
#define CAMERA_LARGURA 12
#define CAMERA_ALTURA  10

/*
Mudanças levemente drásticas no main.c pra permitir que o jogador e os inimigos se movam de maneira fluida, 
além de integrar todo o sistema de lista encadeada e as trocentas funções q eu escrevi.
Se tem um trecho q eu apaguei é pq tem uma função no entidades.c que faz a mesma coisa so q mais dificil.

se eu quebrei alguma outra coisa q ja tava funcionando peço perdao
*/


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
                        entidades = lista_criar();
                        lista_spawnar_sala(entidades, sala, &jogador);
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
                    entidade_atacar(jogador, entidades, sala, 3, &score);
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
                        Sala* nova = sala_carregar(proxima);
                        if (nova) {
                            if (entidades) { //limpa as entidades da sala q vai ser descarregada pra n quebrar a rebimboca da parafuseta, mesmo comentario se aplica pra todas as vezes q isso aparecer
                                lista_limpar(entidades); 
                            }
                            sala_descarregar(sala);
                            sala = nova;

                            entidades = lista_criar(); //cria novas entidades na sala nova
                            lista_spawnar_sala(entidades, sala, &jogador);

                            switch (dir) { //mudou pouca coisa tbm, só usa o jogador->pos.x/.y
                                case DIR_CIMA:     jogador->pos.y = sala->altura - 2; break;
                                case DIR_BAIXO:    jogador->pos.y = 1;                break;
                                case DIR_DIREITA:  jogador->pos.x = 1;                break;
                                case DIR_ESQUERDA: jogador->pos.x = sala->largura - 2; break;
                            }

                            jogador->x = jogador->pos.x + 0.5f; //sincroniza posição real no centro da tile depois de trocar de sala, sem isso ele aparece no canto da tile (sla pq)
                            jogador->y = jogador->pos.y + 0.5f; //ler comentario acima
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
                    lista_desenhar(entidades, cameraX, cameraY, TAMANHO_TILE);
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
                tela_ranking_desenhar();
                EndDrawing();
                break;

            case ESTADO_GAME_OVER:
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("GAME OVER", 260, 250, 40, RED);
                DrawText("Pressione ENTER para voltar ao menu", 250, 320, 20, WHITE);
                EndDrawing();

                if (IsKeyPressed(KEY_ENTER)) {
                    if (entidades) {
                        lista_limpar(entidades);
                        entidades = NULL;
                    }

                    if (sala) {
                        sala_descarregar(sala);
                        sala = NULL;
                    }

                    jogador = NULL;
                    sistema_transicao(&estado, ESTADO_MENU);
                }
                break;
                
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
    if (sala) {
        sala_descarregar(sala);
    }
    menu_finalizar();
    CloseWindow();
    return 0;
}
