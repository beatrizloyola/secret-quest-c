# Secret Quest

<img src="https://upload.wikimedia.org/wikipedia/en/7/76/Secret_Quest_cover.jpg" align="right" width="180" style="padding-left: 10px; padding-bottom: 5px"/>

Jogo de exploração top-down em C com raylib, baseado em Secret Quest (Atari, 1989). Desenvolvido como projeto da disciplina de PIF (Programação Imperativa e Funcional) da CESAR School, turma A, 2026.1.

## Sobre

O jogador explora uma estação espacial com 16 salas interligadas, enfrenta inimigos alienígenas e coleta 4 fragmentos do código de autodestruição. Com o código completo, ativa o terminal e precisa escapar pela saída antes que a estação exploda.

## Link de gameplay:

https://youtu.be/nuajGfqd0wY

## Funcionalidades

- Mapa com 16 salas navegáveis, cada uma em arquivo `.txt`
- Inimigos com patrulha, perseguição e combate corpo a corpo
- 4 fragmentos de código espalhados pelo mapa
- Terminal de autodestruição com código de 4 dígitos
- Contagem regressiva de 20s após ativar o terminal
- Dois recursos independentes: **energia** (gasta ao atacar) e **oxigênio** (drena com tempo e dano de inimigo)
- Drops de oxigênio e energia ao matar inimigos
- Ranking com top 5 scores persistido em arquivo binário
- Menus: Iniciar, Ranking, Sair

## Controles

| Tecla | Ação |
|---|---|
| ← ↑ → ↓ | Mover |
| `SPACE` | Atacar (custa 1 de energia) |
| `ENTER` | Interagir com terminal |
| `ESC` | Pausar / cancelar terminal |

## Como compilar

### Linux
**Dependências:** [raylib](https://www.raylib.com/) instalado em `/usr/local`.

```bash
make
./game
```

### Windows
**Dependências:** w64devkit em `C:\raylib\w64devkit`, raylib em `C:\raylib\raylib\src`.

1. Compile (pode usar o terminal do w64devkit ou o integrado do VS Code):
   ```bash
   gcc main.c entidade/entidade.c mapa/mapa.c sistema/sistema.c -o game.exe -IC:/raylib/raylib/src -LC:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
   ```
2. Execute:
   ```bash
   .\game.exe
   ```

Para limpar o executável em qualquer plataforma:

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
sistema/
    sistema.c / sistema.h
assets/
    background.png
    backHUD.png
    backOVER.png
    player_up/down/left/right.png
highscores.dat
Makefile
```

## Formato das salas

Cada sala é um arquivo `.txt` com grid de caracteres:

```
12 10
N:mapa/salas/salaX.txt S: L:mapa/salas/salaY.txt O:
#####DD#####
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
| `I` | Item (score) |
| `D` | Porta |
| `a` `b` `c` `d` | Fragmentos do código (0–3) |
| `T` | Terminal de autodestruição |
| `X` | Saída da estação |

A primeira linha define o tamanho da sala. A segunda linha define as saídas em cada direção (N/S/L/O).

## Conceitos de C aplicados

- Structs e enums
- Ponteiros e passagem por referência
- Alocação dinâmica (`malloc`/`free`)
- Lista encadeada (entidades)
- Matriz dinâmica 2D (grid da sala)
- Leitura e escrita em arquivo binário (ranking)

## Equipe

| Dev | Módulo |
|---|---|
| Beatriz Loyola | `mapa.c` — parser, colisão, renderização, transição de salas |
| Daniel Donaire | `entidade.c` — lista encadeada, combate, IA |
| Pedro Bedor | `sistema.c` — menus, HUD, ranking |
