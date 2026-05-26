#include "mapa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Sala* sala_carregar(const char* arquivo) {
    FILE* f = fopen(arquivo, "r");
    if (!f) return NULL;

    Sala* s = malloc(sizeof(Sala));
    strncpy(s->arquivo, arquivo, 63);
    s->arquivo[63] = '\0';
    s->spawn_jogador.x = 2;
    s->spawn_jogador.y = 4;

    fscanf(f, "%d %d\n", &s->largura, &s->altura);

    s->grid = malloc(s->altura * sizeof(Tile*));
    for (int y = 0; y < s->altura; y++) {
        s->grid[y] = malloc(s->largura * sizeof(Tile));

        for (int x = 0; x < s->largura; x++) {
            int c = fgetc(f);
            while (c == '\r') c = fgetc(f);

            TileType tipo = (c == '#') ? TILE_PAREDE : TILE_CHAO;
            s->grid[y][x].tipo   = tipo;
            s->grid[y][x].solido = (tipo == TILE_PAREDE);
        }
        fgetc(f); // '\n'
    }

    fclose(f);
    return s;
}

void sala_descarregar(Sala* s) {
    if (!s) return;
    for (int y = 0; y < s->altura; y++)
        free(s->grid[y]);
    free(s->grid);
    free(s);
}

bool sala_colisao(Sala* s, int x, int y) {
    if (x < 0 || y < 0 || x >= s->largura || y >= s->altura) return true;
    return s->grid[y][x].solido;
}

void sala_desenhar(Sala* s, int cam_x, int cam_y, int tamanho_tile, int cam_larg, int cam_alt) {
    for (int y = 0; y < cam_alt; y++) {
        int my = cam_y + y;
        for (int x = 0; x < cam_larg; x++) {
            int mx = cam_x + x;
            if (mx >= s->largura || my >= s->altura) continue;

            Color cor = s->grid[my][mx].solido ? YELLOW : BLACK;
            DrawRectangle(x * tamanho_tile, y * tamanho_tile, tamanho_tile, tamanho_tile, cor);
        }
    }
}
