#include "raylib.h"

int main(void)
{
    const int larguraTela = 800;
    const int alturaTela = 800;

    InitWindow(larguraTela, alturaTela, "Jojinho");
    SetTargetFPS(60);

    while(!WindowShouldClose()){
        // Update
            // Coordenadas do player, física, etc

        // Draw
            // Renderiza as coisas na tela
        BeginDrawing();

    
            ClearBackground(RAYWHITE); // Sempre limpar o background antes!

            char *texto = "Texto centralizado!";

            int alturaTexto = 20; // Tamanho da fonte
            int larguraTexto = MeasureText(texto, alturaTexto);
            // Se for com fonte importada, usar MeasureTextEx(), que retorna um x (largura) e um y (altura)

            int xTexto = (larguraTela - larguraTexto)/2;
            int yTexto = (alturaTela - alturaTexto)/2;

            DrawText(texto, xTexto, yTexto, alturaTexto, PINK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}