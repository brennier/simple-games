#include <raylib.h>
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h> 
#include <time.h>
#include <string.h>

#define BLOCKSIZE 4
#define SCREENWIDTH 1600
#define SCREENHEIGHT 900
#define MAX_X (SCREENWIDTH / BLOCKSIZE)
#define MAX_Y (SCREENHEIGHT / BLOCKSIZE)

Color grid[MAX_X][MAX_Y];
Color next_grid[MAX_X][MAX_Y];

void reset_next_grid()
{
    for (int i = 0; i < MAX_X; i++)
        for (int j = 0; j < MAX_Y; j++)
            next_grid[i][j] = BLACK;
}

bool is_black(Color color)
{
    return ColorIsEqual(color, BLACK);
}

bool is_color(Color color)
{
    return !is_black(color);
}

int main() {
    srand(time(NULL));
    float hue = 0;
    for (int i = 0; i < MAX_X; i++)
        for (int j = 0; j < MAX_Y; j++)
            grid[i][j] = next_grid[i][j] = BLACK;

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Sand Simulation");
    SetTargetFPS(240);

    while (WindowShouldClose() != true)
    {
        Color color = ColorFromHSV((int)hue, 1.0, 1.0);
        memcpy(grid, next_grid, sizeof(grid));
        reset_next_grid();
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            grid[GetMouseX() / BLOCKSIZE][GetMouseY() / BLOCKSIZE] = color;
            hue = Wrap(hue+.2, 0, 360);
        }

        for (int x = 0; x < MAX_X; x++)
            for (int y = 0; y < MAX_Y; y++)
            {
                if (ColorIsEqual(grid[x][y], BLACK))
                    continue;
                
                int random = (rand() % 2) ? 1 : -1;

                if (y != MAX_Y - 1 && is_black(grid[x][y+1]))
                    next_grid[x][y+1] = grid[x][y];
                else if (y == MAX_Y - 1)
                    next_grid[x][y] = grid[x][y];
                else if (is_color(grid[x][y+1]) && is_color(grid[x+1][y+1]) && is_color(grid[x-1][y+1]))
                    next_grid[x][y] = grid[x][y];
                else if (is_black(grid[x+random][y+1]))
                    next_grid[x+random][y+1] = grid[x][y];
                else if (is_black(grid[x-random][y+1]))
                    next_grid[x-random][y+1] = grid[x][y];
            }

        BeginDrawing();
        ClearBackground(BLACK);

        for (int x = 0; x < MAX_X; x++)
            for (int y = 0; y < MAX_Y; y++)
                DrawRectangle(x * BLOCKSIZE, y * BLOCKSIZE, BLOCKSIZE - 1, BLOCKSIZE - 1, next_grid[x][y]);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}