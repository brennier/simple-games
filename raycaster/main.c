#include <raylib.h>
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h> 
#include <time.h>

#define SCREENWIDTH 1600
#define SCREENHEIGHT 640
#define MAPSIZE 640
#define BLOCKSIZE 32
#define RAYCASTPIXELWIDTH 8
#define VIEWANGLE 60.0 // in degrees

#define LIGHTBLUE (Color){ 135, 206, 235, 255 }

#define RAYNUMBER ( (SCREENWIDTH - MAPSIZE) / RAYCASTPIXELWIDTH )
#define MAPYOFFSET ( (SCREENHEIGHT - MAPSIZE) / 2 )
#define HALFVIEWANGLE (VIEWANGLE / 2)

const int map[20][20] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

struct Player {
    Vector2 pos;
    Vector2 angle;
    int speed;
    int size;
};

typedef struct Player Player;

void drawMap() {
    for (int i = 0; i < MAPSIZE/BLOCKSIZE; i++)
        for (int j = 0; j < MAPSIZE/BLOCKSIZE; j++)
            DrawRectangle(
                BLOCKSIZE * j,
                MAPYOFFSET + BLOCKSIZE * i,
                BLOCKSIZE - 1,
                BLOCKSIZE - 1,
                map[i][j] ? GRAY : BLACK
            );
}

void drawPlayer(Player player) {
    DrawCircleV(player.pos, player.size, YELLOW);
    // DrawLineV(player.pos, Vector2Add(player.pos, Vector2Scale(player.angle, player.size * 3)), YELLOW);
}

bool isBlock(Vector2 pixel_pos) {
    if (pixel_pos.x > MAPSIZE)
        return 0;
    int block_column = pixel_pos.x / BLOCKSIZE;
    int block_row = (pixel_pos.y - MAPYOFFSET) / BLOCKSIZE;
    return map[block_row][block_column] > 0;
}

bool isBlockFuzzy(Vector2 pixel_pos) {
    return (isBlock(Vector2Add(pixel_pos, (Vector2){+1,+1}))
         || isBlock(Vector2Add(pixel_pos, (Vector2){-1,+1}))
         || isBlock(Vector2Add(pixel_pos, (Vector2){+1,-1}))
         || isBlock(Vector2Add(pixel_pos, (Vector2){-1,-1})));
}

bool isInBlock(Player player) {
    Vector2 front_pixel = Vector2Add(player.pos, Vector2Scale(player.angle, 3 * player.size));
    return isBlock(front_pixel);
}

bool isInWindow(Vector2 pixel_pos) {
    return 0 < pixel_pos.x && pixel_pos.x < SCREENWIDTH
        && 0 < pixel_pos.y && pixel_pos.y < SCREENHEIGHT;
}

bool isInMap(Vector2 pixel_pos) {
    return 0 < pixel_pos.x && pixel_pos.x < MAPSIZE
        && MAPYOFFSET < pixel_pos.y
        && pixel_pos.y < SCREENHEIGHT - MAPYOFFSET;
}

bool playerInBlock(Vector2 next_pos, int size) {
    Vector2 angle = Vector2Scale((Vector2){1, 0}, size);

    for (int i = 0; i < 360; i += 45)
        if (isBlock(Vector2Add(next_pos, Vector2Rotate(angle, DEG2RAD * i))))
            return true;
    
    return false;
}

Player movePlayer(Player player) {
    Vector2 next_pos = player.pos;
    if (IsKeyDown(KEY_UP))
        next_pos = Vector2Add(player.pos, Vector2Scale(player.angle, player.speed));
    else if (IsKeyDown(KEY_DOWN))
        next_pos = Vector2Add(player.pos, Vector2Scale(player.angle, -1 * player.speed));

    if (playerInBlock(next_pos, player.size))
        player.pos = player.pos;
    else
        player.pos = next_pos;

    if (IsKeyDown(KEY_RIGHT))
        player.angle = Vector2Rotate(player.angle, DEG2RAD * player.speed);
    else if (IsKeyDown(KEY_LEFT))
        player.angle = Vector2Rotate(player.angle, - DEG2RAD * player.speed);
    return player;
}

Vector3 rayEndPoint(Player player, float angle_offset) {
    Vector2 angle = Vector2Rotate(player.angle, DEG2RAD * angle_offset);
    float angle_degrees = RAD2DEG * Vector2Angle(angle, (Vector2){1.0, 0.0});
    Vector2 first_vertical_wall = player.pos;
    Vector2 first_horizontal_wall = player.pos;

    if (90 <= angle_degrees || angle_degrees < -90)
    {
        first_vertical_wall.x = player.pos.x - ((int)player.pos.x % BLOCKSIZE);
        first_vertical_wall.y = player.pos.y + tan(DEG2RAD * angle_degrees) * ((int)player.pos.x % BLOCKSIZE);
        first_vertical_wall = Vector2Add(first_vertical_wall, (Vector2){-1,0});
        Vector2 delta = (Vector2){ - BLOCKSIZE, tan(DEG2RAD * angle_degrees) * BLOCKSIZE};

        while (isInMap(first_vertical_wall) && !isBlockFuzzy(first_vertical_wall))
            first_vertical_wall = Vector2Add(first_vertical_wall, delta);
    }
    else
    {
        first_vertical_wall.x = player.pos.x + (BLOCKSIZE - ((int)player.pos.x % BLOCKSIZE));
        first_vertical_wall.y = player.pos.y - tan(DEG2RAD * angle_degrees) * (BLOCKSIZE - ((int)player.pos.x % BLOCKSIZE));
        Vector2 delta = (Vector2){ BLOCKSIZE, - tan(DEG2RAD * angle_degrees) * BLOCKSIZE};

        while (isInMap(first_vertical_wall) && !isBlockFuzzy(first_vertical_wall))
            first_vertical_wall = Vector2Add(first_vertical_wall, delta);
    }

    if (0 <= angle_degrees && angle_degrees <= 180)
    {
        first_horizontal_wall.y = player.pos.y - ((int)player.pos.y % BLOCKSIZE);
        first_horizontal_wall.x = player.pos.x - tan(DEG2RAD * (angle_degrees - 90)) * ((int)player.pos.y % BLOCKSIZE);
        first_horizontal_wall = Vector2Add(first_horizontal_wall, (Vector2){0, -1});
        Vector2 delta = (Vector2){ - tan(DEG2RAD * (angle_degrees - 90)) * BLOCKSIZE, - BLOCKSIZE};

        while (isInMap(first_horizontal_wall) && !isBlockFuzzy(first_horizontal_wall))
            first_horizontal_wall = Vector2Add(first_horizontal_wall, delta);
    }
    else
    {
        first_horizontal_wall.y = player.pos.y + (BLOCKSIZE - ((int)player.pos.y % BLOCKSIZE));
        first_horizontal_wall.x = player.pos.x + tan(DEG2RAD * (angle_degrees - 90)) * (BLOCKSIZE - ((int)player.pos.y % BLOCKSIZE));
        Vector2 delta = (Vector2){ tan(DEG2RAD * (angle_degrees - 90)) * BLOCKSIZE, BLOCKSIZE};

        while (isInMap(first_horizontal_wall) && !isBlockFuzzy(first_horizontal_wall))
            first_horizontal_wall = Vector2Add(first_horizontal_wall, delta);
    }

    if (Vector2Distance(player.pos, first_horizontal_wall) < Vector2Distance(player.pos, first_vertical_wall))
        return (Vector3){first_horizontal_wall.x, first_horizontal_wall.y, 1};
    else
        return (Vector3){first_vertical_wall.x, first_vertical_wall.y, 0};
}

// Simple version, not in use
Vector2 rayEndPointSimple(Player player, float angle_offset) {
    Vector2 angle = Vector2Rotate(player.angle, DEG2RAD * angle_offset);
    Vector2 end_point = player.pos;
    while (!isBlock(end_point) && isInMap(end_point))
        end_point = Vector2Add(end_point, angle);
    return end_point;
}

void drawColumn(float block_distance, float angle, Color color) {
    int view_width = (SCREENWIDTH - MAPSIZE);
    int view_center = MAPSIZE + (view_width / 2);
    int column_offset = (RAYNUMBER / 2) * (angle / HALFVIEWANGLE);

    block_distance *= cos(DEG2RAD * angle); // correct for fish eye effect
    int column_height = SCREENHEIGHT / block_distance;
    int yOffSet = (SCREENHEIGHT - column_height) / 2;

    DrawRectangle(view_center + column_offset * RAYCASTPIXELWIDTH, 0, RAYCASTPIXELWIDTH, SCREENHEIGHT / 2, LIGHTBLUE);
    DrawRectangle(view_center + column_offset * RAYCASTPIXELWIDTH, SCREENHEIGHT / 2, RAYCASTPIXELWIDTH, SCREENHEIGHT / 2, DARKBROWN);
    DrawRectangle(view_center + column_offset * RAYCASTPIXELWIDTH, yOffSet, RAYCASTPIXELWIDTH, column_height, color);
}

bool horizontal_hit(Vector2 pixel_pos) {
    pixel_pos.y -= MAPYOFFSET;
    pixel_pos.x /= BLOCKSIZE;
    pixel_pos.y /= BLOCKSIZE;
    pixel_pos.x -= (int)pixel_pos.x;
    pixel_pos.y -= (int)pixel_pos.y;
    if ((pixel_pos.x < 0.02 || pixel_pos.x > 0.98)
    && !(pixel_pos.y < 0.02 || pixel_pos.y > 0.98))
        return true;
    return false;
}

int main() {
    srand(time(NULL));
    int frameCounter = 0;
    Player player;
    player.pos = (Vector2){MAPSIZE / 2, SCREENHEIGHT / 2};
    player.angle = (Vector2){0.0, -1.0};
    player.speed = 2;
    player.size = 8;
    Vector2 ray_end_points[RAYNUMBER];
    bool ray_hit_horizontal[RAYNUMBER];
    bool ray_hit_horizontal_normalized[RAYNUMBER];

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Raycaster");
    SetTargetFPS(60);

    while (WindowShouldClose() != true)
    {
        player = movePlayer(player);

        BeginDrawing();
        ClearBackground(BLACK);
        drawMap();
        drawPlayer(player);

        for (int i = 0; i < RAYNUMBER; i++)
        {
            float angle = - HALFVIEWANGLE + VIEWANGLE * i / RAYNUMBER;
            Vector3 result = rayEndPoint(player, angle);
            ray_end_points[i] = (Vector2){ result.x, result.y };
            ray_hit_horizontal[i] = result.z;
        }

        ray_hit_horizontal_normalized[0] = ray_hit_horizontal[0];
        ray_hit_horizontal_normalized[1] = ray_hit_horizontal[1];
        ray_hit_horizontal_normalized[RAYNUMBER-1] = ray_hit_horizontal[RAYNUMBER-1];
        ray_hit_horizontal_normalized[RAYNUMBER-2] = ray_hit_horizontal[RAYNUMBER-2];
        for (int i = 2; i < RAYNUMBER-2; i++)
        {
            int horizontal_hits = ray_hit_horizontal[i-2] + ray_hit_horizontal[i-1] + ray_hit_horizontal[i] + ray_hit_horizontal[i+1] + ray_hit_horizontal[i+2];
            if (horizontal_hits >= 3)
                ray_hit_horizontal_normalized[i] = true;
            else
                ray_hit_horizontal_normalized[i] = false;
        }

        for (int i = 0; i < RAYNUMBER; i++)
        {
            float angle = - HALFVIEWANGLE + VIEWANGLE * i / RAYNUMBER;
            float distance = Vector2Distance(player.pos, ray_end_points[i]);
            if (ray_hit_horizontal_normalized[i])
            {
                DrawLineV(Vector2Add(player.pos, Vector2Scale(Vector2Rotate(player.angle, DEG2RAD * angle), player.size)), ray_end_points[i], RED);
                drawColumn(distance / BLOCKSIZE, angle, DARKGREEN);
            }
            else
            {
                DrawLineV(Vector2Add(player.pos, Vector2Scale(Vector2Rotate(player.angle, DEG2RAD * angle), player.size)), ray_end_points[i], RED);
                drawColumn(distance / BLOCKSIZE, angle, GREEN);
            }
        }

        //for (float angle = - HALFVIEWANGLE; angle <= HALFVIEWANGLE; angle+= VIEWANGLE / RAYNUMBER)
        //{
        //    int wall_type;
        //    Vector3 hit_location = rayEndPoint(player, angle);
        //    Vector2 end_point = rayEndPointSimple(player, angle);
        //    wall_type = hit_location.z;

        //    float distance = Vector2Distance(player.pos, end_point);
        //    if (wall_type)
        //    {
        //        DrawLineV(Vector2Add(player.pos, Vector2Scale(Vector2Rotate(player.angle, DEG2RAD * angle), player.size)), end_point, GREEN);
        //        drawColumn(distance / BLOCKSIZE, angle, GREEN);
        //    }
        //    else
        //    {
        //        DrawLineV(Vector2Add(player.pos, Vector2Scale(Vector2Rotate(player.angle, DEG2RAD * angle), player.size)), end_point, DARKGREEN);
        //        drawColumn(distance / BLOCKSIZE, angle, DARKGREEN);
        //    }
        //}

        DrawFPS(10, 10);
        EndDrawing();

        frameCounter++;
    }

    CloseWindow();
    return 0;
}