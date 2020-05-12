#include "raylib.h"

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)

// Change this depending on the path of your executable relative to the assets folder
#define ASSET_PATH "../assets/"

typedef enum EnemyFlags
{
    ENEMY_PAST_US = 0x01,
    ENEMY_ACTIVE  = 0x02,
    ENEMY_PAST_THE_MIDDLE = 0x04,
} EnemyFlags;

typedef struct Enemy
{
    EnemyFlags flags;

    union
    {
        Rectangle collider;
        struct
        {
            float x;
            float y;
            float w;
            float h;
        };
    };
} Enemy;

#define ENEMIES_COUNT (6)
Enemy enemies[ENEMIES_COUNT];

#define GROUND_HEIGHT (30)

int y_on_ground(int height)
{
    return SCREEN_HEIGHT - height - GROUND_HEIGHT;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Window title");
    SetTargetFPS(60);

    const Rectangle standup_dino = {
        .x = 30,
        .width = 60,
        .height = 110,
        .y = y_on_ground(110)
    };

    const Rectangle crouched_dino = {
        .x = 30,
        .width = 100,
        .height = 60,
        .y = y_on_ground(60)
    };

    Rectangle dino = standup_dino;

    const float gravity = 2;
    float dino_velocity = gravity;
    bool game_over = false;
    int score = 0;

    const Rectangle tall_enemy_collider = {
        .x = SCREEN_WIDTH,
        .width = standup_dino.width - 10,
        .height = standup_dino.height - 30,
        .y = y_on_ground(standup_dino.height - 30),
    };

    const Rectangle bird_enemy_collider = {
        .x = SCREEN_WIDTH,
        .width = dino.width + 10,
        .height = 20,
        .y = y_on_ground(standup_dino.height)
    };

    for (int i = 0; i < ENEMIES_COUNT; i++)
    {
        enemies[i].collider = tall_enemy_collider;
    }

    enemies[0].flags |= ENEMY_ACTIVE;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        const bool dino_on_ground = dino.y == y_on_ground(dino.height);
        const bool dino_is_up = dino.height == standup_dino.height;
        const bool dino_is_crouched = dino.height == crouched_dino.height;

        if (IsKeyPressed(KEY_SPACE) && dino_on_ground && dino_is_up)
        {
            dino_velocity = -5.f;
        }

        if (IsKeyPressed(KEY_S) && dino_on_ground && dino_is_up)
        {
            dino.y = dino.y + standup_dino.height - crouched_dino.height;
            dino.width = crouched_dino.width;
            dino.height = crouched_dino.height;
        }
        else if (IsKeyReleased(KEY_S) && dino_on_ground && dino_is_crouched)
        {
            dino.y = dino.y - (standup_dino.height - crouched_dino.height);
            dino.width = standup_dino.width;
            dino.height = standup_dino.height;
        }

        if (dino_velocity < gravity)
        {
            dino_velocity += 10 * GetFrameTime();
        }

        dino.y += 2 * dino_velocity;
        if (dino.y >= y_on_ground(dino.height))
        {
            dino.y = y_on_ground(dino.height);
        }

        for (int i = 0; i < ENEMIES_COUNT; i++)
        {
            if (enemies[i].flags & ENEMY_ACTIVE)
            {
                if (enemies[i].x + enemies[i].w < dino.x && !(enemies[i].flags & ENEMY_PAST_US))
                {
                    enemies[i].flags |= ENEMY_PAST_US;
                    score++;
                }

                game_over = game_over || CheckCollisionRecs(dino, enemies[i].collider);

                enemies[i].x -= 200 * GetFrameTime();
                if (enemies[i].x + enemies[i].w < 0) enemies[i].flags &= ~ENEMY_ACTIVE;

                if (enemies[i].x - enemies[i].w < SCREEN_WIDTH / 3 && !(enemies[i].flags & ENEMY_PAST_THE_MIDDLE))
                {
                    enemies[i].flags |= ENEMY_PAST_THE_MIDDLE;

                    for (int j = 0; j < ENEMIES_COUNT; j++)
                    {
                        if (j != i && !(enemies[j].flags & ENEMY_ACTIVE))
                        {
                            enemies[j] = (Enemy) {0};
                            enemies[j].flags |= ENEMY_ACTIVE;
                            enemies[j].collider = bird_enemy_collider;
                            break;
                        }
                    }
                }

                DrawRectangleRec(enemies[i].collider, GRAY);
            }
        }

        DrawRectangle(0, SCREEN_HEIGHT - GROUND_HEIGHT, SCREEN_WIDTH, GROUND_HEIGHT, BROWN);

        DrawRectangleRec(dino, BLACK);

        if (game_over) DrawText("Game Over", 30, 30, 60, BLACK);
        else DrawText(FormatText("Score: %d", score), 30, 30, 60, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}