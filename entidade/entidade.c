#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "entidade.h"

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


void lista_adicionar(ListaEntidades* lista, Entidade* entidade) { //adiciona uma entidade no INÍCIO da lista. EX.: se antes era A->B->NULL, agora vira N->A->B->NULL
    if (lista == NULL || entidade == NULL) { //lenguisa B-)
        return;
    }

    entidade->next = lista->head;
    lista->head = entidade;

    lista->contagem++; //debug, aumenta o contador de entidades na lista
}

void lista_remover(ListaEntidades* lista, Entidade* alvo) { //remove uma entidade em qualquer lugar da lista e da free naquele espaço de memória
    if (lista == NULL || alvo == NULL) { //sarsissa 8-)
        return;
    }

    Entidade* atual = lista->head;
    Entidade* anterior = NULL; //preciso do anterior pra quando der free, ele apontar pro que vinha depois da entidade removida. Começa como NULL pq não tem anterior ainda

    while (atual != NULL) {
        if (atual == alvo) {
            if (anterior == NULL) { //pro caso do atual ser o primeiro item da lista (anterior não existe (NULL))
                lista->head = atual->next;
            }
            else { 
                anterior->next = atual->next; //anterior agora aponta pra casa depois do atual, "pulando" a casa atual
            }

            free(atual); //atual desaparece pra sempre
            lista->contagem--; //debug, reduz o contador de entidades da lista

            return;
        }

        anterior = atual;
        atual = atual->next;
    }
}

void lista_limpar(ListaEntidades* lista) { //apaga a lista e libera o espaço alocado pra ela, muito mais simples que lista_remover
    if (lista == NULL) { //çalame :S
        return;
    }

    Entidade* atual = lista->head; //atual aponta pro primeiro item da lista

    while (atual != NULL) { //enquanto atual não for vazio, ele continua dando free no atual e passando pra próxima casa
        Entidade* proximo = atual->next;

        free(atual);

        atual = proximo;
    }

    lista->head = NULL;
    lista->contagem = 0;

    free(lista);
}