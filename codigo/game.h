#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "../cli-lib/include/screen.h"
#include "../cli-lib/include/keyboard.h"
#include "../cli-lib/include/timer.h"


#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24

typedef enum {
    MENU,
    PLAYING,
    GAME_OVER
} GameStatus;

typedef struct ScoreNode {
    int placar_esquerda, placar_direita;
    struct ScoreNode *next;
} ScoreNode;

typedef struct {
    int raquete_esquerda, raquete_direita;
    int bola_x, bola_y;
    int bola_dir_x, bola_dir_y;
    int placar_esquerda, placar_direita;
    bool quit;
    GameStatus status;
    int jogador_vencedor;
    char **campo;
    ScoreNode *historico_placar;
} GameState;

// Funções principais
void jogo_inicio(GameState *game);
void processar_input(GameState *game);
void atualizar_jogo(GameState *game);
void renderizar(GameState *game);
void liberar(GameState *game);

// Funções de placar
void add_placar_historico(GameState *game);
void salvar_placar(GameState *game);
void carregar_placar(GameState *game);
void resetar_placar(GameState *game);

#endif