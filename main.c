#include "raylib.h"
#include <stdio.h>

#define LARGURA 48
#define ALTURA 40

#define TAMANHO_TILE 80 // Quanto maior o tamanho do tile, maior a resolução do jogo
#define CAMERA_LARGURA 12
#define CAMERA_ALTURA 10

int LINHA_COMPLETA[LARGURA] = {
    [0 ... 47] = 1
};

int LINHA_CORREDOR[LARGURA] = {
    1,1,0,0,0,0,0,0,0,0, 1,1,1,1,0,0,0,0,0,0,0,0,
    1,1,1,1,0,0,0,0,0,0,0,0,
    1,1,1,1,0,0,0,0,0,0,0,0,1,1
};

int LINHA_ABERTA[LARGURA] = {
    1,1,
    [2 ... 45] = 0,
    1,1
};

int LINHA_ESPECIAL_A[LARGURA] = {
    [0 ... 47] = 1
};

void init_especial_a() {
    LINHA_ESPECIAL_A[41] = 0;
    LINHA_ESPECIAL_A[42] = 0;
}

int LINHA_ESPECIAL_B[LARGURA] = {
    1,1,1,1,1,0,0,1,1,1,
    1,1,1,1,1,1,1,0,0,1,
    1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,
    1,0,0,1,1,1,1,1
};

int* get_linha(int y) {
    if (y == 0 || y == 39) return LINHA_COMPLETA;
    if (y >= 1 && y <= 3) return LINHA_CORREDOR;
    if (y >= 4 && y <= 5) return LINHA_ABERTA;
    if (y >= 6 && y <= 8) return LINHA_CORREDOR;
    if (y == 9 || y == 10) return LINHA_ESPECIAL_A;
    if (y >= 11 && y <= 18) return LINHA_CORREDOR;
    if (y == 19 || y == 20) return LINHA_ESPECIAL_B;
    if (y >= 21 && y <= 28) return LINHA_CORREDOR;
    if (y == 29 || y == 30) return LINHA_ESPECIAL_B;
    if (y >= 31 && y <= 33) return LINHA_CORREDOR;
    if (y >= 34 && y <= 35) return LINHA_ABERTA;
    if (y >= 36 && y <= 38) return LINHA_CORREDOR;

    return LINHA_COMPLETA;
}

void gerar_mapa(int mapa[LARGURA][ALTURA]){
    for (int y = 0; y < ALTURA; y++){
        int *linha = get_linha(y);

        for (int x = 0; x < LARGURA; x++){
            mapa[x][y] = linha[x];
        }
    }
}

void desenhar_mapa(int mapa[LARGURA][ALTURA], int cameraX, int cameraY) {
    for (int y = 0; y < CAMERA_ALTURA; y++) {
        int mapaY = cameraY + y;

        for (int x = 0; x < CAMERA_LARGURA; x++) {
            int mapaX = cameraX + x;

            if (mapaX >= LARGURA || mapaY >= ALTURA) continue;

            int tile = mapa[mapaX][mapaY];

            Color cor = (tile == 1) ? YELLOW : BLACK;

            DrawRectangle(
                x * TAMANHO_TILE,
                y * TAMANHO_TILE,
                TAMANHO_TILE,
                TAMANHO_TILE,
                cor
            );
        }
    }
}

int main(void) {
    InitWindow(CAMERA_LARGURA * TAMANHO_TILE, CAMERA_ALTURA * TAMANHO_TILE, "Secret Quest");

    SetTargetFPS(60);

    init_especial_a();

    int playerX = 5;
    int playerY = 5;

    int mapa[LARGURA][ALTURA];
    gerar_mapa(mapa);

    while (!WindowShouldClose()) {
        
        // Mapa
        if (playerX < 0) playerX = 0;
        if (playerY < 0) playerY = 0;
        if (playerX >= LARGURA) playerX = LARGURA - 1;
        if (playerY >= ALTURA) playerY = ALTURA - 1;

        // Input
        int movimentoX = 0;
        int movimentoY = 0;

        if (IsKeyPressed(KEY_RIGHT)) movimentoX++;
        if (IsKeyPressed(KEY_LEFT))  movimentoX--;
        if (IsKeyPressed(KEY_DOWN))  movimentoY++;
        if (IsKeyPressed(KEY_UP))    movimentoY--;

        if (mapa[playerX + movimentoX][playerY + movimentoY] == 0){
            playerX += movimentoX;
            playerY += movimentoY;
        }

        // Câmera presa na sala
        int cameraX = (playerX / CAMERA_LARGURA) * CAMERA_LARGURA;
        int cameraY = (playerY / CAMERA_ALTURA) * CAMERA_ALTURA;
        if (cameraX > LARGURA - CAMERA_LARGURA) cameraX = LARGURA - CAMERA_LARGURA;
        if (cameraY > ALTURA - CAMERA_ALTURA) cameraY = ALTURA - CAMERA_ALTURA;

        // Renderização
        BeginDrawing();

            ClearBackground(DARKGRAY);
            desenhar_mapa(mapa, cameraX, cameraY);
            DrawRectangle( // Player
                (playerX - cameraX) * TAMANHO_TILE,
                (playerY - cameraY) * TAMANHO_TILE,
                TAMANHO_TILE,
                TAMANHO_TILE,
                BLUE
            );

        EndDrawing();
    }

    CloseWindow();
    return 0;
}