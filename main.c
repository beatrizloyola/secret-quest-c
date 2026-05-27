#include "raylib.h"
#include "types.h"
#include "mapa/mapa.h"
#include "entidade/entidade.h"

#define TAMANHO_TILE  80
#define CAMERA_LARGURA 12
#define CAMERA_ALTURA  10

int main(void) {
    InitWindow(CAMERA_LARGURA * TAMANHO_TILE, CAMERA_ALTURA * TAMANHO_TILE, "Secret Quest");
    SetTargetFPS(60);

    Sala* sala = sala_carregar("mapa/mapa.txt");

    int playerX = sala->spawn_jogador.x;
    int playerY = sala->spawn_jogador.y;

    while (!WindowShouldClose()) {

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

        // Câmera trava por sala
        int cameraX = (playerX / CAMERA_LARGURA) * CAMERA_LARGURA;
        int cameraY = (playerY / CAMERA_ALTURA)  * CAMERA_ALTURA;
        if (cameraX > sala->largura  - CAMERA_LARGURA) cameraX = sala->largura  - CAMERA_LARGURA;
        if (cameraY > sala->altura - CAMERA_ALTURA)  cameraY = sala->altura - CAMERA_ALTURA;

        // Renderização
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
    }

    sala_descarregar(sala);
    CloseWindow();
    return 0;
}
