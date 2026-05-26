#include <stdio.h>
#include <raylib.h>
#include <entidade.h>

ListaEntidades* lista_criar(void) { //criação de lista encadeada pras entidades
    ListaEntidades* lista = malloc(sizeof(ListaEntidades));

    if (lista == NULL) {
        return NULL;
    }

    lista->head = NULL;
    lista->contagem = 0; //debug

    return lista;
}

Entidade* entidade_criar(EntTipo tipo, int x, int y) { //função de criar entidade, pega tipo e posição
    Entidade* entidade = malloc(sizeof(Entidade));

    if (entidade == NULL) {
        return NULL;
    }

    entidade->tipo = tipo;

    entidade->pos.x = x;
    entidade->pos.y = y;

    entidade->vivo = true;          //booleano que vai ser setado pra false caso a entidade "morra" (item coletado ou hp<=0)

    entidade->direcao = DIR_BAIXO;  // só pra padronizar, ela spawna "olhando" pra baixo

    entidade->timer_ataque = 0.0f;  //

    entidade->next = NULL;

    if (tipo == ENT_JOGADOR) {      //se o tipo for jogador:
        entidade->max_hp = 10;
        entidade->hp = 10;
        entidade->ataque = 2;
    }
    else if (tipo == ENT_INIMIGO) { //se nao, se o tipo for inimigo:
        entidade->max_hp = 3;
        entidade->hp = 3;
        entidade->ataque = 1;
    }
    else {                          //se nao, o tipo é item:
        entidade->max_hp = 1;
        entidade->hp = 1;
        entidade->ataque = 0;
    }

    return entidade;
}
