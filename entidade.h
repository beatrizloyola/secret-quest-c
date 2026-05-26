#ifndef ENTIDADE_H
#define ENTIDADE_H

#include "raylib.h"
#include "types.h"

ListaEntidades* lista_criar(); //cria a lista de entidades

void lista_adicionar(ListaEntidades* lista, Entidade* e); //insere entidade na lista

void lista_remover(ListaEntidades* lista, Entidade* alvo); //remove entidade da lista e da free no espaço alocado pra ela

void lista_limpar(ListaEntidades* lista); //free na lista inteira

//void lista_atualizar(ListaEntidades* lista, Sala* sala, float dt); //TBA

//void lista_desenhar(ListaEntidades* lista, int cam_x, int cam_y, Texture2D* sprites); //TBA

Entidade* entidade_criar(EntTipo tipo, int x, int y); //cria uma entidade

//void entidade_atacar(Entidade* atacante, ListaEntidades* alvos, Sala* sala); //TBA

#endif