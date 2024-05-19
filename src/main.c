#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_FUNCTIONS 10
#define SIDEBAR_WIDTH 200
#define ZOOM_BUTTON_SIZE 50
#define MIN_ZOOM_LEVEL 1
#define MAX_ZOOM_LEVEL 10

static float TextToFloat(const char* text) {
    return atof(text);
}

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "tinyexpr.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

static int zoomLevel = 5;
static int gridSpacing;
static float guiTextScale = 1.0f;

static void PreprocessExpression(char* expr);
static double EvaluateExpression(const char* expr, double x);

static void DrawGridG() {
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    gridSpacing = 50 / zoomLevel;

    for (int i = centerX % gridSpacing; i < SCREEN_WIDTH; i += gridSpacing) {
        DrawLine(i, 0, i, SCREEN_HEIGHT, LIGHTGRAY);
    }

    for (int i = centerY % gridSpacing; i < SCREEN_HEIGHT; i += gridSpacing) {
        DrawLine(0, i, SCREEN_WIDTH, i, LIGHTGRAY);
    }

    DrawLine(centerX, 0, centerX, SCREEN_HEIGHT, BLACK);
    DrawLine(0, centerY, SCREEN_WIDTH, centerY, BLACK);
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Graphing Calculator");
    SetTargetFPS(60);

    char inputExpr[1024] = {0};
    char expressions[MAX_FUNCTIONS][1024] = {0};
    int numFunctions = 0;
    Color colors[MAX_FUNCTIONS] = {RED, GREEN, BLUE, YELLOW, PURPLE, ORANGE, PINK, BROWN, LIME, GOLD};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        DrawGridG();

        DrawRectangle(0, 0, SIDEBAR_WIDTH, SCREEN_HEIGHT, LIGHTGRAY);
        GuiLabel((Rectangle){10, 10, SIDEBAR_WIDTH - 20, 25}, "Functions:");

        int startY = 40;
        for (int i = 0; i < numFunctions; i++) {
            char functionLabel[1028] = {0};
            sprintf(functionLabel, "f(x) = %s", expressions[i]);
            DrawRectangleLines(5, startY, SIDEBAR_WIDTH - 10, 25, colors[i]);
            GuiLabel((Rectangle){10, startY, SIDEBAR_WIDTH - 20, 25}, functionLabel);
            startY += 30;
        }

        GuiLabel((Rectangle){10, startY + 10, SIDEBAR_WIDTH - 20, 25}, "Enter a new function:");
        if (GuiTextBox((Rectangle){10, startY + 40, SIDEBAR_WIDTH - 20, 30}, inputExpr, 1024, true)) {
            if (strlen(inputExpr) > 0 && numFunctions < MAX_FUNCTIONS) {
                PreprocessExpression(inputExpr);
                strcpy(expressions[numFunctions], inputExpr);
                numFunctions++;
                memset(inputExpr, 0, sizeof(inputExpr));
            }
        }

        Rectangle zoomInRect = { SCREEN_WIDTH - SIDEBAR_WIDTH - ZOOM_BUTTON_SIZE - 10, SCREEN_HEIGHT - ZOOM_BUTTON_SIZE - 10, ZOOM_BUTTON_SIZE, ZOOM_BUTTON_SIZE };
        Rectangle zoomOutRect = { SCREEN_WIDTH - ZOOM_BUTTON_SIZE - 10, SCREEN_HEIGHT - ZOOM_BUTTON_SIZE - 10, ZOOM_BUTTON_SIZE, ZOOM_BUTTON_SIZE };
        DrawRectangleLines(zoomInRect.x, zoomInRect.y, zoomInRect.width, zoomInRect.height, GRAY);
        DrawRectangleLines(zoomOutRect.x, zoomOutRect.y, zoomOutRect.width, zoomOutRect.height, GRAY);
        GuiDrawText("+", (Rectangle){ zoomInRect.x, zoomInRect.y, ZOOM_BUTTON_SIZE, ZOOM_BUTTON_SIZE }, TEXT_ALIGN_CENTER, BLACK);
        GuiDrawText("-", (Rectangle){ zoomOutRect.x, zoomOutRect.y, ZOOM_BUTTON_SIZE, ZOOM_BUTTON_SIZE }, TEXT_ALIGN_CENTER, BLACK);        
        if (GuiButton(zoomInRect, "")) {
            if (zoomLevel < MAX_ZOOM_LEVEL) {
                zoomLevel++;
            }
        }

        if (GuiButton(zoomOutRect, "")) {
            if (zoomLevel > MIN_ZOOM_LEVEL) {
                zoomLevel--;
            }
        }

        for (int i = 0; i < numFunctions; i++) {
            int lastX = 0, lastY = 0;
            for (int j = SIDEBAR_WIDTH; j < SCREEN_WIDTH; j++) {
                double x = (j - SCREEN_WIDTH / 2.0) / (25.0 * zoomLevel);
                double y = EvaluateExpression(expressions[i], x) * (25.0 * zoomLevel) + SCREEN_HEIGHT / 2;

                if (j != SIDEBAR_WIDTH) {
                    DrawLine(lastX, lastY, j, (int)y, colors[i]);
                }

                lastX = j;
                lastY = (int)y;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

static double EvaluateExpression(const char* expr, double x) {
    double result = 0.0;
    int err;
    te_variable vars[] = {{"x", &x}};

    char buffer[2048] = {0};
    strcpy(buffer, expr);
    PreprocessExpression(buffer);

    te_expr* n = te_compile(buffer, vars, 1, &err);
    if (n) {
        result = -te_eval(n); // negate cuz i think there is some bug in my logic
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
