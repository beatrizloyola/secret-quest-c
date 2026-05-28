#ifndef ENTIDADE_H
#define ENTIDADE_H

#include "raylib.h"
#include "../types.h"

ListaEntidades* lista_criar(); //cria a lista de entidades

void lista_adicionar(ListaEntidades* lista, Entidade* e); //insere entidade na lista

void lista_remover(ListaEntidades* lista, Entidade* alvo); //remove entidade da lista e da free no espaço alocado pra ela

void lista_limpar(ListaEntidades* lista); //free na lista inteira

void lista_spawnar_sala(ListaEntidades* lista, Sala* sala, Entidade** jogador); //cria jogador, inimigos e itens a partir dos spawns da sala

void entidade_mover_jogador(Entidade* jogador, Sala* sala, float dt); //movimenta o jogador de forma fluida usando posição fracionada

void lista_atualizar(ListaEntidades* lista, Sala* sala, Entidade* jogador, float dt, int* score, EstadoJogo* estado); //atualiza inimigos, cooldowns, dano por contato e coleta de item

void lista_desenhar(ListaEntidades* lista, Sala* sala, int cam_x, int cam_y, int tamanho_tile); //desenha todas as entidades da lista e a hitbox de ataque quando estiver ativa

Entidade* entidade_criar(EntTipo tipo, int x, int y); //cria uma entidade

void entidade_atacar(Entidade* atacante, ListaEntidades* alvos, Sala* sala, int alcance, int* score); //ataca até 3 tiles na direção que a entidade tá olhando

#endif