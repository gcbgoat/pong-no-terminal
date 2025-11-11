#include "game.h"
#include "../cli-lib/include/screen.h"
#include "../cli-lib/include/keyboard.h"
#include "../cli-lib/include/timer.h"


int main() {
    // Inicializa sistemas da CLI-LIB
    screenInit(1);
    screenHideCursor();
    keyboardInit();
    timerInit(20);  // ~14 FPS

    GameState game;
    jogo_inicio(&game);

    while (!game.quit) {
        processar_input(&game);

        if (timerTimeOver()) {  // Atualiza no ritmo do timer
            atualizar_jogo(&game);
            renderizar(&game);
        }
    }

    // Limpeza
    keyboardDestroy();
    screenDestroy();
    liberar(&game);
    return 0;
}