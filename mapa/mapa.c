#include "mapa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Sala* sala_carregar(const char* arquivo) {
    FILE* f = fopen(arquivo, "r"); // Abre o arquivo
    if (!f) return NULL; // Se não achar, cabou

    Sala* s = malloc(sizeof(Sala)); // Aloca espaço pra sala
    strncpy(s->arquivo, arquivo, 63); // Copia arquivo para s->arquivo
    s->arquivo[63] = '\0'; // Fim do string
    s->spawn_jogador.x = 0;
    s->spawn_jogador.y = 0;
    s->num_inimigos = 0; // Qtd inicial
    s->num_itens = 0;

    fscanf(f, "%d %d\n", &s->largura, &s->altura); // Lê a primeira linha do txt para saber largura e altura da sala

    s->grid = malloc(s->altura * sizeof(Tile*)); // Aloca uma altura por linha do mapa
    for (int y = 0; y < s->altura; y++) {
        s->grid[y] = malloc(s->largura * sizeof(Tile)); // Aloca uma largura por linha do mapa

        // Ignora o \r que o Windows coloca no final do arquivo
        for (int x = 0; x < s->largura; x++) {
            int c = fgetc(f);
            while (c == '\r') c = fgetc(f);

            TileType tipo;
            switch (c) {
                case '#': tipo = TILE_PAREDE; break;
                default:  tipo = TILE_CHAO;   break;
            }
            s->grid[y][x].tipo   = tipo;
            s->grid[y][x].solido = (tipo == TILE_PAREDE);

            if (c == 'S') { // Salva spawn de jogador
                s->spawn_jogador.x = x;
                s->spawn_jogador.y = y;
            } else if (c == 'E' && s->num_inimigos < 16) { // Spawna um inimigo se já não tiver no cap
                s->spawn_inimigos[s->num_inimigos].x = x;
                s->spawn_inimigos[s->num_inimigos].y = y;
                s->num_inimigos++;
            } else if (c == 'I' && s->num_itens < 16) { // Spawna um item se já não tiver no cap
                s->spawn_itens[s->num_itens].x = x;
                s->spawn_itens[s->num_itens].y = y;
                s->num_itens++;
            }
        }
        fgetc(f); // '\n'
    }

    fclose(f); // Fecha o arquivo
    return s; // Retorna a sala
}

void sala_descarregar(Sala* s) {
    if (!s) return;
    for (int y = 0; y < s->altura; y++)
        free(s->grid[y]); // Libera tile a tile
    free(s->grid); // Libera grade
    free(s); // Libera sala
}

bool sala_colisao(Sala* s, int x, int y) { // Não deixa o jogador escapar dos limites da sala
    if (x < 0 || y < 0 || x >= s->largura || y >= s->altura) return true;
    return s->grid[y][x].solido;
}

void sala_desenhar(Sala* s, int cam_x, int cam_y, int tamanho_tile, int cam_larg, int cam_alt) {
    for (int y = 0; y < cam_alt; y++) { // Mexe a câmera quando muda de sala
        int my = cam_y + y;
        for (int x = 0; x < cam_larg; x++) {
            int mx = cam_x + x;
            if (mx >= s->largura || my >= s->altura) continue;

            Color cor = s->grid[my][mx].solido ? YELLOW : BLACK; // Se for parede é amarelo, se for chão é preto
            DrawRectangle(x * tamanho_tile, y * tamanho_tile, tamanho_tile, tamanho_tile, cor);
        }
    }
}
