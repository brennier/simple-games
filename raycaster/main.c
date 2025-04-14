#include <raylib.h>
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M_PI 3.14159265358979323846  /* pi */
#define SCREENWIDTH 1800
#define SCREENHEIGHT 720
#define MAPSIZE 720
#define BLOCKSIZE 30
#define RAYCASTPIXELWIDTH 4
#define VIEWANGLE (70.0 * DEG2RAD)
#define TEXTURESIZE 128
#define MAX_ANIMATION_FRAMES 5

#define LIGHTBLUE (Color){ 135, 206, 235, 255 }

#define RAYNUMBER ( (SCREENWIDTH - MAPSIZE) / RAYCASTPIXELWIDTH )
#define MAPYOFFSET ( (SCREENHEIGHT - MAPSIZE) / 2 )
#define HALFVIEWANGLE (VIEWANGLE / 2)

Texture2D wall_textures, weapon_textures, enemy_textures, item_textures;

typedef enum WallType
{
    NO_WALL = 0,
    DOOR = 1,
    RED_BRICK = 2,
    GRAY_BRICK = 3,
} WallType;

typedef enum Orientation
{
    HORIZONTAL,
    VERTICAL
} Orientation;

typedef enum weapon_type
{
    WEAPON_KNIFE,
    WEAPON_GUN,
    WEAPON_RIFLE,
    WEAPON_MACHINE_GUN,
} weapon_type;

typedef struct Sprite
{
    int size;
    int speed;
    Vector2 pos;
    Texture2D texture;
    int animation_frame;
    Vector2 angle;
} Sprite;

typedef struct Wall
{
    WallType type;
    Orientation orientation;
    Vector2 texture_coordinates;
} Wall;

typedef struct WallPoint
{
    Vector2 pos;
    WallType wall_type;
    Orientation wall_orientation;
    int texture_column;
} WallPoint;

//typedef struct SpritePoint
//{
//} SpritePoint;

//typedef union RayEndPoint
//{
//    WallPoint;
//    SpritePoint;
//} RayEndPoint;

WallType map[MAPSIZE / BLOCKSIZE][MAPSIZE / BLOCKSIZE] = {
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 3},
    {2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 3},
    {2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 3},
    {2, 0, 0, 2, 2, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 3},
    {2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 3},
    {2, 0, 0, 0, 2, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 3},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 1, 2, 2, 2, 3},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 2, 2, 2},
    {2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 2, 2, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2},
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
};

typedef struct Player {
    Vector2 pos;
    Vector2 angle;
    int speed;
    int size;
    weapon_type weapon;
} Player;

Vector2 get_wall_texture_coordinates(const WallType wall_type)
{
    Vector2 coordinates;
    switch (wall_type)
    {
        case NO_WALL: coordinates = (Vector2){0, 0}; break;
        case RED_BRICK: coordinates = (Vector2){4, 1}; break;
        case GRAY_BRICK: coordinates = (Vector2){0, 5}; break;
        case DOOR: coordinates = (Vector2){1, 5}; break;
        default: coordinates = (Vector2){0, 0}; break;
    }
    return Vector2Scale(coordinates, TEXTURESIZE);
}

void draw_map() {
    DrawRectangle(0, MAPYOFFSET, MAPSIZE, MAPSIZE, BLACK);
    Color wall_color;
    for (int i = 0; i < MAPSIZE/BLOCKSIZE; i++)
        for (int j = 0; j < MAPSIZE/BLOCKSIZE; j++)
        {
            switch (map[i][j])
            {
                case NO_WALL:    continue; break;
                case RED_BRICK:  wall_color = GRAY;  break;
                case GRAY_BRICK: wall_color = GRAY;  break;
                case DOOR:       wall_color = BROWN; break;
                default: wall_color = WHITE; break;
            }
            DrawRectangle(
                BLOCKSIZE * j,
                MAPYOFFSET + BLOCKSIZE * i,
                BLOCKSIZE - 1,
                BLOCKSIZE - 1,
                wall_color
            );
        }
}

void draw_player(const Player player) {
    DrawCircleV(player.pos, player.size, YELLOW);
}

bool isInMap(const Vector2 pixel_pos) {
    return 0 < pixel_pos.x && pixel_pos.x < MAPSIZE
        && MAPYOFFSET < pixel_pos.y
        && pixel_pos.y < MAPYOFFSET + MAPSIZE;
}

bool is_block(const Vector2 pixel_pos) {
    if (!isInMap(pixel_pos))
        return false;
    int block_column = pixel_pos.x / BLOCKSIZE;
    int block_row = (pixel_pos.y - MAPYOFFSET) / BLOCKSIZE;
    return map[block_row][block_column] > 0;
}

bool isInBlock(const Player player) {
    Vector2 front_pixel = Vector2Add(player.pos, Vector2Scale(player.angle, 3 * player.size));
    return is_block(front_pixel);
}

bool isInWindow(const Vector2 pixel_pos) {
    return 0 < pixel_pos.x && pixel_pos.x < SCREENWIDTH
        && 0 < pixel_pos.y && pixel_pos.y < SCREENHEIGHT;
}

bool playerInBlock(Vector2 next_pos, int size) {
    Vector2 angle = Vector2Scale((Vector2){1, 0}, size);
    for (int i = 0; i < 360; i += 45)
        if (is_block(Vector2Add(next_pos, Vector2Rotate(angle, DEG2RAD * i))))
            return true;
    return false;
}

bool open_door(const Player player)
{
    Vector2 pos_in_front = Vector2Add(player.pos, Vector2Scale(player.angle, 2*player.size));
    int block_column = pos_in_front.x / BLOCKSIZE;
    int block_row = (pos_in_front.y - MAPYOFFSET) / BLOCKSIZE;
    if (map[block_row][block_column] == DOOR)
    {
        map[block_row][block_column] = NO_WALL;
        return true;
    }
    return false;
}

WallType get_wall_type(const Vector2 pixel_position)
{
    int block_column = pixel_position.x / BLOCKSIZE;
    int block_row = (pixel_position.y - MAPYOFFSET) / BLOCKSIZE;
    return map[block_row][block_column];
}

bool find_intersection(const Vector2 start, const Vector2 end, Vector2* intersection_point) {
    Vector2 angle = Vector2Normalize(Vector2Subtract(end, start));
    Vector2 first_vertical_wall = start;
    Vector2 first_horizontal_wall = start;

    // The amount to add if x increases by one BLOCKSIZE
    Vector2 y_step = (Vector2){ BLOCKSIZE, BLOCKSIZE * (angle.y / angle.x) };
    // The amount to increase if y increases by one BLOCKSIZE
    Vector2 x_step = (Vector2){ BLOCKSIZE * (angle.x / angle.y), BLOCKSIZE };

    // Set the previous and next block-aligned coordinates
    int prev_aligned_x = floor(start.x / BLOCKSIZE) * BLOCKSIZE;
    int prev_aligned_y = floor(start.y / BLOCKSIZE) * BLOCKSIZE;
    int next_aligned_x = BLOCKSIZE + prev_aligned_x;
    int next_aligned_y = BLOCKSIZE + prev_aligned_y;

    // Distance to the next alignment
    float x_dist = 0.0f;
    float y_dist = 0.0f;

    // Find the first vertical wall
    if (angle.x > 0)
        x_dist = next_aligned_x - start.x;
    else
    {
        y_step = Vector2Scale(y_step, -1.0f);
        x_dist = start.x - prev_aligned_x;
        x_dist += 0.001;
    }

    if (angle.y > 0)
        y_dist = next_aligned_y - start.y;
    else
    {
        x_step = Vector2Scale(x_step, -1.0f);
        y_dist = start.y - prev_aligned_y;
        y_dist += 0.001;
    }

    first_vertical_wall = Vector2Add(start, Vector2Scale(y_step, x_dist / BLOCKSIZE));
    first_horizontal_wall = Vector2Add(start, Vector2Scale(x_step, y_dist / BLOCKSIZE));

    while (isInMap(first_vertical_wall) && !is_block(first_vertical_wall))
        first_vertical_wall = Vector2Add(first_vertical_wall, y_step);
    while (isInMap(first_horizontal_wall) && !is_block(first_horizontal_wall))
        first_horizontal_wall = Vector2Add(first_horizontal_wall, x_step);
    
    if (!isInMap(first_horizontal_wall) && !isInMap(first_vertical_wall))
        return false;

    if (Vector2DistanceSqr(start, first_horizontal_wall) < Vector2DistanceSqr(start, first_vertical_wall))
        *intersection_point = first_horizontal_wall;
    else
        *intersection_point = first_vertical_wall;
    
    return true;
}

// This will return a Vector4 of the form {end_point.x, end_point.y, wall_type, texture_column_position}
WallPoint rayEndPoint(const Player player, const float angle_offset) {
    Vector2 angle = Vector2Rotate(player.angle, angle_offset);
    Vector2 first_vertical_wall = player.pos;
    Vector2 first_horizontal_wall = player.pos;

    // The amount to add if x increases by one BLOCKSIZE
    Vector2 y_step = (Vector2){ BLOCKSIZE, BLOCKSIZE * (angle.y / angle.x) };
    // The amount to increase if y increases by one BLOCKSIZE
    Vector2 x_step = (Vector2){ BLOCKSIZE * (angle.x / angle.y), BLOCKSIZE };

    // Set the previous and next block-aligned coordinates
    int prev_aligned_x = floor(player.pos.x / BLOCKSIZE) * BLOCKSIZE;
    int prev_aligned_y = floor(player.pos.y / BLOCKSIZE) * BLOCKSIZE;
    int next_aligned_x = BLOCKSIZE + prev_aligned_x;
    int next_aligned_y = BLOCKSIZE + prev_aligned_y;

    // Distance to the next alignment
    float x_dist = 0.0f;
    float y_dist = 0.0f;

    // Find the first vertical wall
    if (angle.x > 0)
        x_dist = next_aligned_x - player.pos.x;
    else
    {
        y_step = Vector2Scale(y_step, -1.0f);
        x_dist = player.pos.x - prev_aligned_x;
        x_dist += 0.001;
    }

    if (angle.y > 0)
        y_dist = next_aligned_y - player.pos.y;
    else
    {
        x_step = Vector2Scale(x_step, -1.0f);
        y_dist = player.pos.y - prev_aligned_y;
        y_dist += 0.001;
    }

    first_vertical_wall = Vector2Add(player.pos, Vector2Scale(y_step, x_dist / BLOCKSIZE));
    first_horizontal_wall = Vector2Add(player.pos, Vector2Scale(x_step, y_dist / BLOCKSIZE));

    while (isInMap(first_vertical_wall) && !is_block(first_vertical_wall))
        first_vertical_wall = Vector2Add(first_vertical_wall, y_step);
    while (isInMap(first_horizontal_wall) && !is_block(first_horizontal_wall))
        first_horizontal_wall = Vector2Add(first_horizontal_wall, x_step);

    float texture_column_vertical = ((int)first_vertical_wall.y % BLOCKSIZE);
    texture_column_vertical *= (float)TEXTURESIZE / BLOCKSIZE;
    float texture_column_horizontal = ((int)first_horizontal_wall.x % BLOCKSIZE);
    texture_column_horizontal *= (float)TEXTURESIZE / BLOCKSIZE;

    WallPoint wall_point;

    if (Vector2DistanceSqr(player.pos, first_horizontal_wall) < Vector2DistanceSqr(player.pos, first_vertical_wall))
    {
        wall_point.pos = first_horizontal_wall;
        wall_point.wall_orientation = HORIZONTAL;
        wall_point.texture_column = texture_column_horizontal;
        wall_point.wall_type = get_wall_type(first_horizontal_wall);
    }
    else
    {
        wall_point.pos = first_vertical_wall;
        wall_point.wall_orientation = VERTICAL;
        wall_point.texture_column = texture_column_vertical;
        wall_point.wall_type = get_wall_type(first_vertical_wall);
    }

    return wall_point;
}

Player movePlayer(Player player, const float time_delta) {
    Vector2 pos_delta = (Vector2){ 0.0, 0.0 };
    float speed = player.speed * time_delta * 60.0f;

    if (IsKeyDown(KEY_RIGHT))
        player.angle = Vector2Rotate(player.angle, DEG2RAD * speed);
    else if (IsKeyDown(KEY_LEFT))
        player.angle = Vector2Rotate(player.angle, - DEG2RAD * speed);
    
    float angle = 0.0;
    if (IsKeyDown(KEY_UP))
    {
        pos_delta = Vector2Scale(player.angle, speed);
        angle = 0.0 * DEG2RAD;
    }
    else if (IsKeyDown(KEY_DOWN))
    {
        pos_delta = Vector2Scale(player.angle, -1 * speed);
        angle = 180.0 * DEG2RAD;
    }

    if (playerInBlock(Vector2Add(player.pos, pos_delta), player.size))
    {
        switch (rayEndPoint(player, angle).wall_orientation)
        {
            case HORIZONTAL: pos_delta.y *= 0; break;
            case VERTICAL:   pos_delta.x *= 0; break;
        }
    }

    // If I'm still in a wall even after pushing off the wall in front,
    // then I must be in a corner, so don't move
    if (playerInBlock(Vector2Add(player.pos, pos_delta), player.size))
        return player;

    player.pos = Vector2Add(player.pos, pos_delta);

    return player;
}

// Simple version, not in use
Vector2 rayEndPointSimple(Player player, float angle_offset) {
    Vector2 angle = Vector2Rotate(player.angle, angle_offset);
    Vector2 end_point = player.pos;
    while (!is_block(end_point) && isInMap(end_point))
        end_point = Vector2Add(end_point, angle);
    return end_point;
}

void drawColumn(float block_distance, int view_column, float brightness, int texture_column, WallType wall_type) {
    int view_width = (SCREENWIDTH - MAPSIZE);
    int view_center = MAPSIZE + (view_width / 2);
    int column_offset = view_column - (RAYNUMBER / 2);

    int column_height = SCREENHEIGHT / block_distance;
    int yOffSet = (SCREENHEIGHT - column_height) / 2;

    Vector2 wall_texture_coordinates = get_wall_texture_coordinates(wall_type);

    DrawTexturePro
    (
        wall_textures,
        (Rectangle){wall_texture_coordinates.x + texture_column, wall_texture_coordinates.y, 1, TEXTURESIZE},
        (Rectangle){view_center + column_offset * RAYCASTPIXELWIDTH, yOffSet, RAYCASTPIXELWIDTH, column_height},
        (Vector2){0.0, 0.0},
        0.0,
        ColorBrightness(WHITE, brightness)
    );
}

void draw_skybox()
{
    DrawRectangleGradientV(MAPSIZE, 0, SCREENWIDTH - MAPSIZE, SCREENHEIGHT / 2, BLUE, LIGHTBLUE);
    DrawRectangleGradientV(MAPSIZE, SCREENHEIGHT / 2, SCREENWIDTH - MAPSIZE, SCREENHEIGHT / 2, (Color){76, 35, 26, 255}, BROWN);
}

Texture2D load_texture(char filename[])
{
    Image image = LoadImage(filename);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

void load_all_textures()
{
    wall_textures   = load_texture("textures/walls.png");
    weapon_textures = load_texture("textures/weapons.png");
    enemy_textures  = load_texture("textures/enemies.png");
    item_textures   = load_texture("textures/items.png");
}

void draw_weapon(Texture2D weapon, weapon_type weapon_type, int frame) {
    int fourth_width = (SCREENWIDTH - MAPSIZE) / 4;
    DrawTexturePro
    (
        weapon,
        (Rectangle){frame * (64 + 1), weapon_type * (64 + 1), 64, 64},
        (Rectangle){MAPSIZE + fourth_width/2, 0, 3*fourth_width, SCREENHEIGHT},
        (Vector2){0.0, 0.0},
        0.0,
        WHITE
    );
}

void draw_sprite(Player player, Sprite sprite, const float ray_distance[]) {
    DrawCircleV(sprite.pos, sprite.size, RED);
    Vector2 direction_to_enemy = Vector2Subtract(sprite.pos, player.pos);
    float angle = Vector2Angle(player.angle, direction_to_enemy);
    int center_view_column = Remap(angle, -HALFVIEWANGLE, HALFVIEWANGLE, 0, RAYNUMBER);
    float distance = Vector2Distance(player.pos, sprite.pos);
    float block_distance = distance / BLOCKSIZE;
    float sprite_size = SCREENHEIGHT / block_distance;
    int first_view_column = center_view_column - sprite_size / 8;
    
    for (int view_column = first_view_column; view_column < first_view_column + sprite_size / 4; view_column++)
    {
        if (view_column < 0 || view_column > RAYNUMBER)
            continue;
        int sprite_column = Remap(view_column, first_view_column, first_view_column + sprite_size / 4, 0, TEXTURESIZE);
        if (distance < ray_distance[view_column])
            DrawTexturePro
            (
                sprite.texture,
                (Rectangle){sprite_column, 0, 1, TEXTURESIZE},
                (Rectangle){MAPSIZE + view_column * RAYCASTPIXELWIDTH, SCREENHEIGHT / 2 - sprite_size / 2.1, RAYCASTPIXELWIDTH, sprite_size},
                (Vector2){ 0.0, 0.0 },
                0.0,
                WHITE
            );
    }
}

Sprite move_enemy(Player player, Sprite enemy, float time_delta) {
    Vector2 next_pos = enemy.pos;
    float speed = enemy.speed * time_delta * 60.0f;

    if (Vector2Distance(player.pos, enemy.pos) > 5 * BLOCKSIZE)
        return enemy;
    float random_float = (float)rand()/(float)(RAND_MAX);
    random_float -= random_float / 2.0;
    enemy.angle = Vector2Normalize(Vector2Subtract(player.pos, enemy.pos));
    if (Vector2Angle(player.pos, enemy.pos) > Vector2Angle(enemy.angle, (Vector2){ 1.0, 0.0 }))
        enemy.angle = Vector2Rotate(enemy.angle, - DEG2RAD * speed);
    else
        enemy.angle = Vector2Rotate(enemy.angle, DEG2RAD * speed);

    next_pos = Vector2Add(enemy.pos, Vector2Scale(enemy.angle, speed));

    if (playerInBlock(next_pos, enemy.size))
        enemy.pos = enemy.pos;
    else
        enemy.pos = next_pos;
    return enemy;
}

int main() {
    srand((unsigned int)time(NULL));
    Player player = {
        .pos = (Vector2){MAPSIZE / 2, SCREENHEIGHT / 2},
        .angle = (Vector2){0.0, -1.0},
        .speed = 2,
        .size = 7,
        .weapon = WEAPON_RIFLE,
    };
    float time_delta;
    float action_frame = 0.0f;
    float input_delay = 0.0f;
    float ray_distance[RAYNUMBER]; 

    // Cache calls to cosine for fish eye correcting each ray
    static float cosine_cache[RAYNUMBER];
    for (int ray_index = 0; ray_index < RAYNUMBER; ray_index++)
    {
        float angle = - HALFVIEWANGLE + VIEWANGLE * ray_index / RAYNUMBER;
        cosine_cache[ray_index] = cos(angle);
    }

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Raycaster");
    load_all_textures();

    Sprite enemy = {
        .pos = (Vector2){3 * MAPSIZE / 4, MAPSIZE / 4},
        .size = 5,
        .speed = 1,
        .angle = (Vector2){ 1.0, 0.0 },
        .texture = enemy_textures
    };

    Sprite key = {
        .pos = (Vector2){3 * MAPSIZE / 4, 2 * MAPSIZE / 3},
        .size = 5,
        .speed = 0,
        .angle = (Vector2){ 0.0, 0.0 },
        .texture = item_textures
    };

    while (WindowShouldClose() != true)
    {
        time_delta = GetFrameTime();
        player = movePlayer(player, time_delta);
        enemy = move_enemy(player, enemy, time_delta);
        if (input_delay == 0.0f && IsKeyDown(KEY_SPACE))
        {
            if (open_door(player))
                input_delay = 2.0f;
            else
            {
                input_delay = 4.0f;
                action_frame = 1.0;
            }
        }

        BeginDrawing();
        draw_map();
        draw_player(player);
        draw_skybox();
        for (int view_column = 0; view_column < RAYNUMBER; view_column++)
        {
            float angle = - HALFVIEWANGLE + VIEWANGLE * view_column / RAYNUMBER;
            WallPoint end_point = rayEndPoint(player, angle);
    
            ray_distance[view_column] = Vector2Distance(player.pos, end_point.pos);
            ray_distance[view_column] *= cosine_cache[view_column]; // correct for fish eye effect
            float block_distance = (ray_distance[view_column] / BLOCKSIZE);
            float brightness = 1.0f / (block_distance * block_distance);
            brightness = Clamp(brightness, 0.0, 1.0);
            brightness = Remap(brightness, 0.0, 1.0, -0.4, -0.1);
            //float brightness = -0.3;

            if (view_column % 8 == 0)
                DrawLineV(Vector2Add(player.pos, Vector2Scale(Vector2Rotate(player.angle, angle), player.size)), end_point.pos, GREEN);

            if (end_point.wall_orientation == HORIZONTAL)
                drawColumn(block_distance, view_column, brightness + 0.3, end_point.texture_column, end_point.wall_type);
            else
                drawColumn(block_distance, view_column, brightness, end_point.texture_column, end_point.wall_type);
        }

        draw_sprite(player, enemy, ray_distance);
        draw_sprite(player, key, ray_distance);
        draw_weapon(weapon_textures, player.weapon, (int)action_frame);
    
        DrawRectangle(SCREENWIDTH - 105, 10, 90, 20, WHITE);
        DrawFPS(SCREENWIDTH - 100, 10);
        EndDrawing();

        if (0.0f < action_frame && action_frame < MAX_ANIMATION_FRAMES)
            action_frame += time_delta * 15.0f;

        if (action_frame > MAX_ANIMATION_FRAMES)
            action_frame = 0.0f;

        if (input_delay > 0)
            input_delay -= time_delta * 15.0f;
        else if (input_delay < 0 && !IsKeyDown(KEY_SPACE))
            input_delay = 0.0f;
    }

    CloseWindow();
    return 0;
}