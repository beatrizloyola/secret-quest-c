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
    s->num_inimigos = 0; // Qtd inicial
    s->num_itens = 0;
    s->num_fragmentos = 0; // fragmentos do código espalhados pela sala
    memset(s->inimigos_mortos, 0, sizeof(s->inimigos_mortos));

    fscanf(f, "%d %d", &s->largura, &s->altura);

    s->saida_norte[0] = '\0';
    s->saida_sul[0]   = '\0';
    s->saida_leste[0] = '\0';
    s->saida_oeste[0] = '\0';

    char linha_saidas[256];
    fgets(linha_saidas, sizeof(linha_saidas), f); // Consume resto da linha 1

    // Formato da 2a linha: "N:arquivo S:arquivo L:arquivo O:arquivo". Campo vazio = sem saída nessa direção
    if (fgets(linha_saidas, sizeof(linha_saidas), f)) {
        char *tok = strtok(linha_saidas, " \t\r\n");
        while (tok) {
            if (tok[1] == ':') {
                char *val = tok + 2;
                if (tok[0] == 'N') strncpy(s->saida_norte, val, 63);
                else if (tok[0] == 'S') strncpy(s->saida_sul,   val, 63);
                else if (tok[0] == 'L') strncpy(s->saida_leste, val, 63);
                else if (tok[0] == 'O') strncpy(s->saida_oeste, val, 63);
            }
            tok = strtok(NULL, " \t\r\n");
        }
    }

    s->grid = malloc(s->altura * sizeof(Tile*)); // Aloca uma altura por linha do mapa
    for (int y = 0; y < s->altura; y++) {
        s->grid[y] = malloc(s->largura * sizeof(Tile)); // Aloca uma largura por linha do mapa

        for (int x = 0; x < s->largura; x++) {
            int c = fgetc(f);
            while (c == '\r' || c == '\n') c = fgetc(f); // Ignora o \r e \n que o Windows coloca no final do arquivo

            TileType tipo;
            switch (c) {
                case '#': tipo = TILE_PAREDE;   break; // # vira parede
                case 'D': tipo = TILE_PORTA;    break; // D vira porta
                case 'T': tipo = TILE_TERMINAL; break; // T vira terminal de autodestruição
                case 'X': tipo = TILE_SAIDA;    break; // X vira saída final da estação
                default:  tipo = TILE_CHAO;     break; // O resto é chão (usei .)
            }
            s->grid[y][x].tipo   = tipo;
            s->grid[y][x].solido = (tipo == TILE_PAREDE); // só parede é sólido; terminal e saída são chão

            if (c == 'S') { // Salva spawn de jogador
                s->spawn_jogador.x = x;
                s->spawn_jogador.y = y;
            } else if (c == 'E' && s->num_inimigos < 16) { // Spawna um inimigo se já não tiver no cap
                s->spawn_inimigos[s->num_inimigos].x = x;
                s->spawn_inimigos[s->num_inimigos].y = y;
                s->num_inimigos++; // Salva posição do inimigo e passa pro pŕoximo índice
            } else if (c == 'I' && s->num_itens < 16) { // Spawna um item se já não tiver no cap
                s->spawn_itens[s->num_itens].x = x;
                s->spawn_itens[s->num_itens].y = y;
                s->num_itens++; // Salva a posição do item e passa pro próximo índice
            } else if (c >= 'a' && c <= 'd' && s->num_fragmentos < 4) { // a/b/c/d são fragmentos 0/1/2/3 do código
                s->spawn_fragmentos[s->num_fragmentos].x = x;
                s->spawn_fragmentos[s->num_fragmentos].y = y;
                s->fragmento_ids[s->num_fragmentos]      = c - 'a'; // 'a'→0, 'b'→1, 'c'→2, 'd'→3
                s->num_fragmentos++;
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

Direcao sala_direcao_porta(Sala* s, int x, int y) {
    // Calcula distância da porta até as bordas (assim não precisa explicitar se ela é norte, sul, leste ou oeste)
    int dist_norte = y;
    int dist_sul   = s->altura  - 1 - y;
    int dist_oeste = x;
    int dist_leste = s->largura - 1 - x;

    int min = dist_norte;
    Direcao dir = DIR_CIMA;
    if (dist_sul   < min) { min = dist_sul;   dir = DIR_BAIXO;    }
    if (dist_oeste < min) { min = dist_oeste; dir = DIR_ESQUERDA; }
    if (dist_leste < min) {                   dir = DIR_DIREITA;  }
    return dir;
}

const char* sala_saida_em(Sala* s, int x, int y) {
    switch (sala_direcao_porta(s, x, y)) {
        // Retorna o caminho da sala destino (se não tiver é nulo)
        case DIR_CIMA:     return s->saida_norte[0] ? s->saida_norte : NULL;
        case DIR_BAIXO:    return s->saida_sul[0]   ? s->saida_sul   : NULL;
        case DIR_ESQUERDA: return s->saida_oeste[0] ? s->saida_oeste : NULL;
        case DIR_DIREITA:  return s->saida_leste[0] ? s->saida_leste : NULL;
    }
    return NULL;
}

void sala_desenhar(Sala* s, int cam_x, int cam_y, int tamanho_tile, int cam_larg, int cam_alt) {
    for (int y = 0; y < cam_alt; y++) { // Desenha os tiles visíveis dentro da área da câmera
        int my = cam_y + y;
        for (int x = 0; x < cam_larg; x++) {
            int mx = cam_x + x;
            if (mx >= s->largura || my >= s->altura) continue;

            Color cor;
            switch (s->grid[my][mx].tipo) { // cada tipo de tile tem sua cor
                case TILE_PAREDE:   cor = YELLOW;   break;
                case TILE_PORTA:    cor = BLACK;    break;
                case TILE_TERMINAL: cor = SKYBLUE;  break; // terminal brilha azul
                case TILE_SAIDA:    cor = MAGENTA;  break; // saída brilha rosa
                default:            cor = BLACK;    break; // chão é preto
            }
            DrawRectangle(x * tamanho_tile, y * tamanho_tile, tamanho_tile, tamanho_tile, cor);
        }
    }
}
