#include <raylib.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>

// based on the raylib missle command sample file

#define BASE_COLOR             (Color){ 0, 0, 0, 0 }

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Pong {
    Vector2 position;
    Vector2 delta;

} Pong;

typedef struct Paddle {
    Vector2 position;
    Vector2 delta;

} Paddle;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static int screenWidth = 1200;
static int screenHeight = 720;

static bool gameOver = false;
static bool pause = false;

static unsigned int player_score = 0;
static unsigned int ai_score = 0;

static Paddle paddles[2] = { 0 };
static Pong pong = { 0 };

static const unsigned int PaddleSize = 250;
static const unsigned int CircleSize = 25;
static const int PaddleSpeed = 1;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UpdateDrawFrame(void);  // Update and Draw (one frame)

// Additional module functions
static void UpdatePaddlePlayer();
static void UpdatePaddleAI();
static void ResetPaddles();

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "PONG!");

    InitGame();

    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//--------------------------------------------------------------------------------------
// Game Module Functions Definition
//--------------------------------------------------------------------------------------

static void ResetPaddles()
{
    // Initialize Paddles
    paddles[0].position = (Vector2){ 50, (screenHeight / 2) - PaddleSize / 2 };
    paddles[0].delta = (Vector2){ 0, 0 };

    paddles[1].position = (Vector2){ screenWidth - 100, (screenHeight / 2) - PaddleSize / 2};
    paddles[1].delta = (Vector2){ 0, 0 };

    // Initialize Ball
    pong.position = (Vector2){ screenWidth / 2, screenHeight / 2 };
    pong.delta = (Vector2){ GetRandomValue(-8, 8), GetRandomValue(-8, 8) };
}

// Initialize game variables
void InitGame(void)
{
    ResetPaddles();

    player_score = 0;
    ai_score = 0;
}

// Update game (one frame)
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            // Pong update

            pong.position.x += pong.delta.x;
            pong.position.y += pong.delta.y;

            // keep it in bounds :)
            if (CheckCollisionCircleLine(pong.position, CircleSize, (Vector2) {0,0}, (Vector2) {screenWidth, 0}))
            {
                pong.delta.y = -pong.delta.y;
            }
            if (CheckCollisionCircleLine(pong.position, CircleSize, (Vector2) {0,screenHeight}, (Vector2) {screenWidth, screenHeight}))
            {
                pong.delta.y = -pong.delta.y;
            }

            if (CheckCollisionCircleLine(pong.position, CircleSize, (Vector2) {0,0}, (Vector2) {0, screenHeight}))
            {
                // Player Goal
                ++ai_score;

                ResetPaddles();
            }
            if (CheckCollisionCircleLine(pong.position, CircleSize, (Vector2) {screenWidth,0}, (Vector2) {screenWidth, screenHeight}))
            {
                // AI Goal
                ++player_score;

                ResetPaddles();
            }

            // Paddles
            if (CheckCollisionCircleRec(pong.position, CircleSize, (Rectangle) {paddles[0].position.x, paddles[0].position.y, PaddleSize / 4, PaddleSize}))
            {
                pong.delta.x = -pong.delta.x;
            }

            if (CheckCollisionCircleRec(pong.position, CircleSize, (Rectangle) {paddles[1].position.x, paddles[1].position.y, PaddleSize / 4, PaddleSize}))
            {
                pong.delta.x = -pong.delta.x;
            }

            // Game Logic
            UpdatePaddlePlayer();
            UpdatePaddleAI();
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame(void)
{
    BeginDrawing();

        ClearBackground(BLACK);

        if (!gameOver)
        {
            for (int i = 0; i < 2; i++)
            {
                DrawRectangle(paddles[i].position.x, paddles[i].position.y, PaddleSize / 4, PaddleSize, RAYWHITE);
            }

            DrawCircle(pong.position.x, pong.position.y, CircleSize, RAYWHITE);

            const int number_screen_buffer = 25;
            const int font_size = 28;

            char num [3];
            sprintf(num, "%u", player_score);
            DrawText(num, number_screen_buffer, number_screen_buffer, font_size, GRAY);

            sprintf(num, "%u", ai_score);
            DrawText(num, screenWidth - number_screen_buffer, number_screen_buffer, font_size, GRAY);

            if (pause) 
            {
                DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
            }
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);

    EndDrawing();
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}

//--------------------------------------------------------------------------------------
// Additional module functions
//--------------------------------------------------------------------------------------
static void UpdatePaddlePlayer()
{
    if (IsKeyDown(KEY_UP))
    {
        paddles[0].position.y -= PaddleSpeed;
    }
    
    if (IsKeyDown(KEY_DOWN))
    {
        paddles[0].position.y += PaddleSpeed;
    }
}

static void UpdatePaddleAI()
{
    const int centre_x = paddles[1].position.x + (PaddleSize/8);
    const int centre_y = paddles[1].position.y + (PaddleSize/2);

    if (pong.position.y > centre_y)
    {
        paddles[1].delta.y = PaddleSpeed;
    }
    if (pong.position.y < centre_y)
    {
        paddles[1].delta.y = -PaddleSpeed;
    }

    paddles[1].position.y += paddles[1].delta.y;

    // static int missileIndex = 0;

    // // Launch missile
    // if (framesCounter%MISSILE_LAUNCH_FRAMES == 0)
    // {
    //     float module;
    //     float sideX;
    //     float sideY;

    //     // Activate the missile
    //     missile[missileIndex].active = true;

    //     // Assign start position
    //     missile[missileIndex].origin = (Vector2){ GetRandomValue(20, screenWidth - 20), -10 };
    //     missile[missileIndex].position = missile[missileIndex].origin;
    //     missile[missileIndex].objective = (Vector2){ GetRandomValue(20, screenWidth - 20), screenHeight + 10 };

    //     // Calculate speed
    //     module = sqrt( pow(missile[missileIndex].objective.x - missile[missileIndex].origin.x, 2) +
    //                    pow(missile[missileIndex].objective.y - missile[missileIndex].origin.y, 2));

    //     sideX = (missile[missileIndex].objective.x - missile[missileIndex].origin.x)*MISSILE_SPEED/module;
    //     sideY = (missile[missileIndex].objective.y - missile[missileIndex].origin.y)*MISSILE_SPEED/module;

    //     missile[missileIndex].speed = (Vector2){ sideX, sideY };

    //     // Update
    //     missileIndex++;
    //     if (missileIndex == MAX_MISSILES) missileIndex = 0;
    // }
}