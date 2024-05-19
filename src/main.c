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
#define GRID_SPACING 50

static void PreprocessExpression(char* expr);

static double EvaluateExpression(const char* expr, double x) {
    double result = 0.0;
    int err;
    te_variable vars[] = {{"x", &x}};

    char buffer[2048] = {0};
    strcpy(buffer, expr);
    PreprocessExpression(buffer);

    te_expr* n = te_compile(buffer, vars, 1, &err);
    if (n) {
        result = -te_eval(n);
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

    if (expr[0] == '-') {
        buffer[j++] = '-';
        if (len > 1 && isdigit(expr[1])) {
            buffer[j++] = '0';
        }
    }

    if (isdigit(expr[0]) || (expr[0] == '-' && isdigit(expr[1]))) {
        int start = (expr[0] == '-') ? 1 : 0;
        buffer[j++] = expr[start];
        if (len > start + 1 && isalpha(expr[start + 1])) {
            buffer[j++] = '*';
            buffer[j++] = expr[start + 1];
            if (len > start + 2) strcpy(buffer + j, expr + start + 2);
        } else {
            strcpy(buffer + j, expr + start + 1);
        }
    } else {
        strcpy(buffer, expr);
    }

    for (int i = 0; buffer[i]; i++) {
        if (isdigit(buffer[i]) && buffer[i + 1] && (isalpha(buffer[i + 1]) || buffer[i + 1] == '(')) {
            char temp[2048] = {0};
            strcpy(temp, buffer + i + 1);
            buffer[i + 1] = '*';
            strcpy(buffer + i + 2, temp);
        }
    }

    int i = strlen(buffer);
    if (i > 1 && isdigit(buffer[i - 2]) && isalpha(buffer[i - 1])) {
        buffer[i] = '*';
        buffer[i + 1] = '\0';
    }

    strcpy(expr, buffer);
}

static void DrawGridG() {
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;

    for (int i = centerX % GRID_SPACING; i < SCREEN_WIDTH; i += GRID_SPACING) {
        DrawLine(i, 0, i, SCREEN_HEIGHT, LIGHTGRAY);
    }
    for (int i = centerY % GRID_SPACING; i < SCREEN_HEIGHT; i += GRID_SPACING) {
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
                PreprocessExpression(inputExpr);             }

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

