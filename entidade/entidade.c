#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "entidade.h"
#include "../mapa/mapa.h"

//As funções à seguir usam static pq elas só são usadas nesse arquivo aqui, não preciso chamar elas em lugar nenhum depois disso

static float valor_absoluto(float valor) { //retorna o valor positivo, usado pra medir distância entre as hitboxes
    if (valor < 0.0f) {
        return -valor;
    }

    return valor;
}

static float entidade_raio(Entidade* entidade) { //define o tamanho da hitbox da entidade em tiles
    if (entidade == NULL) { //"entrarao no meu codigo e lerao meus comentarios" - napoleao bonaparte
        return 0.0f;
    }

    if (entidade->tipo == ENT_ITEM) { //item tem hitbox menor pra parecer coletável
        return 0.22f;
    }

    return 0.32f; //jogador e inimigo ocupam um pouco menos que 1 tile
}

static void entidade_sincronizar_tile(Entidade* entidade) { //atualiza a posição inteira em tile usando o centro real da entidade
    if (entidade == NULL) { //macarram :V
        return;
    }

    entidade->pos.x = (int)entidade->x;
    entidade->pos.y = (int)entidade->y;
}

static int distancia_manhattan(Entidade* a, Entidade* b) { //calcula distância em tiles sem diagonal
    int dx = a->pos.x - b->pos.x;
    int dy = a->pos.y - b->pos.y;

    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    return dx + dy;
}

static bool entidades_encostando(Entidade* a, Entidade* b) { //verifica contato usando hitbox simples de retângulo
    if (a == NULL || b == NULL) { //onde fica o ´ em révéíllón?
        return false;
    }

    float soma_raios = entidade_raio(a) + entidade_raio(b);
    float dx = valor_absoluto(a->x - b->x);
    float dy = valor_absoluto(a->y - b->y);

    return dx <= soma_raios && dy <= soma_raios;
}

static bool entidade_colide_mapa(Entidade* entidade, Sala* sala, float novo_x, float novo_y, bool bloquear_porta) { //testa se a hitbox da entidade bate no mapa
    if (entidade == NULL || sala == NULL) { //早上好中国 现在我有冰淇淋🍦我很喜欢冰淇淋🍦
        return true;
    }

    float raio = entidade_raio(entidade);

    float esquerda_real = novo_x - raio;
    float direita_real = novo_x + raio;
    float cima_real = novo_y - raio;
    float baixo_real = novo_y + raio;

    if (esquerda_real < 0.0f || cima_real < 0.0f || direita_real >= sala->largura || baixo_real >= sala->altura) {
        return true;
    }

    int esquerda = (int)esquerda_real;
    int direita = (int)direita_real;
    int cima = (int)cima_real;
    int baixo = (int)baixo_real;

    for (int y = cima; y <= baixo; y++) { //testa todos os tiles tocados pela hitbox
        for (int x = esquerda; x <= direita; x++) {
            if (sala_colisao(sala, x, y)) { //parede bloqueia qualquer entidade
                return true;
            }

            if (bloquear_porta && sala->grid[y][x].tipo == TILE_PORTA) { //inimigo não atravessa porta sozinho
                return true;
            }
        }
    }

    return false;
}

static void direcao_para_delta(Direcao direcao, int* dx, int* dy) { //converte direção em movimento x/y
    *dx = 0;
    *dy = 0;

    switch (direcao) {
        case DIR_CIMA:     *dy = -1; break;
        case DIR_BAIXO:    *dy = 1;  break;
        case DIR_ESQUERDA: *dx = -1; break;
        case DIR_DIREITA:  *dx = 1;  break;
    }
}

static Direcao inverter_direcao(Direcao direcao) { //inverte a direção quando o inimigo bate em parede ou porta
    switch (direcao) {
        case DIR_CIMA:     return DIR_BAIXO;
        case DIR_BAIXO:    return DIR_CIMA;
        case DIR_ESQUERDA: return DIR_DIREITA;
        case DIR_DIREITA:  return DIR_ESQUERDA;
    }

    return DIR_DIREITA;
}

static bool tile_dentro_ataque(Entidade* atacante, Sala* sala, int tile_x, int tile_y) { //verifica se um tile está dentro do alcance atual da espada
    if (atacante == NULL || sala == NULL || atacante->timer_debug_ataque <= 0.0f) { //nao pegarao o meu codigo
        return false;
    }

    int dx, dy;
    direcao_para_delta(atacante->debug_ataque_direcao, &dx, &dy);

    for (int i = 1; i <= atacante->debug_ataque_alcance; i++) { //alcance da espada em distância a partir do jogador
        int frente_x = atacante->pos.x + dx * i;
        int frente_y = atacante->pos.y + dy * i;

        int tiles_x[3];
        int tiles_y[3];

        tiles_x[0] = frente_x; //tile diretamente na frente
        tiles_y[0] = frente_y;

        if (dx != 0) { //se ataca pra esquerda/direita, as diagonais ficam acima e abaixo da frente
            tiles_x[1] = frente_x;
            tiles_y[1] = frente_y - 1;
            tiles_x[2] = frente_x;
            tiles_y[2] = frente_y + 1;
        }
        else { //se ataca pra cima/baixo, as diagonais ficam à esquerda e à direita da frente
            tiles_x[1] = frente_x - 1;
            tiles_y[1] = frente_y;
            tiles_x[2] = frente_x + 1;
            tiles_y[2] = frente_y;
        }

        for (int j = 0; j < 3; j++) { //testa frente + duas diagonais
            if (tiles_x[j] == tile_x && tiles_y[j] == tile_y) {
                if (tile_x < 0 || tile_y < 0 || tile_x >= sala->largura || tile_y >= sala->altura) { //fora do mapa não conta como acerto
                    return false;
                }

                if (sala_colisao(sala, tile_x, tile_y)) { //parede bloqueia aquele pedaço da espada
                    return false;
                }

                return true;
            }
        }
    }

    return false;
}

static void desenhar_debug_ataque(Entidade* atacante, Sala* sala, int cam_x, int cam_y, int tamanho_tile) { //desenha em verde os tiles que a espada está cobrindo, só pra debug
    if (atacante == NULL || sala == NULL || atacante->timer_debug_ataque <= 0.0f) { //pegaram o meu codigo :c
        return;
    }

    int dx, dy;
    direcao_para_delta(atacante->debug_ataque_direcao, &dx, &dy);

    for (int i = 1; i <= atacante->debug_ataque_alcance; i++) { //normalmente i só vale 1: frente + diagonal superior + diagonal inferior
        int frente_x = atacante->pos.x + dx * i;
        int frente_y = atacante->pos.y + dy * i;

        int tiles_x[3];
        int tiles_y[3];

        tiles_x[0] = frente_x; //tile diretamente na frente do jogador
        tiles_y[0] = frente_y;

        if (dx != 0) { //ataque horizontal: pinta frente, diagonal de cima e diagonal de baixo
            tiles_x[1] = frente_x;
            tiles_y[1] = frente_y - 1;
            tiles_x[2] = frente_x;
            tiles_y[2] = frente_y + 1;
        }
        else { //ataque vertical: pinta frente, diagonal da esquerda e diagonal da direita
            tiles_x[1] = frente_x - 1;
            tiles_y[1] = frente_y;
            tiles_x[2] = frente_x + 1;
            tiles_y[2] = frente_y;
        }

        for (int j = 0; j < 3; j++) { //desenha os 3 tiles da hitbox da espada
            int tile_x = tiles_x[j];
            int tile_y = tiles_y[j];

            if (tile_x < 0 || tile_y < 0 || tile_x >= sala->largura || tile_y >= sala->altura) { //se sair do mapa, ignora só esse pedaço
                continue;
            }

            if (sala_colisao(sala, tile_x, tile_y)) { //debug não pinta parede
                continue;
            }

            DrawRectangle(
                (tile_x - cam_x) * tamanho_tile,
                (tile_y - cam_y) * tamanho_tile,
                tamanho_tile,
                tamanho_tile,
                GREEN
            );
        }
    }
}

static void inimigo_mudar_direcao(Entidade* inimigo, Sala* sala) { //faz o inimigo tentar mudar de direção sozinho, igual no jogo original
    if (inimigo == NULL || sala == NULL) { //sem inimigo ou sala não tem como decidir caminho
        return;
    }

    for (int tentativa = 0; tentativa < 4; tentativa++) { //tenta algumas direções antes de desistir
        Direcao nova_direcao = (Direcao)GetRandomValue(0, 3);
        int dx, dy;
        direcao_para_delta(nova_direcao, &dx, &dy);

        float teste_x = inimigo->x + dx * 0.25f;
        float teste_y = inimigo->y + dy * 0.25f;

        if (!entidade_colide_mapa(inimigo, sala, teste_x, teste_y, true)) { //só troca se a direção nova não jogar ele direto na parede
            inimigo->direcao = nova_direcao;
            break;
        }
    }

    inimigo->timer_mudar_direcao = GetRandomValue(45, 110) / 100.0f; //próxima troca voluntária entre 0.45s e 1.10s
}

ListaEntidades* lista_criar(void) { //criação de lista encadeada pras entidades
    ListaEntidades* lista = malloc(sizeof(ListaEntidades));

    if (lista == NULL) { //"penso logo farmo aura" - rené descartes
        return NULL;
    }

    lista->head = NULL;
    lista->contagem = 0; //debug

    return lista;
}

Entidade* entidade_criar(EntTipo tipo, int x, int y) { //função de criar entidade, pega tipo e posição
    Entidade* entidade = malloc(sizeof(Entidade));

    if (entidade == NULL) { //go drinking🍻 (vai tomando)
        return NULL;
    }

    entidade->tipo = tipo;

    entidade->pos.x = x;
    entidade->pos.y = y;

    entidade->x = x + 0.5f; //posição real começa no centro do tile de spawn
    entidade->y = y + 0.5f; //posição real começa no centro do tile de spawn

    entidade->vivo = true;          //booleano que vai ser setado pra false caso a entidade "morra" (item coletado ou hp<=0)

    entidade->direcao = DIR_BAIXO;  // só pra padronizar, ela spawna "olhando" pra baixo

    entidade->timer_ataque = 0.0f;  //
    entidade->timer_movimento = 0.0f; //timer usado pelo inimigo pra patrulhar sem andar rápido demais
    entidade->timer_mudar_direcao = 0.0f; //timer usado pra inimigo mudar de direção sem precisar bater na parede
    entidade->timer_debug_ataque = 0.0f; //timer usado pra manter a espada ativa e pintar a hitbox na tela
    entidade->debug_ataque_x = x; //mantido por compatibilidade, mas o ataque agora segue a posição atual do jogador
    entidade->debug_ataque_y = y; //ler comentário acima
    entidade->debug_ataque_alcance = 0; //sem ataque ativo no spawn
    entidade->debug_ataque_direcao = DIR_BAIXO; //direção padrão do debug
    entidade->ataque_ja_acertou = false; //evita dar dano várias vezes no mesmo golpe de espada

    entidade->next = NULL;

    if (tipo == ENT_JOGADOR) {      //se o tipo for jogador:
        entidade->max_hp = 10;
        entidade->hp = 10;
        entidade->ataque = 2;
        entidade->velocidade = 6.7f;
    }
    else if (tipo == ENT_INIMIGO) { //se nao, se o tipo for inimigo:
        entidade->max_hp = 3;
        entidade->hp = 3;
        entidade->ataque = 1;
        entidade->direcao = DIR_DIREITA;
        entidade->velocidade = 6.7f;
        entidade->timer_mudar_direcao = GetRandomValue(45, 110) / 100.0f;
    }
    else {                          //se nao, o tipo é item:
        entidade->max_hp = 1;
        entidade->hp = 1;
        entidade->ataque = 0;
        entidade->velocidade = 0.0f;
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

void lista_spawnar_sala(ListaEntidades* lista, Sala* sala, Entidade** jogador) { //cria as entidades usando os spawns que o mapa leu do .txt
    if (lista == NULL || sala == NULL) { //mortamdela :O
        return;
    }

    Entidade* novo_jogador = entidade_criar(ENT_JOGADOR, sala->spawn_jogador.x, sala->spawn_jogador.y);
    lista_adicionar(lista, novo_jogador);

    if (jogador != NULL) { //guarda um ponteiro direto pro jogador pra não precisar procurar na lista toda hora
        *jogador = novo_jogador;
    }

    for (int i = 0; i < sala->num_inimigos; i++) { //cada E no .txt vira um inimigo
        Entidade* inimigo = entidade_criar(ENT_INIMIGO, sala->spawn_inimigos[i].x, sala->spawn_inimigos[i].y);
        lista_adicionar(lista, inimigo);
    }

    for (int i = 0; i < sala->num_itens; i++) { //cada I no .txt vira um item
        Entidade* item = entidade_criar(ENT_ITEM, sala->spawn_itens[i].x, sala->spawn_itens[i].y);
        lista_adicionar(lista, item);
    }
}

void entidade_mover_jogador(Entidade* jogador, Sala* sala, float dt) { //movimenta o jogador de forma fluida enquanto a tecla estiver pressionada
    if (jogador == NULL || sala == NULL) { //cala breza :p
        return;
    }

    float dx = 0.0f;
    float dy = 0.0f;

    //IsKeyDown continua verdadeiro enquanto a tecla estiver pressionada, por isso é melhor pra movimentação
    if (IsKeyDown(KEY_RIGHT)) {
        dx = 1.0f;
        jogador->direcao = DIR_DIREITA;
    }
    else if (IsKeyDown(KEY_LEFT)) {
        dx = -1.0f;
        jogador->direcao = DIR_ESQUERDA;
    }
    else if (IsKeyDown(KEY_DOWN)) {
        dy = 1.0f;
        jogador->direcao = DIR_BAIXO;
    }
    else if (IsKeyDown(KEY_UP)) {
        dy = -1.0f;
        jogador->direcao = DIR_CIMA;
    }

    float novo_x = jogador->x + dx * jogador->velocidade * dt;
    float novo_y = jogador->y + dy * jogador->velocidade * dt;

    if (!entidade_colide_mapa(jogador, sala, novo_x, jogador->y, false)) { //move no eixo x se não bater em parede
        jogador->x = novo_x;
    }

    if (!entidade_colide_mapa(jogador, sala, jogador->x, novo_y, false)) { //move no eixo y se não bater em parede
        jogador->y = novo_y;
    }

    entidade_sincronizar_tile(jogador); //mantém pos.x/pos.y coerentes pro ataque, porta e coleta
}

static void aplicar_dano_ataque(Entidade* atacante, ListaEntidades* alvos, Sala* sala, int* score) { //aplica o dano da espada no primeiro inimigo dentro da hitbox ativa
    if (atacante == NULL || alvos == NULL || sala == NULL || atacante->ataque_ja_acertou) { //se já acertou nesse golpe, não dá dano de novo
        return;
    }

    Entidade* atual = alvos->head;

    while (atual != NULL) {
        Entidade* proximo = atual->next; //guarda o próximo porque o inimigo pode morrer e sair da lista

        if (atual->tipo == ENT_INIMIGO && atual->vivo && tile_dentro_ataque(atacante, sala, atual->pos.x, atual->pos.y)) {
            atual->hp -= atacante->ataque;
            atacante->ataque_ja_acertou = true;

            if (atual->hp <= 0) { //se o hp zerar, inimigo morre e sai da lista
                atual->vivo = false;

                if (score != NULL) {
                    *score += 100;
                }

                lista_remover(alvos, atual);
            }

            return;
        }

        atual = proximo;
    }
}

void entidade_atacar(Entidade* atacante, ListaEntidades* alvos, Sala* sala, int alcance, int* score) { //ativa um golpe de espada que segue o jogador por alguns instantes
    if (atacante == NULL || alvos == NULL || sala == NULL) { //peperoni :T
        return;
    }

    if (atacante->timer_ataque > 0.0f) { //cooldown pra não spammar ataque
        return;
    }

    atacante->timer_ataque = 0.50f; //cooldown do golpe
    atacante->timer_debug_ataque = 0.50f; //tempo em que a hitbox da espada fica ativa e pintada de verde
    atacante->debug_ataque_x = atacante->pos.x; //mantido só por compatibilidade, o debug agora segue a posição atual do jogador
    atacante->debug_ataque_y = atacante->pos.y; //ler comentário acima
    atacante->debug_ataque_alcance = alcance; //distância da espada: com alcance 1, pinta frente + duas diagonais
    atacante->debug_ataque_direcao = atacante->direcao; //guarda a direção do golpe no momento do espaço
    atacante->ataque_ja_acertou = false; //cada golpe pode acertar um inimigo uma vez

    aplicar_dano_ataque(atacante, alvos, sala, score); //tenta acertar imediatamente ao apertar espaço
}

void lista_atualizar(ListaEntidades* lista, Sala* sala, Entidade* jogador, float dt, int* score, EstadoJogo* estado) { //atualiza cooldown, IA inimiga, dano por contato e coleta de item
    if (lista == NULL || sala == NULL || jogador == NULL) { //inhame (acabarm as ideias) :l
        return;
    }

    Entidade* atual = lista->head;

    while (atual != NULL) {
        Entidade* proximo = atual->next; //guarda o próximo antes, porque item pode ser removido no meio do loop

        if (atual->timer_ataque > 0.0f) {
            atual->timer_ataque -= dt;
            if (atual->timer_ataque < 0.0f) atual->timer_ataque = 0.0f;
        }

        if (atual->timer_movimento > 0.0f) {
            atual->timer_movimento -= dt;
            if (atual->timer_movimento < 0.0f) atual->timer_movimento = 0.0f;
        }

        if (atual->timer_mudar_direcao > 0.0f) {
            atual->timer_mudar_direcao -= dt;
            if (atual->timer_mudar_direcao < 0.0f) atual->timer_mudar_direcao = 0.0f;
        }

        if (atual->timer_debug_ataque > 0.0f) { //faz o debug visual do ataque sumir depois de alguns frames
            atual->timer_debug_ataque -= dt;
            if (atual->timer_debug_ataque < 0.0f) atual->timer_debug_ataque = 0.0f;
        }

        if (atual->tipo == ENT_INIMIGO && atual->vivo) {
            if (jogador->timer_debug_ataque > 0.0f && !jogador->ataque_ja_acertou && tile_dentro_ataque(jogador, sala, atual->pos.x, atual->pos.y)) { //se o inimigo entrar na espada durante o golpe, toma dano
                atual->hp -= jogador->ataque;
                jogador->ataque_ja_acertou = true;

                if (atual->hp <= 0) { //se o hp zerar, inimigo morre e sai da lista
                    atual->vivo = false;

                    if (score != NULL) {
                        *score += 100;
                    }

                    lista_remover(lista, atual);
                    atual = proximo;
                    continue;
                }
            }

            if ((entidades_encostando(atual, jogador) || distancia_manhattan(atual, jogador) <= 0) && atual->timer_ataque <= 0.0f) { //se encostar no jogador, dá dano com cooldown
                jogador->hp -= atual->ataque;
                atual->timer_ataque = 1.0f;

                if (jogador->hp <= 0) { //se o jogador morrer, vai pra tela de game over
                    jogador->hp = 0;
                    jogador->vivo = false;

                    if (estado != NULL) {
                        *estado = ESTADO_GAME_OVER;
                    }
                }
            }

            if (atual->timer_mudar_direcao <= 0.0f) { //além de inverter na parede, o inimigo também muda de direção por vontade própria
                inimigo_mudar_direcao(atual, sala);
            }

            if (atual->timer_movimento <= 0.0f) { //anda todo frame, se bater em parede ou porta inverte
                int dx, dy;
                direcao_para_delta(atual->direcao, &dx, &dy);

                float novo_x = atual->x + dx * atual->velocidade * dt;
                float novo_y = atual->y + dy * atual->velocidade * dt;

                if (entidade_colide_mapa(atual, sala, novo_x, novo_y, true)) {
                    atual->direcao = inverter_direcao(atual->direcao);
                    atual->timer_movimento = 0.15f; //pequena pausa depois de bater, pra patrulha não tremer na parede
                }
                else {
                    atual->x = novo_x;
                    atual->y = novo_y;
                    entidade_sincronizar_tile(atual);
                }
            }
        }
        else if (atual->tipo == ENT_ITEM && atual->vivo) {
            if (entidades_encostando(atual, jogador)) { //se o jogador encostar no item, coleta
                atual->vivo = false;

                if (score != NULL) {
                    *score += 250;
                }

                lista_remover(lista, atual);
            }
        }

        atual = proximo;
    }
}

void lista_desenhar(ListaEntidades* lista, Sala* sala, int cam_x, int cam_y, int tamanho_tile) { //desenha jogador, inimigos, itens e o debug da hitbox de ataque
    if (lista == NULL) { //mandeoca :I
        return;
    }

    Entidade* atual = lista->head;

    while (atual != NULL) {
        if (atual->tipo == ENT_JOGADOR) { //desenha primeiro a hitbox do ataque pra entidade aparecer por cima dela
            desenhar_debug_ataque(atual, sala, cam_x, cam_y, tamanho_tile);
        }

        Color cor = WHITE;

        if (atual->tipo == ENT_JOGADOR) {
            cor = BLUE;
        }
        else if (atual->tipo == ENT_INIMIGO) {
            cor = RED;
        }
        else if (atual->tipo == ENT_ITEM) {
            cor = GREEN;
        }

        float tamanho_entidade = entidade_raio(atual) * 2.0f * tamanho_tile;
        float tela_x = (atual->x - cam_x) * tamanho_tile - tamanho_entidade / 2.0f;
        float tela_y = (atual->y - cam_y) * tamanho_tile - tamanho_entidade / 2.0f;

        DrawRectangle(
            (int)tela_x,
            (int)tela_y,
            (int)tamanho_entidade,
            (int)tamanho_entidade,
            cor
        );

        atual = atual->next;
    }
}
