#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "entidade.h"
#include "../mapa/mapa.h"
#include "../sistema/sistema.h"

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

static void ataque_retangulo(Entidade* atacante, float* esquerda, float* direita, float* cima, float* baixo) { //calcula a hitbox da "espada", usando posição float
    float alcance = 1.0f;
    float metade_tile_jogador = 0.5f; //inclui o tile onde o jogador está, não só o tile da frente
    float alcance_lateral = 0.75f; //pega mais que meia tile nos lados pra não falhar quando o inimigo encosta no canto

    if (atacante != NULL && atacante->debug_ataque_alcance > 0) { //por enquanto o main passa 1, mas da pra trocar se precisar
        alcance = (float)atacante->debug_ataque_alcance;
    }

    if (atacante == NULL || esquerda == NULL || direita == NULL || cima == NULL || baixo == NULL) { //VVVVVVRRRRRRRUUUUUUUUUUUUUUUUUUUMMMMMMMMMMMMMMM🏎️🏎️🏎️🏎️🏎️
        return;
    }

    if (atacante->direcao == DIR_DIREITA) { //tile do jogador + 1 tile completo na frente + 0.75 tile pra cima/baixo
        *esquerda = atacante->x - metade_tile_jogador;
        *direita = atacante->x + metade_tile_jogador + alcance;
        *cima = atacante->y - alcance_lateral;
        *baixo = atacante->y + alcance_lateral;
    }
    else if (atacante->direcao == DIR_ESQUERDA) { //mesma coisa, só que pra esquerda
        *esquerda = atacante->x - metade_tile_jogador - alcance;
        *direita = atacante->x + metade_tile_jogador;
        *cima = atacante->y - alcance_lateral;
        *baixo = atacante->y + alcance_lateral;
    }
    else if (atacante->direcao == DIR_BAIXO) { //tile do jogador + 1 tile pra baixo + 0.75 tile pros lados
        *esquerda = atacante->x - alcance_lateral;
        *direita = atacante->x + alcance_lateral;
        *cima = atacante->y - metade_tile_jogador;
        *baixo = atacante->y + metade_tile_jogador + alcance;
    }
    else { //DIR_CIMA ne cabecao
        *esquerda = atacante->x - alcance_lateral;
        *direita = atacante->x + alcance_lateral;
        *cima = atacante->y - metade_tile_jogador - alcance;
        *baixo = atacante->y + metade_tile_jogador;
    }
}

static bool retangulos_encostando(float esq_a, float dir_a, float cima_a, float baixo_a, float esq_b, float dir_b, float cima_b, float baixo_b) {
    if (esq_a > dir_a) {
        float temp = esq_a;
        esq_a = dir_a;
        dir_a = temp;
    }

    if (cima_a > baixo_a) {
        float temp = cima_a;
        cima_a = baixo_a;
        baixo_a = temp;
    }

    if (esq_b > dir_b) {
        float temp = esq_b;
        esq_b = dir_b;
        dir_b = temp;
    }

    if (cima_b > baixo_b) {
        float temp = cima_b;
        cima_b = baixo_b;
        baixo_b = temp;
    }

    return esq_a <= dir_b && dir_a >= esq_b && cima_a <= baixo_b && baixo_a >= cima_b;
}

static bool ataque_bloqueado_por_parede(Entidade* atacante, Sala* sala) { //impede a espada de funcionar se o tile diretamente na frente for parede
    if (atacante == NULL || sala == NULL) { //sei nem pra que colocar um desse aq mas vai q quebra tudo por nao ter
        return true;
    }

    int dx, dy;
    direcao_para_delta(atacante->direcao, &dx, &dy);

    int frente_x = (int)(atacante->x + dx * 0.75f); //pega o tile na frente do centro do jogador usando a direção atual
    int frente_y = (int)(atacante->y + dy * 0.75f);

    if (frente_x < 0 || frente_y < 0 || frente_x >= sala->largura || frente_y >= sala->altura) { //fora do mapa bloqueia
        return true;
    }

    return sala_colisao(sala, frente_x, frente_y); //parede bloqueia o golpe
}

static bool entidade_dentro_ataque(Entidade* atacante, Sala* sala, Entidade* alvo) { //verifica se a hitbox float da espada encosta na hitbox float do inimigo
    if (atacante == NULL || sala == NULL || alvo == NULL || atacante->timer_debug_ataque <= 0.0f) { //aoooo potencia
        return false;
    }

    if (ataque_bloqueado_por_parede(atacante, sala)) { //não deixa acertar através da parede logo na frente
        return false;
    }

    float ataque_esq, ataque_dir, ataque_cima, ataque_baixo;
    ataque_retangulo(atacante, &ataque_esq, &ataque_dir, &ataque_cima, &ataque_baixo);

    float raio_alvo = entidade_raio(alvo);
    float alvo_esq = alvo->x - raio_alvo;
    float alvo_dir = alvo->x + raio_alvo;
    float alvo_cima = alvo->y - raio_alvo;
    float alvo_baixo = alvo->y + raio_alvo;

    return retangulos_encostando(ataque_esq, ataque_dir, ataque_cima, ataque_baixo, alvo_esq, alvo_dir, alvo_cima, alvo_baixo);
}

static void desenhar_debug_ataque(Entidade* atacante, Sala* sala, int cam_x, int cam_y, int tamanho_tile) { //desenha em verde o retângulo real da espada, preso ao jogador, só pra debug
    if (atacante == NULL || sala == NULL || atacante->timer_debug_ataque <= 0.0f) { //pegaram o meu codigo :c
        return;
    }

    if (ataque_bloqueado_por_parede(atacante, sala)) { //se tem parede na frente, não pinta a espada atravessando parede
        return;
    }

    float ataque_esq, ataque_dir, ataque_cima, ataque_baixo;
    ataque_retangulo(atacante, &ataque_esq, &ataque_dir, &ataque_cima, &ataque_baixo);

    DrawRectangle(
        (int)((ataque_esq - cam_x) * tamanho_tile),
        (int)((ataque_cima - cam_y) * tamanho_tile),
        (int)((ataque_dir - ataque_esq) * tamanho_tile),
        (int)((ataque_baixo - ataque_cima) * tamanho_tile),
        GREEN
    );
}

#define INIMIGO_ALCANCE_VISAO 6 //distância em tiles pra inimigo começar a perseguir o jogador

static void inimigo_perseguir_jogador(Entidade* inimigo, Entidade* jogador, Sala* sala) { //move o inimigo em direção ao jogador; tenta o eixo com maior distância primeiro
    int dx = jogador->pos.x - inimigo->pos.x;
    int dy = jogador->pos.y - inimigo->pos.y;

    Direcao opcoes[2];
    int n = 0;

    if ((dx < 0 ? -dx : dx) >= (dy < 0 ? -dy : dy)) { //prefere o eixo com maior delta
        if (dx > 0) opcoes[n++] = DIR_DIREITA;
        else if (dx < 0) opcoes[n++] = DIR_ESQUERDA;
        if (dy > 0) opcoes[n++] = DIR_BAIXO;
        else if (dy < 0) opcoes[n++] = DIR_CIMA;
    } else {
        if (dy > 0) opcoes[n++] = DIR_BAIXO;
        else if (dy < 0) opcoes[n++] = DIR_CIMA;
        if (dx > 0) opcoes[n++] = DIR_DIREITA;
        else if (dx < 0) opcoes[n++] = DIR_ESQUERDA;
    }

    for (int i = 0; i < n; i++) { //tenta cada opção, usa a primeira que não bate em parede
        int ddx, ddy;
        direcao_para_delta(opcoes[i], &ddx, &ddy);
        float teste_x = inimigo->x + ddx * 0.25f;
        float teste_y = inimigo->y + ddy * 0.25f;
        if (!entidade_colide_mapa(inimigo, sala, teste_x, teste_y, true)) {
            inimigo->direcao = opcoes[i];
            return;
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
    entidade->debug_ataque_x = x; //o ataque segue a posição atual do jogador
    entidade->debug_ataque_y = y; //ler comentário acima
    entidade->debug_ataque_alcance = 0; //sem ataque ativo no spawn
    entidade->debug_ataque_direcao = DIR_BAIXO; //o ataque usa a direção atual da entidade
    entidade->ataque_ja_acertou = false; //evita dar dano várias vezes no mesmo golpe de espada

    entidade->next = NULL;

    if (tipo == ENT_JOGADOR) {      //se o tipo for jogador:
        entidade->max_hp      = 15;
        entidade->hp          = 15;
        entidade->max_oxigenio = 15;
        entidade->oxigenio     = 15;
        entidade->ataque      = 3;
        entidade->velocidade  = 4.0f;
    }
    else if (tipo == ENT_INIMIGO) { //se nao, se o tipo for inimigo:
        entidade->max_hp = 5;
        entidade->hp = 5;
        entidade->ataque = 2;
        entidade->direcao = DIR_DIREITA;
        entidade->velocidade = 2.5f;
        entidade->timer_mudar_direcao = GetRandomValue(45, 110) / 100.0f;
    }
    else if (tipo == ENT_ITEM) {    //se nao, o tipo é item:
        entidade->max_hp = 1;
        entidade->hp = 1;
        entidade->ataque = 0;
        entidade->velocidade = 0.0f;
    }
    else {                          //se nao, é um fragmento do código de autodestruição
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

void lista_spawnar_sala(ListaEntidades* lista, Sala* sala, Entidade** jogador, Fase* fase) { //cria as entidades usando os spawns que o mapa leu do .txt
    if (lista == NULL || sala == NULL) { //mortamdela :O
        return;
    }

    Entidade* novo_jogador = entidade_criar(ENT_JOGADOR, sala->spawn_jogador.x, sala->spawn_jogador.y);
    novo_jogador->spawn_idx = -1;
    lista_adicionar(lista, novo_jogador);

    if (jogador != NULL) { //guarda um ponteiro direto pro jogador pra não precisar procurar na lista toda hora
        *jogador = novo_jogador;
    }

    for (int i = 0; i < sala->num_inimigos; i++) { //cada E no .txt vira um inimigo; pula se já foi morto
        if (sala->inimigos_mortos[i]) continue;
        Entidade* inimigo = entidade_criar(ENT_INIMIGO, sala->spawn_inimigos[i].x, sala->spawn_inimigos[i].y);
        inimigo->spawn_idx = i;
        lista_adicionar(lista, inimigo);
    }

    for (int i = 0; i < sala->num_itens; i++) { //cada I no .txt vira um item
        Entidade* item = entidade_criar(ENT_ITEM, sala->spawn_itens[i].x, sala->spawn_itens[i].y);
        item->spawn_idx = -1;
        lista_adicionar(lista, item);
    }

    for (int i = 0; i < sala->num_fragmentos; i++) { //cada a/b/c/d no .txt vira um fragmento; pula se o jogador já coletou
        int id = sala->fragmento_ids[i];
        if (fase != NULL && ((fase->digitos_coletados >> id) & 1)) continue; //já foi pego, não reaparece
        Entidade* frag = entidade_criar(ENT_FRAGMENTO, sala->spawn_fragmentos[i].x, sala->spawn_fragmentos[i].y);
        frag->spawn_idx = id; //reutiliza spawn_idx pra guardar qual fragmento é (0-3)
        lista_adicionar(lista, frag);
    }
}

void entidade_mover_jogador(Entidade* jogador, Sala* sala, float dt) { //movimenta o jogador de forma fluida enquanto a tecla estiver pressionada
    if (jogador == NULL || sala == NULL) { //cala breza :p
        return;
    }

    float dx = 0.0f;
    float dy = 0.0f;

    //IsKeyDown continua verdadeiro enquanto a tecla estiver pressionada
    //X e Y são lidados separadamente, assim dá pra andar na diagonal
    if (IsKeyDown(KEY_RIGHT)) {
        dx = 1.0f;
        jogador->direcao = DIR_DIREITA;
    }
    if (IsKeyDown(KEY_LEFT)) {
        dx = -1.0f;
        jogador->direcao = DIR_ESQUERDA;
    }
    if (IsKeyDown(KEY_DOWN)) {
        dy = 1.0f;
        jogador->direcao = DIR_BAIXO;
    }
    if (IsKeyDown(KEY_UP)) {
        dy = -1.0f;
        jogador->direcao = DIR_CIMA;
    }

    if (dx != 0.0f && dy != 0.0f) { //diagonal: normaliza pra não andar mais rápido
        dx *= 0.7071f;
        dy *= 0.7071f;
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

        if (atual->tipo == ENT_INIMIGO && atual->vivo && entidade_dentro_ataque(atacante, sala, atual)) {
            atual->hp -= atacante->ataque;
            atacante->ataque_ja_acertou = true;

            if (atual->hp <= 0) { //se o hp zerar, inimigo morre e sai da lista
                atual->vivo = false;

                if (atual->spawn_idx >= 0) {
                    sala->inimigos_mortos[atual->spawn_idx] = true;
                }

                if (score != NULL) {
                    *score += 100;
                }

                if (GetRandomValue(0, 9) < 7) { //70% de chance de dropar oxigênio ou energia
                    Entidade* drop = entidade_criar(ENT_ITEM, atual->pos.x, atual->pos.y);
                    drop->ataque    = GetRandomValue(1, 2); //1 = oxigênio, 2 = energia
                    drop->spawn_idx = -1;
                    lista_adicionar(alvos, drop);
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
    atacante->debug_ataque_alcance = alcance; //distância da espada: com alcance 1, pega o tile do jogador + 1 tile na frente
    atacante->debug_ataque_direcao = atacante->direcao; //hitbox acompanha a direção atual durante o golpe
    atacante->ataque_ja_acertou = false; //cada golpe pode acertar um inimigo uma vez

    aplicar_dano_ataque(atacante, alvos, sala, score); //tenta acertar imediatamente ao apertar espaço
}

void lista_atualizar(ListaEntidades* lista, Sala* sala, Entidade* jogador, float dt, int* score, EstadoJogo* estado, Fase* fase) { //atualiza cooldown, IA inimiga, dano por contato, coleta de item e coleta de fragmento
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
            if (jogador->timer_debug_ataque > 0.0f && !jogador->ataque_ja_acertou && entidade_dentro_ataque(jogador, sala, atual)) { //se o inimigo entrar na espada durante o golpe, toma dano
                atual->hp -= jogador->ataque;
                jogador->ataque_ja_acertou = true;

                if (atual->hp <= 0) { //se o hp zerar, inimigo morre e sai da lista
                    atual->vivo = false;

                    if (atual->spawn_idx >= 0) {
                        sala->inimigos_mortos[atual->spawn_idx] = true;
                    }

                    if (score != NULL) {
                        *score += 100;
                    }

                    if (GetRandomValue(0, 9) < 7) { //70% de chance de dropar oxigênio ou energia
                        Entidade* drop = entidade_criar(ENT_ITEM, atual->pos.x, atual->pos.y);
                        drop->ataque   = GetRandomValue(1, 2); //1 = oxigênio, 2 = energia
                        drop->spawn_idx = -1;
                        lista_adicionar(lista, drop);
                    }

                    lista_remover(lista, atual);
                    atual = proximo;
                    continue;
                }
            }

            if ((entidades_encostando(atual, jogador) || distancia_manhattan(atual, jogador) <= 0) && atual->timer_ataque <= 0.0f) { //se encostar no jogador, drena oxigênio (inimigos são aliens que sugam o ar)
                jogador->oxigenio -= atual->ataque;
                atual->timer_ataque = 1.0f;

                if (jogador->oxigenio <= 0) { //sem oxigênio = game over
                    jogador->oxigenio = 0;
                    jogador->vivo = false;

                    if (estado != NULL) {
                        *estado = ESTADO_GAME_OVER;
                    }
                }
            }

            if (distancia_manhattan(atual, jogador) <= INIMIGO_ALCANCE_VISAO) { //dentro do alcance: persegue o jogador
                inimigo_perseguir_jogador(atual, jogador, sala);
            } else if (atual->timer_mudar_direcao <= 0.0f) { //fora do alcance: patrulha aleatória
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

                if (atual->ataque == 0) { //item normal do mapa: só score
                    if (score != NULL) *score += 250;
                } else if (atual->ataque == 1) { //drop oxigênio: restaura tanque de oxigênio
                    jogador->oxigenio += 3;
                    if (jogador->oxigenio > jogador->max_oxigenio) jogador->oxigenio = jogador->max_oxigenio;
                    if (score != NULL) *score += 50;
                } else { //drop energia: restaura HP de combate
                    jogador->hp += 3;
                    if (jogador->hp > jogador->max_hp) jogador->hp = jogador->max_hp;
                    if (score != NULL) *score += 50;
                }

                lista_remover(lista, atual);
            }
        }
        else if (atual->tipo == ENT_FRAGMENTO && atual->vivo) {
            if (entidades_encostando(atual, jogador)) { //jogador encostou no fragmento do código
                atual->vivo = false;

                if (fase != NULL) {
                    fase->digitos_coletados  |= (1 << atual->spawn_idx); //marca o bit do fragmento como coletado
                    fase->ultimo_fragmento    = atual->spawn_idx;         //avisa o main.c qual foi coletado agora
                }

                if (score != NULL) {
                    *score += 500; //fragmento vale mais que item normal pq é parte da missão
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
            if (atual->ataque == 1) cor = SKYBLUE; //drop oxigênio: azul claro
            else if (atual->ataque == 2) cor = LIME;    //drop energia: verde limão (igual à barra)
            else cor = PURPLE;                           //item normal do mapa: roxo
        }
        else if (atual->tipo == ENT_FRAGMENTO) {
            cor = ORANGE; //fragmento do código aparece laranja pra se destacar do item verde
        }

        float tamanho_entidade = entidade_raio(atual) * 2.0f * tamanho_tile;
        float tela_x = (atual->x - cam_x) * tamanho_tile - tamanho_entidade / 2.0f;
        float tela_y = (atual->y - cam_y) * tamanho_tile - tamanho_entidade / 2.0f;

        if (atual->tipo == ENT_JOGADOR) {
            Texture2D tex = sistema_get_player_texture(atual->direcao);
            float scale = tamanho_entidade / (float)tex.width; // 51.2 / 64 = 0.8
            DrawTextureEx(tex, (Vector2){tela_x, tela_y}, 0.0f, scale, WHITE);
        } else {
            DrawRectangle((int)tela_x, (int)tela_y, (int)tamanho_entidade, (int)tamanho_entidade, cor);
        }

        atual = atual->next;
    }
}
