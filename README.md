# Pong no Terminal

Um **Pong** clássico que roda no **terminal**, escrito em **C** e usando **CLI-lib** para tela, teclado e timer. Projeto didático, leve e divertido.

## ✨ Recursos
- Renderização no terminal com **cores** (bola, raquetes e placar)  
- **Menu animado** (efeito de estrelas e “Pressione Espaço”)  
- **Som** (beep) em colisões e pontuação  
- **Dificuldade dinâmica** (a bola acelera conforme o rally aumenta)  
- **Tela de Game Over** estilizada  
- **Histórico de placares** persistente (`pong_scores.dat`)

## 🕹️ Controles
- **W / S** – mover raquete esquerda  
- **I / K** – mover raquete direita  
- **Espaço** – iniciar partida (menu)  
- **R** – resetar histórico de placares (menu)  
- **Q** – sair (menu ou game over)

## 🚀 Como compilar e rodar
Pré-requisitos: **GCC** ou **Clang** (macOS, Linux ou WSL).

```bash
make       # compila
make run   # compila e executa
make clean # remove objetos (.o) e binário
```

O executável gerado se chama **`pong`**.

## 👤 Autores
- **Pedro Marinho**
- **Guilherme Baltar**
- **Arthur Vonsosten**


## 📜 Licença
MIT 
