#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "raylib.h"
#include "types.h"
#include "mapa/mapa.h"
#include "entidade/entidade.h"
#include "sistema/sistema.h"

#define TAMANHO_TILE  80
#define CAMERA_LARGURA 12
#define CAMERA_ALTURA  10
#define SALA_CACHE_MAX 32
#define ALTURA_HUD 140

static Sala* sala_cache[SALA_CACHE_MAX]; //salas ficam em memória durante toda a partida pra preservar inimigos_mortos
static int   sala_cache_n = 0;

static Sala* sala_obter(const char* arquivo) { //retorna sala do cache se já foi carregada, senão carrega do disco e guarda
    for (int i = 0; i < sala_cache_n; i++) {
        if (strncmp(sala_cache[i]->arquivo, arquivo, 63) == 0) {
            return sala_cache[i];
        }
    }
    if (sala_cache_n >= SALA_CACHE_MAX) return NULL;
    Sala* s = sala_carregar(arquivo);
    if (s) sala_cache[sala_cache_n++] = s;
    return s;
}

static void sala_cache_limpar(void) { //libera todas as salas do cache; chamado no game over e ao fechar janela
    for (int i = 0; i < sala_cache_n; i++) {
        sala_descarregar(sala_cache[i]);
    }
    sala_cache_n = 0;
}

/*
Mudanças levemente drásticas no main.c pra permitir que o jogador e os inimigos se movam de maneira fluida, 
além de integrar todo o sistema de lista encadeada e as trocentas funções q eu escrevi.
Se tem um trecho q eu apaguei é pq tem uma função no entidades.c que faz a mesma coisa so q mais dificil.

se eu quebrei alguma outra coisa q ja tava funcionando peço perdao
*/

static bool tile_livre_spawn(Sala* sala, int x, int y) { //verifica se uma tile pode receber o jogador depois de trocar de sala
    if (sala == NULL) { //resenha ou morte! - dom pedro Iº
        return false;
    }

    if (x < 0 || y < 0 || x >= sala->largura || y >= sala->altura) { //se saiu do mapa, não serve
        return false;
    }

    if (sala->grid[y][x].tipo == TILE_PORTA) { //não spawna em cima da porta pra evitar loop infinito entre salas
        return false;
    }

    return !sala_colisao(sala, x, y); //se não for parede, serve
}

static Direcao lado_entrada_sala_nova(Direcao dir_saida) { //converte a porta por onde saiu no lado por onde entra na sala nova
    switch (dir_saida) {
        case DIR_CIMA:     return DIR_BAIXO;
        case DIR_BAIXO:    return DIR_CIMA;
        case DIR_ESQUERDA: return DIR_DIREITA;
        case DIR_DIREITA:  return DIR_ESQUERDA;
    }

    return DIR_BAIXO;
}

static bool procurar_spawn_na_entrada(Sala* sala, Direcao lado_entrada, int* out_x, int* out_y) { //procura uma tile livre logo depois da porta da sala nova
    if (sala == NULL || out_x == NULL || out_y == NULL) { //tdah ou vagabundagem?
        return false;
    }

    if (lado_entrada == DIR_ESQUERDA) { //entrou pelo lado esquerdo, então procura portas na coluna 0 e spawna pra dentro da sala
        for (int y = 0; y < sala->altura; y++) {
            if (sala->grid[y][0].tipo == TILE_PORTA) {
                for (int x = 1; x < sala->largura; x++) {
                    if (tile_livre_spawn(sala, x, y)) {
                        *out_x = x;
                        *out_y = y;
                        return true;
                    }
                }
            }
        }
    }
    else if (lado_entrada == DIR_DIREITA) { //entrou pelo lado direito, então procura portas na última coluna e spawna pra dentro da sala
        int porta_x = sala->largura - 1;
        for (int y = 0; y < sala->altura; y++) {
            if (sala->grid[y][porta_x].tipo == TILE_PORTA) {
                for (int x = sala->largura - 2; x >= 0; x--) {
                    if (tile_livre_spawn(sala, x, y)) {
                        *out_x = x;
                        *out_y = y;
                        return true;
                    }
                }
            }
        }
    }
    else if (lado_entrada == DIR_CIMA) { //entrou por cima, então procura portas na linha 0 e spawna pra dentro da sala
        for (int x = 0; x < sala->largura; x++) {
            if (sala->grid[0][x].tipo == TILE_PORTA) {
                for (int y = 1; y < sala->altura; y++) {
                    if (tile_livre_spawn(sala, x, y)) {
                        *out_x = x;
                        *out_y = y;
                        return true;
                    }
                }
            }
        }
    }
    else if (lado_entrada == DIR_BAIXO) { //entrou por baixo, então procura portas na última linha e spawna pra dentro da sala
        int porta_y = sala->altura - 1;
        for (int x = 0; x < sala->largura; x++) {
            if (sala->grid[porta_y][x].tipo == TILE_PORTA) {
                for (int y = sala->altura - 2; y >= 0; y--) {
                    if (tile_livre_spawn(sala, x, y)) {
                        *out_x = x;
                        *out_y = y;
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

static bool procurar_spawn_fallback(Sala* sala, int* out_x, int* out_y) { //plano B caso alguma sala esteja sem porta do lado esperado
    if (sala == NULL || out_x == NULL || out_y == NULL) { //ai dento
        return false;
    }

    if (tile_livre_spawn(sala, sala->spawn_jogador.x, sala->spawn_jogador.y)) { //se o S da sala for válido, usa ele
        *out_x = sala->spawn_jogador.x;
        *out_y = sala->spawn_jogador.y;
        return true;
    }

    for (int y = 0; y < sala->altura; y++) { //se der tudo errado ele procura qualquer chão livre na sala
        for (int x = 0; x < sala->largura; x++) {
            if (tile_livre_spawn(sala, x, y)) {
                *out_x = x;
                *out_y = y;
                return true;
            }
        }
    }

    return false;
}

static void posicionar_jogador_apos_porta(Sala* sala, Entidade* jogador, Direcao dir_saida) { //posiciona o jogador na entrada correta da sala nova, sem alterar o desenho das salas
    if (sala == NULL || jogador == NULL) { //computador ligue a maquina de averiguar resenha
        return;
    }

    int novo_x = jogador->pos.x;
    int novo_y = jogador->pos.y;
    Direcao lado_entrada = lado_entrada_sala_nova(dir_saida);

    if (!procurar_spawn_na_entrada(sala, lado_entrada, &novo_x, &novo_y)) { //se não achou a porta esperada, usa fallback seguro
        procurar_spawn_fallback(sala, &novo_x, &novo_y);
    }

    jogador->pos.x = novo_x;
    jogador->pos.y = novo_y;
    jogador->x = jogador->pos.x + 0.5f; //sincroniza posição real no centro da tile depois de trocar de sala
    jogador->y = jogador->pos.y + 0.5f; //ler comentario acima
}


int main(void) {
    InitWindow(CAMERA_LARGURA * TAMANHO_TILE, (CAMERA_ALTURA * TAMANHO_TILE) + ALTURA_HUD, "Secret Quest");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL); //desabilita ESC fechar janela; janela fecha só pelo X ou pelo menu Sair

    menu_inicializar();
    sistema_carregar_assets();
    EstadoJogo estado = ESTADO_MENU;
    Sala* sala = NULL;
    ListaEntidades* entidades = NULL;
    Entidade* jogador = NULL;
    int score = 0;
    bool bloquear_enter_menu = false; //evita que o ENTER do game over já aperte "iniciar jogo" no menu

    Fase fase = {0};
    fase.codigo_autodestruicao = 4837; //código fixo da missão; os 4 fragmentos revelam cada dígito

    bool entrada_terminal = false;   //true quando o jogador está digitando o código no terminal
    char codigo_terminal[8]  = {0};  //buffer do código que o jogador está digitando
    int  codigo_terminal_len = 0;    //tamanho atual do buffer

    float timer_msg_frag = 0.0f;     //quanto tempo ainda exibe a mensagem de fragmento coletado
    char  msg_frag[48]   = {0};      //texto da mensagem (ex: "Fragmento coletado: dígito 4")

    float timer_oxigenio = 0.0f; //tempo desde a última queda de oxigênio

    // Controle das cutscenes
    int cutscene_idx = 0;       // 0 a 3
    float fade_alpha = 1.0f;    // 1.0 = preto total, 0.0 = imagem visível
    int fade_dir = -1;          // -1 = fade in (clareia), 1 = fade out (escurece)
    bool esperando_enter = false;

    Recorde ranking[5];
    int ranking_count = 0;
    char nome_input[16] = {0};
    int nome_input_len = 0;

    ranking_carregar(ranking, 5);
    for (int i = 0; i < 5; i++) {
        if (ranking[i].score > 0) ranking_count++;
        else break;
    }


    while (!WindowShouldClose()) {

        switch (estado) {

            case ESTADO_MENU:
                if (bloquear_enter_menu) { //se veio do game over, espera soltar ENTER antes do menu aceitar input (tava bugado antes)
                    if (!IsKeyDown(KEY_ENTER)) {
                        bloquear_enter_menu = false;
                    }
                }
                else {
                    menu_atualizar(&estado);
                }

                BeginDrawing();
                menu_desenhar();
                EndDrawing();
                break;

            case ESTADO_CUTSCENE: {
                float dt_cut = GetFrameTime();
                float fade_speed = 2.5f; // duração dos fades

                // Atualiza fade
                if (!esperando_enter) {
                    fade_alpha += fade_dir * fade_speed * dt_cut;
                    if (fade_alpha <= 0.0f) {
                        fade_alpha = 0.0f;
                        fade_dir = 0;
                        esperando_enter = true;
                    }
                    if (fade_alpha >= 1.0f) {
                        fade_alpha = 1.0f;
                        cutscene_idx++;
                        if (cutscene_idx >= 4) {
                            // Após as cutscenes, o jogo começa
                            cutscene_idx = 0;
                            fade_alpha = 1.0f;
                            fade_dir = -1;
                            esperando_enter = false;
                            sistema_transicao(&estado, ESTADO_JOGANDO);
                            break;
                        }
                        fade_dir = -1;
                        esperando_enter = false;
                    }
                }

                // Enter inicia o fade out
                if (esperando_enter && IsKeyPressed(KEY_ENTER)) {
                    esperando_enter = false;
                    fade_dir = 1; // começa a escurecer
                }

                BeginDrawing();
                Texture2D tex = sistema_get_cutscene_texture(cutscene_idx);
                DrawTexture(tex, 0, 0, WHITE);

                // Sobreposição fade
                if (fade_alpha > 0.0f) {
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 0, 0, 0, (unsigned char)(fade_alpha * 255) });
                }
                EndDrawing();
                break;
            }

            case ESTADO_JOGANDO: {
                if (sala == NULL) {
                    sala = sala_obter("mapa/salas/sala1.txt");
                    if (sala) {
                        entidades = lista_criar();
                        score = 0;
                        nome_input[0] = '\0';
                        nome_input_len = 0;
                        memset(&fase, 0, sizeof(Fase));            //reseta missão ao começar novo jogo
                        fase.codigo_autodestruicao = 4837;
                        fase.ultimo_fragmento      = -1;           //nenhum fragmento coletado ainda
                        timer_msg_frag = 0.0f;
                        msg_frag[0]    = '\0';
                        entrada_terminal   = false;
                        codigo_terminal[0] = '\0';
                        codigo_terminal_len = 0;
                        timer_oxigenio = 0.0f;
                        lista_spawnar_sala(entidades, sala, &jogador, &fase);
                    }
                }

                if (!sala || !entidades || !jogador) break;

                if (IsKeyPressed(KEY_ESCAPE) && !entrada_terminal) { //ESC abre pause; se o terminal estiver aberto, ESC fecha o terminal (tratado abaixo)
                    sistema_transicao(&estado, ESTADO_PAUSE);
                    break;
                }

                float dt = GetFrameTime();

                // contagem regressiva depois de ativar o terminal (20s pra chegar na saída)
                if (fase.codigo_ativado) {
                    fase.tempo_restante -= dt;
                    if (fase.tempo_restante <= 0.0f) { //kaboom
                        fase.tempo_restante = 0.0f;
                        sistema_transicao(&estado, ESTADO_GAME_OVER);
                        break;
                    }
                }

                // oxigênio drena com o tempo; energia só é afetada por inimigos
                timer_oxigenio += dt;
                if (timer_oxigenio >= 7.0f) {
                    timer_oxigenio = 0.0f;
                    jogador->oxigenio--;
                    if (jogador->oxigenio <= 0) {
                        jogador->oxigenio = 0;
                        jogador->vivo = false;
                        sistema_transicao(&estado, ESTADO_GAME_OVER); //sufocou
                    }
                }

                if (!entrada_terminal) { //bloqueia movimento e ataque enquanto o jogador tá no terminal
                    entidade_mover_jogador(jogador, sala, dt);

                    if (IsKeyPressed(KEY_SPACE) && jogador->hp > 0) { //porrada 💥💥 — custa 1 de energia
                        jogador->hp--;
                        if (jogador->hp <= 0) { //ficou sem energia = não consegue mais atacar e morre
                            jogador->hp = 0;
                            jogador->vivo = false;
                            sistema_transicao(&estado, ESTADO_GAME_OVER);
                            break;
                        }
                        entidade_atacar(jogador, entidades, sala, 1, &score);
                    }
                }

                lista_atualizar(entidades, sala, jogador, dt, &score, &estado, &fase); //atualiza IA, dano, cooldown, coleta de item e fragmento

                if (estado != ESTADO_JOGANDO) {
                    break;
                }

                // se entidade.c avisou que um fragmento foi coletado, monta a mensagem e liga o timer
                if (fase.ultimo_fragmento >= 0) {
                    int codigo = fase.codigo_autodestruicao;
                    int digitos[4]; //decompõe o código pra mostrar qual dígito esse fragmento revela
                    digitos[0] = (codigo / 1000) % 10;
                    digitos[1] = (codigo / 100)  % 10;
                    digitos[2] = (codigo / 10)   % 10;
                    digitos[3] =  codigo          % 10;
                    snprintf(msg_frag, sizeof(msg_frag), "Fragmento %d/4 coletado! Dígito: %d",
                             fase.ultimo_fragmento + 1, digitos[fase.ultimo_fragmento]);
                    timer_msg_frag        = 2.5f;
                    fase.ultimo_fragmento = -1; //reseta pra não mostrar de novo no próximo frame
                }

                // Abre o terminal ao pisar nele e apertar ENTER com todos os fragmentos coletados
                if (!entrada_terminal && sala->grid[jogador->pos.y][jogador->pos.x].tipo == TILE_TERMINAL) {
                    if (IsKeyPressed(KEY_ENTER) && fase.digitos_coletados == 0b1111) { //só abre se os 4 fragmentos foram coletados
                        entrada_terminal = true;
                        codigo_terminal[0] = '\0';
                        codigo_terminal_len = 0;
                    }
                }

                // Input do terminal: jogador digita o código de 4 dígitos
                if (entrada_terminal) {
                    int c = GetCharPressed();
                    while (c > 0) {
                        if (c >= '0' && c <= '9' && codigo_terminal_len < 4) { //aceita só dígitos, máximo 4
                            codigo_terminal[codigo_terminal_len++] = (char)c;
                            codigo_terminal[codigo_terminal_len]   = '\0';
                        }
                        c = GetCharPressed();
                    }
                    if (IsKeyPressed(KEY_BACKSPACE) && codigo_terminal_len > 0) {
                        codigo_terminal[--codigo_terminal_len] = '\0';
                    }
                    if (IsKeyPressed(KEY_ESCAPE)) { //ESC cancela sem ativar nada
                        entrada_terminal = false;
                    }
                    if (IsKeyPressed(KEY_ENTER) && codigo_terminal_len == 4) {
                        int tentativa = 0; //converte string de dígitos pra inteiro
                        for (int i = 0; i < codigo_terminal_len; i++) {
                            tentativa = tentativa * 10 + (codigo_terminal[i] - '0');
                        }
                        if (tentativa == fase.codigo_autodestruicao) { //acertou! começa a contagem regressiva
                            fase.codigo_ativado  = 1;
                            fase.tempo_restante  = 20.0f;
                        }
                        entrada_terminal = false; //fecha o terminal independente de acertar ou errar
                    }
                }

                // Sai pela saída final se o terminal já foi ativado
                if (!entrada_terminal && sala->grid[jogador->pos.y][jogador->pos.x].tipo == TILE_SAIDA) {
                    if (fase.codigo_ativado) { //só conta se a autodestruição foi ativada
                        sistema_transicao(&estado, ESTADO_VITORIA);
                        break;
                    }
                }

                // Troca de sala ao pisar em porta
                //mudou pouca coisa, ele pega o "centro" aproximado do jogador (jogador->pos.x/.y) e verifica se ta numa tile que seria a porta
                if (!entrada_terminal && sala->grid[jogador->pos.y][jogador->pos.x].tipo == TILE_PORTA) {
                    Direcao dir    = sala_direcao_porta(sala, jogador->pos.x, jogador->pos.y);
                    const char* proxima = sala_saida_em(sala, jogador->pos.x, jogador->pos.y);
                    if (proxima) {
                        Sala* nova = sala_obter(proxima);
                        if (nova) {
                            int hp_anterior  = jogador->hp;       //guarda energia e oxigênio antes de destruir a lista
                            int o2_anterior  = jogador->oxigenio;

                            if (entidades) { //limpa as entidades da sala q vai ser descarregada pra n quebrar a rebimboca da parafuseta, mesmo comentario se aplica pra todas as vezes q isso aparecer
                                lista_limpar(entidades);
                            }
                            sala = nova;

                            entidades = lista_criar(); //cria novas entidades na sala nova
                            lista_spawnar_sala(entidades, sala, &jogador, &fase);

                            jogador->hp       = hp_anterior; //mantém energia ao trocar de sala
                            jogador->oxigenio = o2_anterior; //mantém oxigênio ao trocar de sala
                            posicionar_jogador_apos_porta(sala, jogador, dir); //evita q o bichinho apareça dentro da parede
                        }
                    }
                }

                // Camera trava por sala
                int cameraX = (jogador->pos.x / CAMERA_LARGURA) * CAMERA_LARGURA;
                int cameraY = (jogador->pos.y / CAMERA_ALTURA)  * CAMERA_ALTURA;
                if (cameraX > sala->largura  - CAMERA_LARGURA) {
                    cameraX = sala->largura  - CAMERA_LARGURA;
                }
                if (cameraY > sala->altura - CAMERA_ALTURA)  {
                    cameraY = sala->altura - CAMERA_ALTURA;
                }
                
                // Renderizacao
                BeginDrawing();
                    ClearBackground(DARKGRAY);
                    sala_desenhar(sala, cameraX, cameraY, TAMANHO_TILE, CAMERA_LARGURA, CAMERA_ALTURA);
                    lista_desenhar(entidades, sala, cameraX, cameraY, TAMANHO_TILE);
                    hud_desenhar(score, jogador->oxigenio, jogador->hp, fase.digitos_coletados); //barras separadas: oxigênio e energia

                    // mensagem temporária de fragmento coletado (aparece no centro da tela por cima)
                    if (timer_msg_frag > 0.0f) {
                        timer_msg_frag -= dt;
                        int alpha = (timer_msg_frag < 0.5f) ? (int)(timer_msg_frag * 510) : 255; //fade out nos últimos 0.5s
                        int msg_largura = MeasureText(msg_frag, 20);
                        int msg_x = (GetScreenWidth() - msg_largura) / 2;
                        DrawText(msg_frag, msg_x, 250, 20, (Color){ 255, 180, 0, (unsigned char)alpha });
                    }

                    // contagem regressiva vermelha quando o terminal foi ativado
                    if (fase.codigo_ativado) {
                        DrawText(TextFormat("TEMPO: %.0f", fase.tempo_restante), CAMERA_LARGURA * TAMANHO_TILE / 2 - 60, 8, 24, RED);
                    }

                    // overlay do terminal: escurece a tela e mostra o campo de input do código
                    if (entrada_terminal) {
                        DrawRectangle(0, 0, CAMERA_LARGURA * TAMANHO_TILE, CAMERA_ALTURA * TAMANHO_TILE, (Color){ 0, 0, 0, 160 });
                        DrawText("TERMINAL DE AUTODESTRUIÇÃO",  180, 220, 22, SKYBLUE);
                        DrawText("Digite o código:",            180, 270, 20, LIGHTGRAY);
                        DrawText(codigo_terminal,               180, 300, 32, YELLOW);
                        DrawText("ENTER confirma codigo  |  ESC cancela", 180, 360, 15, GRAY);
                    }
                EndDrawing();
                break;
            }

            case ESTADO_PAUSE:
                tela_pause_atualizar(&estado);
                BeginDrawing();
                tela_pause_desenhar(
                    sala ? sala->arquivo : NULL,
                    jogador ? jogador->hp       : 0, //energia (dano de combate)
                    score,
                    jogador ? jogador->oxigenio : 0, //oxigênio (drena com tempo)
                    fase.digitos_coletados
                );
                EndDrawing();
                break;

            case ESTADO_RANKING:
                tela_ranking_atualizar(&estado);
                BeginDrawing();
                tela_ranking_desenhar(ranking, ranking_count);
                EndDrawing();
                break;

            case ESTADO_GAME_OVER: {
                int c = GetCharPressed();
                while (c > 0) {
                    if (c >= 32 && c < 127 && nome_input_len < 15) {
                        nome_input[nome_input_len++] = (char)c;
                        nome_input[nome_input_len] = '\0';
                    }
                    c = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && nome_input_len > 0) {
                    nome_input[--nome_input_len] = '\0';
                }

                if (IsKeyPressed(KEY_ENTER) && nome_input_len > 0) {
                    ranking_adicionar(ranking, &ranking_count, nome_input, score);
                    ranking_salvar(ranking, ranking_count);

                    if (entidades) {
                        lista_limpar(entidades);
                        entidades = NULL;
                    }
                    sala_cache_limpar();
                    sala = NULL;
                    jogador = NULL;
                    bloquear_enter_menu = true;
                    sistema_transicao(&estado, ESTADO_MENU);
                }

                BeginDrawing();
                tela_gameover_desenhar(score, nome_input);
                EndDrawing();
                break;
            }
                
            case ESTADO_VITORIA: {
                int cv = GetCharPressed();
                while (cv > 0) {
                    if (cv >= 32 && cv < 127 && nome_input_len < 15) {
                        nome_input[nome_input_len++] = (char)cv;
                        nome_input[nome_input_len] = '\0';
                    }
                    cv = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && nome_input_len > 0) {
                    nome_input[--nome_input_len] = '\0';
                }

                if (IsKeyPressed(KEY_ENTER) && nome_input_len > 0) {
                    ranking_adicionar(ranking, &ranking_count, nome_input, score);
                    ranking_salvar(ranking, ranking_count);

                    if (entidades) {
                        lista_limpar(entidades);
                        entidades = NULL;
                    }
                    sala_cache_limpar();
                    sala = NULL;
                    jogador = NULL;
                    bloquear_enter_menu = true;
                    sistema_transicao(&estado, ESTADO_MENU);
                }

                BeginDrawing();
                tela_vitoria_desenhar(score, nome_input);
                EndDrawing();
                break;
            }

            default:
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
                break;
        }
    }
    if (entidades) {
        lista_limpar(entidades);
    }
    sala_cache_limpar();
    menu_finalizar();
    sistema_descarregar_assets();
    CloseWindow();
    return 0;
}
