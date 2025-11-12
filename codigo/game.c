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
#define MAX_SPEED 3.5f

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

/* ========== Entrada de Teclas ========== */

void processar_input(GameState *game) {
    if (!keyhit()) return;
    
    int ch = readch();
    
    // ===== MENU =====
    if (game->status == MENU) {
        if (ch == ' ') game->status = PLAYING;
        else if (ch == 'q' || ch == 'Q') game->quit = true;
        else if (ch == 'r' || ch == 'R') resetar_placar(game);
        return;
    }
    
    // ===== GAME OVER =====
    if (game->status == GAME_OVER && (ch == 'q' || ch == 'Q')) {
        game->quit = true;
        return;
    }

    // ===== PLAYING =====
    if (game->status == PLAYING) {
        switch(ch) {
            case 'w': 
                if (game->raquete_esquerda > 1) game->raquete_esquerda--; 
                break;
            case 's': 
                if (game->raquete_esquerda < SCREEN_HEIGHT-2) game->raquete_esquerda++; 
                break;
            case 'i': 
                if (game->raquete_direita > 1) game->raquete_direita--; 
                break;
            case 'k': 
                if (game->raquete_direita < SCREEN_HEIGHT-2) game->raquete_direita++; 
                break;

            case 'q':
            case 'Q':
                game->status = MENU;   
                resetar_bola(game);   
                return;
        }
    }
}



/* ========== Atualização do Jogo ========== */

void atualizar_jogo(GameState *game) {
    static int contador_colisoes = 0;
    if (game->status != PLAYING) return;

    game->bola_x += game->bola_dir_x;
    game->bola_y += game->bola_dir_y;

    // Colisão com bordas
    if (game->bola_y <= 0 || game->bola_y >= SCREEN_HEIGHT - 1) {
        game->bola_dir_y *= -1;
        printf("\a"); // som de batida
        fflush(stdout);
    }

    // Colisão com raquetes
    if (game->bola_x <= 1 && abs(game->bola_y - game->raquete_esquerda) <= 2) {
        float hit_pos = (game->bola_y - game->raquete_esquerda) / 2.0f;
        game->bola_dir_x = fabs(game->bola_dir_x) * 1.1f;
        game->bola_dir_y = hit_pos;
        game->bola_x = 2;
        contador_colisoes++;
        printf("\a");
        fflush(stdout);
    }

    if (game->bola_x >= SCREEN_WIDTH - 2 && abs(game->bola_y - game->raquete_direita) <= 2) {
        float hit_pos = (game->bola_y - game->raquete_direita) / 2.0f;
        game->bola_dir_x = -fabs(game->bola_dir_x) * 1.1f;
        game->bola_dir_y = hit_pos;
        game->bola_x = SCREEN_WIDTH - 3;
        contador_colisoes++;
        printf("\a");
        fflush(stdout);
    }

    // Dificuldade dinâmica
    if (contador_colisoes % 5 == 0 && fabs(game->bola_dir_x) < MAX_SPEED) {
        game->bola_dir_x *= 1.1f;
    }

    // Sistema de pontuação
    if (game->bola_x < 0 || game->bola_x >= SCREEN_WIDTH) {
        if (game->bola_x < 0) game->placar_direita++;
        else game->placar_esquerda++;
        
        printf("\a"); // som de ponto
        fflush(stdout);

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

void mostrar_estrelas() {
    for (int i = 0; i < 15; i++) {
        screenGotoxy(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
        putchar((rand() % 2) ? '.' : '*');
    }
}

void mostrar_menu(GameState *game) {
    screenClear();
    mostrar_estrelas();

    screenSetColor(YELLOW, BLACK);
    screenGotoxy(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 - 5);
    printf("====================");
    screenGotoxy(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 - 4);
    printf("    PONG DELUXE     ");
    screenGotoxy(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 - 3);
    printf("====================");
    screenSetColor(WHITE, BLACK);

    screenGotoxy(SCREEN_WIDTH/2 - 15, SCREEN_HEIGHT/2 - 1);
    printf("> ESPAÇO - Iniciar Jogo");
    screenGotoxy(SCREEN_WIDTH/2 - 15, SCREEN_HEIGHT/2);
    printf("> Q - Sair");
    screenGotoxy(SCREEN_WIDTH/2 - 15, SCREEN_HEIGHT/2 + 1);
    printf("> R - Resetar Placar");

    if ((time(NULL) % 2) == 0) {
        screenSetColor(GREEN, BLACK);
        screenGotoxy(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 + 3);
        printf(">>> PRESSIONE ESPAÇO <<<");
        screenSetColor(WHITE, BLACK);
    }

    // Melhores partidas
    screenGotoxy(SCREEN_WIDTH/2 - 8, SCREEN_HEIGHT/2 + 6);
    printf("MELHORES PARTIDAS:");
    ScoreNode *atual = game->historico_placar;
    int count = 0;
    while (atual != NULL && count < 3) {
        screenGotoxy(SCREEN_WIDTH/2 - 6, SCREEN_HEIGHT/2 + 8 + count);
        printf("%d. %2d - %-2d", count + 1, atual->placar_esquerda, atual->placar_direita);
        atual = atual->next;
        count++;
    }
}

void mostrar_game_over(GameState *game) {
    screenClear();
    mostrar_estrelas();

    const char* vencedor_msg = game->jogador_vencedor == 1 ? 
        "JOGADOR 1 VENCEU!" : "JOGADOR 2 VENCEU!";

    screenSetColor(YELLOW, BLACK);
    screenGotoxy(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 - 2);
    printf("🏆  %s  🏆", vencedor_msg);
    screenSetColor(WHITE, BLACK);

    screenGotoxy(SCREEN_WIDTH/2 - 7, SCREEN_HEIGHT/2);
    printf("Placar: %d - %d", game->placar_esquerda, game->placar_direita);

    screenGotoxy(SCREEN_WIDTH/2 - 10, SCREEN_HEIGHT/2 + 2);
    printf("Pressione Q para sair");
}

void renderizar(GameState *game) {
    screenClear();

    if (game->status == PLAYING) {
        screenSetColor(WHITE, BLACK);

        // Bola
        screenSetColor(YELLOW, BLACK);
        screenGotoxy((int)game->bola_x, (int)game->bola_y);
        putchar('O');

        // Raquetes
        screenSetColor(WHITE, BLUE);
        for (int i = -1; i <= 1; i++) {
            int e = game->raquete_esquerda + i;
            if (e >= 0 && e < SCREEN_HEIGHT) {
                screenGotoxy(0, e);
                putchar('|');
            }
        }

        screenSetColor(WHITE, RED);
        for (int i = -1; i <= 1; i++) {
            int d = game->raquete_direita + i;
            if (d >= 0 && d < SCREEN_HEIGHT) {
                screenGotoxy(SCREEN_WIDTH-1, d);
                putchar('|');
            }
        }

        // Placar
        screenSetColor(GREEN, BLACK);
        char placar[10];
        sprintf(placar, "%d - %d", game->placar_esquerda, game->placar_direita);
        screenGotoxy(SCREEN_WIDTH/2 - 3, 0);
        printf("%s", placar);
        screenSetColor(WHITE, BLACK);

    } else if (game->status == MENU) {
        mostrar_menu(game);
    } else if (game->status == GAME_OVER) {
        mostrar_game_over(game);
    }

    screenUpdate();
}

/* ========== Placar e Memória ========== */

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
    ScoreNode *a = game->historico_placar;
    while (a != NULL) {
        fwrite(a, sizeof(ScoreNode), 1, file);
        a = a->next;
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
        *novo = temp;
        novo->next = game->historico_placar;
        game->historico_placar = novo;
    }
    fclose(file);
}

void resetar_placar(GameState *game) {
    ScoreNode *a = game->historico_placar;
    while (a) {
        ScoreNode *t = a;
        a = a->next;
        free(t);
    }
    game->historico_placar = NULL;
    remove(SCORES_FILE);
    
    screenClear();
    screenGotoxy(SCREEN_WIDTH/2 - 15, SCREEN_HEIGHT/2);
    printf("Placares resetados com sucesso!");
    screenUpdate();
    usleep(1000000);
}

void liberar(GameState *game) {
    if (game->campo) {
        for (int i = 0; i < SCREEN_HEIGHT; i++) free(game->campo[i]);
        free(game->campo);
    }
    ScoreNode *a = game->historico_placar;
    while (a) {
        ScoreNode *t = a;
        a = a->next;
        free(t);
    }
}