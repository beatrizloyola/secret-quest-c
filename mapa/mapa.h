#ifndef MAPA_H
#define MAPA_H

#include "../types.h"
#include "raylib.h"

Sala*        sala_carregar(const char* arquivo);
void         sala_descarregar(Sala* s);
bool         sala_colisao(Sala* s, int x, int y);
void         sala_desenhar(Sala* s, int cam_x, int cam_y, int tamanho_tile, int cam_larg, int cam_alt);
Direcao      sala_direcao_porta(Sala* s, int x, int y);
const char*  sala_saida_em(Sala* s, int x, int y);

#endif
