# Secret Quest

<img src="https://upload.wikimedia.org/wikipedia/en/7/76/Secret_Quest_cover.jpg" align="right" width="180" style="padding-left: 10px; padding-bottom: 5px"/>

Jogo de exploração top-down em C com raylib, baseado em Secret Quest (Atari, 1989). Desenvolvido como projeto da disciplina de PIF (Programação Imperativa e Funcional) da CESAR School, turma A, 2026.1.

## Sobre

O jogador explora uma estação espacial com 16 salas interligadas, enfrenta inimigos alienígenas e coleta o Código da Bomba para completar a missão. O progresso é salvo e os melhores scores ficam no ranking.

## Funcionalidades

- Mapa com 16 salas navegáveis, cada uma em arquivo `.txt`
- Inimigos com patrulha e combate corpo a corpo
- Item colecionável (Código da Bomba)
- Sistema de save/load (sala atual, vida, score)
- Ranking com top 5 scores persistido em arquivo binário
- Menus: Iniciar, Ranking, Sair

## Controles

| Tecla | Ação |
|---|---|
| ← ↑ → ↓ | Mover |
| `SPACE` | Atacar |
| `ESC` | Pausar |

## Como compilar

**Dependências:** [raylib](https://www.raylib.com/) instalado em `/usr/local`.

```bash
make
./game
```

Para limpar o executável:

```bash
make clean
```

## Estrutura do projeto

```
main.c
types.h
mapa/
    mapa.c / mapa.h
    salas/
        sala1.txt ... sala16.txt
entidade/
    entidade.c / entidade.h
sistema.c / sistema.h
assets/
    sprites.png
highscores.dat
savegame.dat
Makefile
```

## Formato das salas

Cada sala é um arquivo `.txt` com grid de caracteres:

```
12 10
N:mapa/salas/salaX.txt S: L:mapa/salas/salaY.txt O:
############
#..........#
#....S.....#
#..........#
#.E........D
#......I...D
#..........#
#..........#
#..........#
############
```

| Caractere | Significado |
|---|---|
| `#` | Parede |
| `.` | Chão |
| `S` | Spawn do jogador |
| `E` | Spawn de inimigo |
| `I` | Item |
| `D` | Porta |

A primeira linha define o tamanho da sala. A segunda linha define as saídas em cada direção (N/S/L/O).

## Conceitos de C aplicados

- Structs e enums
- Ponteiros e passagem por referência
- Alocação dinâmica (`malloc`/`free`)
- Lista encadeada (entidades)
- Matriz dinâmica 2D (grid da sala)
- Leitura/escrita em arquivo texto e binário (mapa e save)

## Equipe

| Dev | Módulo |
|---|---|
| Beatriz Loyola | `mapa.c` — parser, colisão, renderização, transição de salas |
| Daniel Donaire | `entidade.c` — lista encadeada, combate |
| Pedro Bedor | `sistema.c` — menus, HUD, save/load, ranking |
