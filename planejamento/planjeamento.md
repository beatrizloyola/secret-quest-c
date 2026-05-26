# Plano de Desenvolvimento — Secret Quest

## Visão Geral do Escopo

| Feature | Detalhe |
|---|---|
| Mapa | 1 estação com 12 salas (idêntica a do jogo original) |
| Arquivo de mapa | Cada sala é um arquivo `.txt` com grid de caracteres |
| Inimigos | 1 tipo apenas (alien simples com patrulha básica) |
| Combate | Corpo a corpo: botão de ataque + hitbox curta + cooldown |
| Itens | Apenas 1 tipo: `"Código da Bomba"` |
| Menus | Iniciar, Ranking, Sair |
| Save/Load | Salva sala atual, vida do jogador e score |
| Ranking | Top 5 scores persistidos em arquivo binário |
| Arte | Spritesheet carregado via raylib (`LoadTexture`) |

---

# Formato do Arquivo de Sala (`.txt`)

Cada arquivo representa uma sala.

Exemplo: `sala_01.txt`

```txt
12 10
############
#..........#
#.E........#
#..........#
#..........#
#..........#
#......I...#
#..........#
#....S.....#
############
````

## Legenda

| Caractere | Significado              |
| --------- | ------------------------ |
| `#`       | Parede (sólido)          |
| `.`       | Chão (vazio)             |
| `S`       | Spawn do jogador         |
| `E`       | Spawn de inimigo         |
| `I`       | Item (`Código da Bomba`) |
| `D`       | Porta para outra sala    |

---

# Estrutura de Arquivos

```txt
main.c

types.h

mapa.c
mapa.h

entidade.c
entidade.h

sistema.c
sistema.h

sala_01.txt
sala_02.txt
...

assets/
    sprites.png
    fonte.png

highscores.dat
savegame.dat

Makefile
```

---

# types.h

```c
#ifndef TYPES_H
#define TYPES_H

typedef struct {
    int x, y;
} Vec2i;

typedef enum {
    TILE_FLOOR,
    TILE_WALL,
    TILE_DOOR
} TileType;

typedef struct {
    TileType tipo;
    bool solido;
} Tile;

typedef struct {
    int largura, altura;
    Tile** grid;

    Vec2i spawn_jogador;

    char arquivo[64];
} Sala;

typedef enum {
    ENT_JOGADOR,
    ENT_INIMIGO,
    ENT_ITEM
} EntTipo;

typedef struct Entidade {
    EntTipo tipo;

    Vec2i pos;

    int hp;
    int max_hp;

    int ataque;

    bool vivo;

    int direcao;
    float timer_ataque;

    struct Entidade* prox;
} Entidade;

typedef struct {
    Entidade* cabeca;
    int contagem;
} ListaEntidades;

typedef struct {
    char nome[16];
    int score;
} Recorde;

typedef enum {
    ESTADO_MENU,
    ESTADO_JOGANDO,
    ESTADO_PAUSA,
    ESTADO_GAME_OVER,
    ESTADO_RANKING,
    ESTADO_VITORIA
} EstadoJogo;

typedef struct {
    Sala* sala_atual;

    ListaEntidades* entidades;

    Entidade* jogador;

    int score;

    EstadoJogo estado;

    int cam_x;
    int cam_y;
} Jogo;

#endif
```

---

# APIs por Dev

## Beatriz — mapa.c / mapa.h

```c
Sala* sala_carregar(const char* arquivo);

void sala_descarregar(Sala* s);

bool sala_colisao(Sala* s, int x, int y);

void sala_desenhar(
    Sala* s,
    int cam_x,
    int cam_y,
    Texture2D* sprite_parede,
    Texture2D* sprite_chao
);
```

### Responsabilidades

* Ler `.txt`
* Fazer parsing do grid
* Alocar matriz dinamicamente
* Implementar colisão
* Renderizar tiles
* Gerenciar transição entre salas

---

## Daniel — entidade.c / entidade.h

```c
ListaEntidades* lista_criar();

void lista_adicionar(
    ListaEntidades* lista,
    Entidade* e
);

void lista_remover(
    ListaEntidades* lista,
    Entidade* alvo
);

void lista_limpar(ListaEntidades* lista);

void lista_atualizar(
    ListaEntidades* lista,
    Sala* sala,
    float dt
);

void lista_desenhar(
    ListaEntidades* lista,
    int cam_x,
    int cam_y,
    Texture2D* sprites
);

Entidade* entidade_criar(
    EntTipo tipo,
    int x,
    int y
);

void entidade_atacar(
    Entidade* atacante,
    ListaEntidades* alvos,
    Sala* sala
);
```

### Responsabilidades

* Lista encadeada
* Criação/destruição de entidades
* IA simples de patrulha
* Combate
* Atualização de entidades
* Renderização

---

## Pedro — sistema.c / sistema.h

```c
void menu_desenhar(
    EstadoJogo* estado,
    int* opcao
);

void hud_desenhar(
    Entidade* jogador,
    int score
);

void ranking_carregar(
    Recorde* lista,
    int max
);

void ranking_salvar(
    Recorde* lista,
    int count
);

bool ranking_adicionar(
    Recorde* lista,
    int* count,
    const char* nome,
    int score
);

void save_salvar(
    Jogo* j,
    const char* arquivo
);

bool save_carregar(
    Jogo* j,
    const char* arquivo
);

void transicao_estado(
    Jogo* j,
    EstadoJogo novo
);
```

### Responsabilidades

* Máquina de estados
* Menus
* HUD
* Save/Load
* Ranking
* Game Over

---

# Cronograma

# Etapa 1 — Fundação e Estrutura

| Dev   | Tarefa                                   | Entregável             |
| ----- | ---------------------------------------- | ---------------------- |
| Todos | Reunião de 30min para congelar `types.h` | `types.h` commitado    |
| Beatriz | Implementar parser de `.txt`             | `mapa.c` funcional     |
| Daniel | Implementar lista encadeada + entidades  | `entidade.c` funcional |
| Pedro | Criar máquina de estados + menus básicos | `sistema.c` funcional  |

## Objetivo da Etapa 1

* `main.c` compila
* Alterna entre MENU e JOGANDO
* Estrutura pronta para integração

---

# Etapa 2 — Gameplay Core

| Dev   | Tarefa                      | Entregável             |
| ----- | --------------------------- | ---------------------- |
| Beatriz | Criar 4–6 salas e transição | Mapa navegável         |
| Daniel | Spawn + movimento + IA      | Inimigos andando       |
| Pedro | Sprites + HUD               | Tela de jogo funcional |

## Objetivo da Etapa 2

* Jogador anda
* Mapa funciona
* Inimigos se movem
* HUD aparece

---

# Etapa 3 — Combate e Sistemas

| Dev   | Tarefa                       | Entregável               |
| ----- | ---------------------------- | ------------------------ |
| Beatriz | Ajustar colisão e portas     | Portas funcionando       |
| Daniel | Implementar combate e coleta | Gameplay principal       |
| Pedro | Ranking + Save/Load          | Persistência funcionando |

## Objetivo da Etapa 3

Loop completo:

```txt
Spawn -> Exploração -> Combate -> Coleta -> Morte/Save -> Ranking
```

---

# Etapa 4 — Integração e Polimento

| Dev   | Tarefa                    | Entregável     |
| ----- | ------------------------- | -------------- |
| Todos | Integração final          | Jogo completo  |
| Beatriz | Verificar leaks no mapa   | Sem vazamentos |
| Daniel | Verificar leaks na lista  | Sem vazamentos |
| Pedro | Polimento de menus e ranking | UI final       |

---

# Checklist dos Conceitos Obrigatórios

| Conceito          | Onde está                          | Como o professor vê           |
| ----------------- | ---------------------------------- | ----------------------------- |
| Structs           | `Sala`, `Tile`, `Entidade`, `Jogo` | Uso extensivo                 |
| Ponteiros         | `Sala*`, `Entidade*`, listas       | Passagem por referência       |
| Alocação dinâmica | `malloc/free`                      | Memória dinâmica explícita    |
| Lista encadeada   | `ListaEntidades`                   | Inserção/remoção dinâmica     |
| Matrizes          | `Tile** grid`                      | Matriz dinâmica 2D            |
| Arquivos          | `.txt` + `.dat`                    | Leitura/escrita texto/binário |

---

# Considerações Importantes

## 1. Paralelismo

Se depender do parser do mapa, criar uma `Sala` mock temporária

---

## 2. Save/Load

Salvar apenas:

* Nome da sala atual
* Vida do jogador
* Score

Ao carregar:

* Reabrir `.txt`
* Reconstruir a sala

Evitar salvar matriz inteira.

---

## 3. Combate Corpo a Corpo

Sugestão:

* Botão: `SPACE`
* Hitbox: 1 tile adjacente
* Cooldown simples
* Contato inimigo = dano no jogador

---

## 4. Sprites

```c
#define TAMANHO_TILE 32
```

---