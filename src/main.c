#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static float TextToFloat(const char* text) {
    return atof(text);
}

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "tinyexpr.h"

#define SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720 


static double EvaluateExpression(const char *expr, double x) {
    double result = 0.0;
    int err;
    te_variable vars[] = {{"x", &x}};
    te_expr *n = te_compile(expr, vars, 1, &err);

    if (n) {
        result = te_eval(n);
        te_free(n);
    } else {
        printf("TinyExpr Error: %d\n", err);
    }
    return result;
}

static void PreprocessExpression(char* expr) {
    if (!expr) return;
    int len = strlen(expr);
    char buffer[2048] = {0};
    int j = 0;

    for (int i = 0; i < len; i++) {
        if (isdigit(expr[i]) && (i + 1 < len) && isalpha(expr[i + 1])) {
            buffer[j++] = expr[i];
            buffer[j++] = '*';
        } else {
            buffer[j++] = expr[i];
        }
    }
    strcpy(expr, buffer);
}

static void DrawGridG() {
    int spacing = 50;     
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;

    for (int i = 0; i < SCREEN_WIDTH; i += spacing) {
        DrawLine(i, 0, i, SCREEN_HEIGHT, LIGHTGRAY);
    }

    for (int i = 0; i < SCREEN_HEIGHT; i += spacing) {
        DrawLine(0, i, SCREEN_WIDTH, i, LIGHTGRAY);
    }

    DrawLine(centerX, 0, centerX, SCREEN_HEIGHT, BLACK);
    DrawLine(0, centerY, SCREEN_WIDTH, centerY, BLACK);
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Graphing Calculator");
    SetTargetFPS(60);

    char inputExpr[1024] = {0};

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            DrawGridG();

            GuiLabel((Rectangle){ 10, 10, 780, 25 }, "Enter a function f(x):");
            if (GuiTextBox((Rectangle){ 10, 40, 780, 30 }, inputExpr, 1024, true)) {
                PreprocessExpression(inputExpr);  // Preprocess for implicit multiplication
            }

            int lastX = 0, lastY = 0;
            for (int i = 0; i < SCREEN_WIDTH; i++) {
                double x = (i - SCREEN_WIDTH / 2) / 25.0;
                double y = EvaluateExpression(inputExpr, x) * 25 + SCREEN_HEIGHT / 2;

                if (i != 0) {
                    DrawLine(lastX, lastY, i, (int)y, RED);
                }

                lastX = i;
                lastY = (int)y;
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

