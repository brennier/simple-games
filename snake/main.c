#include <raylib.h>
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h> 
#include <math.h>

#define MAX_SNAKE_LENGTH 100
#define LIGHTGREEN (Color){ 120, 255, 120, 255 }
const int screenWidth = 800;
const int screenHeight = 800;

enum mode {
    NORMAL,
    SLOWMO,
};

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int min(int a, int b)
{
    return (a > b) ? a : b;
}

double distance(Vector2 vec1, Vector2 vec2) {
    double xdiff = (vec2.x - vec1.x);
    double ydiff = (vec2.y - vec1.y);
    return sqrt(xdiff * xdiff + ydiff * ydiff);
}

enum Directions {
    RIGHT,
    LEFT,
    UP,
    DOWN
};

struct Snake {
    Vector2 positions[MAX_SNAKE_LENGTH];
    enum Directions direction;
    int length;
};

typedef struct Snake Snake;

void drawSnake(Snake snake) {
    for (int i = 0; i < snake.length; i++)
    {
        DrawRectangle(snake.positions[i].x * 16, snake.positions[i].y * 16, 16, 16, WHITE);
        // Draw the outline around the rectangle
        DrawRectangle(snake.positions[i].x * 16, snake.positions[i].y * 16, 16, 2, LIGHTGREEN);
        DrawRectangle(snake.positions[i].x * 16, snake.positions[i].y * 16, 2, 16, LIGHTGREEN);
        DrawRectangle(snake.positions[i].x * 16 + 14, snake.positions[i].y * 16, 2, 16, LIGHTGREEN);
        DrawRectangle(snake.positions[i].x * 16, snake.positions[i].y * 16 + 14, 16, 2, LIGHTGREEN);
    }
}

Snake updateSnake(Snake snake) {
    for (int i = snake.length - 1; i >= 0; i--)
        snake.positions[i+1] = snake.positions[i];
    switch (snake.direction) {
        case UP:
            snake.positions[0] = Vector2Add(snake.positions[1], (Vector2){0,-1});
            break;
        case DOWN:
            snake.positions[0] = Vector2Add(snake.positions[1], (Vector2){0,1});
            break;
        case LEFT:
            snake.positions[0] = Vector2Add(snake.positions[1], (Vector2){-1,0});
            break;
        case RIGHT:
            snake.positions[0] = Vector2Add(snake.positions[1], (Vector2){1,0});
            break;
    }
    return snake;
}

int main() {
    srand(0);
    enum mode mode = NORMAL;
    char message[200];
    int frameCounter = 0;
    Vector2 apple = (Vector2){ rand() % 50, rand() % 40 + 10 };
    Snake snake;
    int powerup = 100;
    snake.direction = UP;
    snake.length = 1;
    snake.positions[0] = (Vector2){25, 25};

    InitWindow(screenWidth, screenHeight, "Snake");
    SetTargetFPS(60);

    while (WindowShouldClose() != true)
    {

        if (IsKeyDown(KEY_UP) && snake.direction != DOWN)
            snake.direction = UP;
        else if (IsKeyDown(KEY_DOWN) && snake.direction != UP)
            snake.direction = DOWN;
        else if (IsKeyDown(KEY_LEFT) && snake.direction != RIGHT)
            snake.direction = LEFT;
        else if (IsKeyDown(KEY_RIGHT) && snake.direction != LEFT)
            snake.direction = RIGHT;  

        if (IsKeyDown(KEY_SPACE) && powerup > 0)
            mode = SLOWMO;
        else
            mode = NORMAL;

        if (mode == SLOWMO && frameCounter % 8 == 0)
        {
            snake = updateSnake(snake);
            powerup -= 2;
        }
        
        if (mode == NORMAL && frameCounter % 2 == 0)
        {
            snake = updateSnake(snake);
            if (powerup < 100)
                powerup++;
        }
        
        if (Vector2Equals(snake.positions[0], apple))
        {
            apple = (Vector2){ rand() % 50, rand() % 40 + 10 };
            snake.length++;
        }

        if (screenWidth / 16 == snake.positions[0].x ||
            snake.positions[0].x < 0 ||
            screenHeight / 16 < snake.positions[0].y ||
            snake.positions[0].y < 0)
        {
            break;
        }

        BeginDrawing();
        ClearBackground(GREEN);
        drawSnake(snake);
        DrawRectangle(apple.x * 16, apple.y * 16, 16, 16, RED);
        DrawRectangle(20, 20, 510, 30, LIGHTGREEN);
        DrawRectangle(25, 25, 5*powerup, 20, ORANGE);
        DrawText("Press [Space] to slow down time", 20, 70, 24, BLACK);
        sprintf(message, "Score: %d", snake.length);
        DrawText(message, 20, screenHeight - 32, 32, BLACK);
        EndDrawing();

        frameCounter++;
    }

    // Shift the snake back one
    for (int i = 0; i < snake.length; i++)
        snake.positions[i] = snake.positions[i+1];

    while (WindowShouldClose() != true)
    {
        BeginDrawing();
        ClearBackground(GREEN);
        drawSnake(snake);
        DrawRectangle(apple.x * 16, apple.y * 16, 16, 16, RED);
        sprintf(message, "Final score: %03d", snake.length);
        DrawText(message, screenWidth / 2 - 16*16, screenHeight / 2, 64, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}