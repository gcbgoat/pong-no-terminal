#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24
#define WINNING_SCORE 10
#define SCORES_FILE "pong_scores.dat"
#define BALL_SPEED 1.8f

/* ========== Funções da Bola ========== */

void resetar_bola(GameState *game) {
    game->bola_x = SCREEN_WIDTH / 2;
    game->bola_y = SCREEN_HEIGHT / 2;
    game->bola_dir_x = (rand() % 2) ? BALL_SPEED : -BALL_SPEED;
    game->bola_dir_y = ((rand() % 3) - 1) * 0.7f;
}

/* ========== Funções Principais ========== */

void jogo_inicio(GameState *game) {
    srand(time(NULL));
    
    game->campo = malloc(SCREEN_HEIGHT * sizeof(char *));
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        game->campo[i] = malloc(SCREEN_WIDTH * sizeof(char));
        memset(game->campo[i], ' ', SCREEN_WIDTH);
    }

    game->raquete_esquerda = game->raquete_direita = SCREEN_HEIGHT / 2;
    resetar_bola(game);
    game->placar_esquerda = game->placar_direita = 0;
    game->quit = false;
    game->status = MENU;
    game->jogador_vencedor = 0;
    game->historico_placar = NULL;
    
    carregar_placar(game);
}

void processar_input(GameState *game) {
    if (!keyhit()) return;
    
    int ch = readch();
    
    if (game->status == MENU) {
        if (ch == ' ') game->status = PLAYING;
        else if (ch == 'q') game->quit = true;
        else if (ch == 'r' || ch == 'R') resetar_placar(game);
        return;
    }
    
    if (game->status == GAME_OVER && ch == 'q') {
        game->quit = true;
        return;
    }

    switch(ch) {
        case 'w': if (game->raquete_esquerda > 1) game->raquete_esquerda--; break;
        case 's': if (game->raquete_esquerda < SCREEN_HEIGHT-2) game->raquete_esquerda++; break;
        case 'i': if (game->raquete_direita > 1) game->raquete_direita--; break;
        case 'k': if (game->raquete_direita < SCREEN_HEIGHT-2) game->raquete_direita++; break;
    }
}

void atualizar_jogo(GameState *game) {
    if (game->status != PLAYING) return;

    game->bola_x += game->bola_dir_x;
    game->bola_y += game->bola_dir_y;

    // Colisão com bordas
    if (game->bola_y <= 0 || game->bola_y >= SCREEN_HEIGHT - 1) {
        game->bola_dir_y *= -1;
    }

    // Colisão com raquetes (versão otimizada)
    if (game->bola_x <= 1 && abs(game->bola_y - game->raquete_esquerda) <= 2) {
        float hit_pos = (game->bola_y - game->raquete_esquerda) / 2.0f;
        game->bola_dir_x = (game->bola_dir_x < 0 ? -game->bola_dir_x : game->bola_dir_x) * 1.1f;
        game->bola_dir_y = hit_pos;
        game->bola_x = 2; // Previne colisão múltipla
    }

    if (game->bola_x >= SCREEN_WIDTH - 2 && abs(game->bola_y - game->raquete_direita) <= 2) {
        float hit_pos = (game->bola_y - game->raquete_direita) / 2.0f;
        game->bola_dir_x = (game->bola_dir_x > 0 ? -game->bola_dir_x : game->bola_dir_x) * 1.1f;
        game->bola_dir_y = hit_pos;
        game->bola_x = SCREEN_WIDTH - 3;
    }

    // Sistema de pontuação
    if (game->bola_x < 0 || game->bola_x >= SCREEN_WIDTH) {
        if (game->bola_x < 0) game->placar_direita++;
        else game->placar_esquerda++;
        
        if (game->placar_esquerda >= WINNING_SCORE || game->placar_direita >= WINNING_SCORE) {
            game->status = GAME_OVER;
            game->jogador_vencedor = (game->placar_esquerda > game->placar_direita) ? 1 : 2;
            add_placar_historico(game);
        } else {
            resetar_bola(game);
        }
    }
}

/* ========== Renderização ========== */

void desenho_raquetes(GameState *game) {
    for (int i = -1; i <= 1; i++) {
        int esquerda = game->raquete_esquerda + i;
        if (esquerda >= 0 && esquerda < SCREEN_HEIGHT) {
            screenGotoxy(0, esquerda);
            putchar('|');
        }
        int direita = game->raquete_direita + i;
        if (direita >= 0 && direita < SCREEN_HEIGHT) {
            screenGotoxy(SCREEN_WIDTH-1, direita);
            putchar('|');
        }
    }
}

void desenho_bola(GameState *game) {
    if (game->bola_x >= 0 && game->bola_x < SCREEN_WIDTH && 
        game->bola_y >= 0 && game->bola_y < SCREEN_HEIGHT) {
        screenGotoxy((int)game->bola_x, (int)game->bola_y);
        putchar('O');
    }
}

void mostrar_menu(GameState *game) {
    screenClear();
    
    // Título centralizado com efeito
    screenGotoxy(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 - 5);
    printf("====================");
    screenGotoxy(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 - 4);
    printf("    PONG SHOWDOWN    ");
    screenGotoxy(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 - 3);
    printf("====================");

    // Opções do menu alinhadas
    screenGotoxy(SCREEN_WIDTH/2 - 15, SCREEN_HEIGHT/2 - 1);
    printf("> ESPAÇO - Iniciar Jogo");
    screenGotoxy(SCREEN_WIDTH/2 - 15, SCREEN_HEIGHT/2);
    printf("> Q - Sair");
    screenGotoxy(SCREEN_WIDTH/2 - 15, SCREEN_HEIGHT/2 + 1);
    printf("> R - Resetar Placar");

    // Linha divisória
    screenGotoxy(SCREEN_WIDTH/2 - 15, SCREEN_HEIGHT/2 + 3);
    printf("----------------------------");

    // Melhores placares
    screenGotoxy(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 + 5);
    printf("MELHORES PARTIDAS");

    ScoreNode *atual = game->historico_placar;
    int count = 0;
    while (atual != NULL && count < 3) {
        screenGotoxy(SCREEN_WIDTH/2 - 8, SCREEN_HEIGHT/2 + 7 + count);
        printf("%d. %2d - %-2d", count + 1, atual->placar_esquerda, atual->placar_direita);
        atual = atual->next;
        count++;
    }

    // Elementos decorativos nas laterais
    for (int i = 0; i < 3; i++) {
        screenGotoxy(SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2 - 4 + i);
        putchar('|');
        screenGotoxy(SCREEN_WIDTH/2 + 20, SCREEN_HEIGHT/2 - 4 + i);
        putchar('|');
    }
}

void mostrar_game_over(GameState *game) {
    screenClear();
    
    const char* vencedor_msg = game->jogador_vencedor == 1 ? 
        "JOGADOR 1 VENCEU!" : "JOGADOR 2 VENCEU!";

    // Moldura superior
    screenGotoxy(SCREEN_WIDTH/2 - 12, SCREEN_HEIGHT/2 - 5);
    printf(" _____________________ ");
    screenGotoxy(SCREEN_WIDTH/2 - 12, SCREEN_HEIGHT/2 - 4);
    printf("|                     |");

    // Título central
    screenGotoxy(SCREEN_WIDTH/2 - 5, SCREEN_HEIGHT/2 - 3);
    printf("GAME OVER");

    // Linha divisória
    screenGotoxy(SCREEN_WIDTH/2 - 12, SCREEN_HEIGHT/2 - 2);
    printf("|---------------------|");

    // Mensagem do vencedor
    screenGotoxy(SCREEN_WIDTH/2 - strlen(vencedor_msg)/2, SCREEN_HEIGHT/2 - 1);
    printf("%s", vencedor_msg);

    // Placar final
    char placar_final[30];
    sprintf(placar_final, "Placar: %d - %d", game->placar_esquerda, game->placar_direita);
    screenGotoxy(SCREEN_WIDTH/2 - strlen(placar_final)/2, SCREEN_HEIGHT/2);
    printf("%s", placar_final);

    // Moldura inferior
    screenGotoxy(SCREEN_WIDTH/2 - 12, SCREEN_HEIGHT/2 + 1);
    printf("|                     |");
    screenGotoxy(SCREEN_WIDTH/2 - 12, SCREEN_HEIGHT/2 + 2);
    printf(" --------------------- ");

    // Instrução para sair
    screenGotoxy(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 + 4);
    printf("Pressione Q para sair");

    // Efeitos decorativos nas laterais
    for (int i = 0; i < 5; i++) {
        screenGotoxy(10, SCREEN_HEIGHT/2 - 3 + i);
        putchar('[');
        screenGotoxy(SCREEN_WIDTH - 11, SCREEN_HEIGHT/2 - 3 + i);
        putchar(']');
    }
}

void renderizar(GameState *game) {
    screenClear();  // Limpa a tela primeiro

    if (game->status == PLAYING) {
        // Preenche a matriz com espaços
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            memset(game->campo[y], ' ', SCREEN_WIDTH);
        }

        // Desenha a bola
        if (game->bola_y >= 0 && game->bola_y < SCREEN_HEIGHT && 
            game->bola_x >= 0 && game->bola_x < SCREEN_WIDTH) {
            game->campo[(int)game->bola_y][(int)game->bola_x] = 'O';
        }

        // Desenha as raquetes
        for (int i = -1; i <= 1; i++) {
            int esquerda = game->raquete_esquerda + i;
            int direita = game->raquete_direita + i;
            
            if (esquerda >= 0 && esquerda < SCREEN_HEIGHT) {
                game->campo[esquerda][0] = '|';
            }
            if (direita >= 0 && direita < SCREEN_HEIGHT) {
                game->campo[direita][SCREEN_WIDTH-1] = '|';
            }
        }

        // Renderiza a matriz
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            screenGotoxy(0, y);
            fwrite(game->campo[y], sizeof(char), SCREEN_WIDTH, stdout);
        }

        // Placar (renderizado separadamente para evitar flicker)
        char placar[10];
        sprintf(placar, "%d - %d", game->placar_esquerda, game->placar_direita);
        screenGotoxy(SCREEN_WIDTH/2 - 3, 0);
        printf("%s", placar);

    } else {
        // Menus são renderizados de forma independente
        switch(game->status) {
            case MENU:
                mostrar_menu(game);
                break;
            case GAME_OVER:
                mostrar_game_over(game);
                break;
            case PLAYING:  // Adicionado para eliminar o warning
                break;
        }
    }
    
    screenUpdate();
}

/* ========== Funções de Placar ========== */

void add_placar_historico(GameState *game) {
    ScoreNode *novo = malloc(sizeof(ScoreNode));
    if (!novo) return;

    novo->placar_esquerda = game->placar_esquerda;
    novo->placar_direita = game->placar_direita;
    novo->next = game->historico_placar;
    game->historico_placar = novo;
    
    salvar_placar(game);
}

void salvar_placar(GameState *game) {
    FILE *file = fopen(SCORES_FILE, "wb");
    if (!file) return;

    ScoreNode *atual = game->historico_placar;
    while (atual != NULL) {
        fwrite(atual, sizeof(ScoreNode), 1, file);
        atual = atual->next;
    }

    fclose(file);
}

void carregar_placar(GameState *game) {
    FILE *file = fopen(SCORES_FILE, "rb");
    if (!file) return;

    ScoreNode temp;
    while (fread(&temp, sizeof(ScoreNode), 1, file) == 1) {
        ScoreNode *novo = malloc(sizeof(ScoreNode));
        if (!novo) break;
        
        novo->placar_esquerda = temp.placar_esquerda;
        novo->placar_direita = temp.placar_direita;
        novo->next = game->historico_placar;
        game->historico_placar = novo;
    }

    fclose(file);
}

void resetar_placar(GameState *game) {
    ScoreNode *atual = game->historico_placar;
    while (atual != NULL) {
        ScoreNode *temp = atual;
        atual = atual->next;
        free(temp);
    }
    game->historico_placar = NULL;
    
    remove(SCORES_FILE);
    
    if (game->status == MENU) {
        screenClear();
        screenGotoxy(SCREEN_WIDTH/2 - 15, SCREEN_HEIGHT/2 + 8);
        printf("Placares resetados com sucesso!");
        screenUpdate();
        usleep(1500000);
    }
}

void liberar(GameState *game) {
    if (game->campo) {
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            free(game->campo[i]);
        }
        free(game->campo);
    }
    
    ScoreNode *atual = game->historico_placar;
    while (atual != NULL) {
        ScoreNode *temp = atual;
        atual = atual->next;
        free(temp);
    }
}