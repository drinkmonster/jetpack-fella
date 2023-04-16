#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define MAX_ASTEROIDS 7

typedef struct {
    Rectangle rect;
    Vector2 speed;
    float rotation;
} Asteroid;

int main(void)
{
    // anti aliasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    // window and game vars
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "jetpack fella");
    SetTargetFPS(60);
    bool didGameStart = false;
    
    // asteroid vars
    Asteroid asteroids[MAX_ASTEROIDS] = {0};
    int numAsteroids = 0;

    // player vars
    int px = screenWidth/2;
    int py = screenHeight/2;
    double g = 0;
    bool isPlaying = true;

    // fuel vars
    bool isFuelSpawned = false;
    double fuel = 100.0;
    int fuelValue = 15;
    int score = 0;
    Rectangle playerRect = { px, py, 20, 20 }; // player
    Rectangle fuelRect = { GetRandomValue(0, screenWidth - 20), GetRandomValue(0, screenHeight - 20), 20, 20 }; // fuel
    Rectangle fuelBarRect = { 15,15,20,100 };
    Rectangle fuelBarBackground = { 10,10,30,110 };
    float radius = 10;

    // sounds
    // TODO!! it no worky rn

    // texture
    Image asteroidImg = LoadImage("image/asteroid.png");
    Texture2D asteroidTex = LoadTextureFromImage(asteroidImg);
    UnloadImage(asteroidImg);
    Rectangle asteroidTexRect = { 0, 0, asteroidTex.width, asteroidTex.height };

    Image jerrycanImg = LoadImage("image/jerrycan.png");
    ImageResize(&jerrycanImg, 25, 25);
    Texture2D jerrycanTex = LoadTextureFromImage(jerrycanImg);
    UnloadImage(jerrycanImg);

    while (!WindowShouldClose())
    {
        // controls
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                px += 4;
            } else {
                px += 2;
            }
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                px -= 4;
            } else {
                px -= 2;
            }
        }
        if (IsKeyDown(KEY_SPACE) && fuel > 0){
            g = 4;
            fuel -= 0.5;
            DrawText("*", px, py+10, 20, RED);
        }
        

        // set asteroid location, size, speed
        if (numAsteroids < MAX_ASTEROIDS && GetRandomValue(0, 100) < 10 && score >= 3) {
            asteroids[numAsteroids].rect.x = GetRandomValue(0, screenWidth - 50);
            asteroids[numAsteroids].rect.y = -20;
            asteroids[numAsteroids].rect.width = GetRandomValue(15,30);
            asteroids[numAsteroids].rect.height = asteroids[numAsteroids].rect.width;
            asteroids[numAsteroids].speed.x = 0;
            asteroids[numAsteroids].speed.y = GetRandomValue(1, 3);
            asteroids[numAsteroids].rotation = GetRandomValue(0,360);
            numAsteroids++;
        }

        /*for (int i = 0; i < numAsteroids; i++) {
            asteroids[i].rect.y += asteroids[i].speed.y;
        }*/
        int baseSpeed = asteroids[numAsteroids].speed.y;
        for (int i = 0; i < numAsteroids; i++) {
            asteroids[i].rect.y += asteroids[i].speed.y;
            // check for asteroid collision
            if (CheckCollisionRecs(playerRect, (Rectangle){asteroids[i].rect.x, asteroids[i].rect.y, asteroids[i].rect.width/2, asteroids[i].rect.height/2})){
                fuel -= asteroids[i].rect.width/3;
                asteroids[i].rect.y = 0;
                asteroids[i].rect.x = GetRandomValue(0, screenWidth - 50);
                if (asteroids[i].rect.width<25) {
                    asteroids[i].rect.width = 25;
                } else {
                    asteroids[i].rect.width = GetRandomValue(15, score*3);
                }
                asteroids[i].speed.y = 80/asteroids[i].rect.width;
                asteroids[i].rect.height = asteroids[i].rect.width;
                asteroids[i].rotation = GetRandomValue(0,360);
            }
            // check if asteroid has gone past the bottom of the screen
            if (asteroids[i].rect.y > screenHeight) {
                // respawn the asteroid at the top of the screen with a new random x coord
                asteroids[i].rect.y = 0;
                asteroids[i].rect.x = GetRandomValue(0, screenWidth - 50);
                asteroids[i].rect.width = GetRandomValue(15, score*3);
                asteroids[i].speed.y = 80/asteroids[i].rect.width;
                asteroids[i].rect.height = asteroids[i].rect.width;
                asteroids[i].rotation = GetRandomValue(0,360);
            }
        }
        

        // update player rect
        playerRect.x = px;
        playerRect.y = py;

        // player x fuel collision
        if (CheckCollisionRecs(playerRect, fuelRect)) {
            if (fuel + fuelValue <= 100) {
                fuel += fuelValue;
            } else if (fuel + fuelValue >= 100) {
                fuel = 100;
            }
            isFuelSpawned = false;
            score++;
        }
        // drawing game elements
        BeginDrawing();
            ClearBackground(BLACK);
            fuelBarRect.height = fuel;
            DrawRectangleRounded(fuelBarBackground, radius, 8, DARKGRAY);
            DrawRectangleRounded(fuelBarRect, radius, 8, ORANGE);
            // DrawRectangle(fuelRect.x, fuelRect.y, fuelRect.width, fuelRect.height, ORANGE);
            DrawTexture(jerrycanTex, fuelRect.x, fuelRect.y, ORANGE);
            DrawText("o", px, py, 20, WHITE);
            for (int i = 0; i < numAsteroids; i++) {
                // DrawRectangleRec(asteroids[i].rect, RED);
                DrawTexturePro(asteroidTex, asteroidTexRect, asteroids[i].rect, (Vector2){0, 0}, asteroids[i].rotation, WHITE);
            }
            DrawText(TextFormat("Score: %i", score), 700, 10, 20, WHITE);
            /* DrawText(TextFormat("x: %d", px), 700, 10, 20, WHITE);
            DrawText(TextFormat("y: %d", py), 700, 30, 20, WHITE);
            DrawText(TextFormat("playing: %d", isPlaying), 700, 50, 20, WHITE);
            DrawText(TextFormat("fuel: %f", fuel), 700, 70, 20, WHITE); useless debug messages */
            int endScore = score;
            // end game screen
            if (isPlaying == false) {
                DrawRectangle(0,0,screenWidth,screenHeight,BLACK);

                const char* text = "You Lost!";
                const float textWidth = MeasureText(text, 50);
                const float textX = (screenWidth - textWidth) / 2.0f;
                const float textY = (screenHeight - 150.0f) / 2.0f;

                DrawText(text, textX, textY, 50, WHITE);

                const char* scoreText = TextFormat("Your score was: %i", endScore);
                const float scoreTextWidth = MeasureText(scoreText, 30);
                const float scoreTextX = (screenWidth - scoreTextWidth) / 2.0f;
                const float scoreTextY = textY + 70.0f;

                DrawText(scoreText, scoreTextX, scoreTextY, 30, WHITE);

                const char* replayText = "Press space to replay";
                const float replayTextWidth = MeasureText(replayText, 30);
                const float replayTextX = (screenWidth - replayTextWidth) / 2.0f;
                const float replayTextY = scoreTextY + 50.0f;

                DrawText(replayText, replayTextX, replayTextY, 30, WHITE);

                if (IsKeyPressed(KEY_SPACE)) {
                    fuel = 100.0;
                    g = 0.0;
                    px = screenWidth / 2;
                    py = screenHeight / 2;
                    isPlaying = true;
                    score = 0;
                    numAsteroids = 0;
                    if (numAsteroids < MAX_ASTEROIDS && GetRandomValue(0, 100) < 10 && score >= 5) {
                        asteroids[numAsteroids].rect.x = GetRandomValue(0, screenWidth - 50);
                        asteroids[numAsteroids].rect.y = 0;
                        asteroids[numAsteroids].rect.width = GetRandomValue(10,25);
                        asteroids[numAsteroids].rect.height = asteroids[numAsteroids].rect.width;
                        asteroids[numAsteroids].speed.x = 0;
                        asteroids[numAsteroids].speed.y = GetRandomValue(1, 3);
                        asteroids[numAsteroids].rotation = GetRandomValue(0,360);
                        numAsteroids++;
                    }
                }
            }

            // main menu
            if (!didGameStart) {
                DrawRectangle(0,0,screenWidth,screenHeight,BLACK);
                const char* menuText = "jetpack fella";
                const float menuTextWidth = MeasureText(menuText, 50);
                const float menuTextX = (screenWidth - menuTextWidth) / 2.0f;
                const float menuTextY = (screenHeight - 150.0f) / 2.0f;

                DrawText(menuText, menuTextX, menuTextY, 50, WHITE);

                const char* spaceText = "space to play";
                const float spaceTextWidth = MeasureText(spaceText, 30);
                const float spaceTextX = (screenWidth - spaceTextWidth) / 2.0f;
                const float spaceTextY = menuTextY + 75.0f;

                DrawText(spaceText, spaceTextX, spaceTextY, 30, WHITE);

                if (IsKeyPressed(KEY_SPACE)) didGameStart = true;
            }

        EndDrawing();

        // gravitation
        py -= g;
        g -= 0.17;

        // wrapping player position on sides
        if (px > screenWidth) {
            px = 0;
        }
        else if (px < 0) {
            px = screenWidth;
        }
        // not letting player escape on top
        if (py <= 0) {
            py = 0;
        }
        // lose if touching the bottom
        if (py > screenHeight) {
            isPlaying = false;
        }
        // change fuel pos
        if (!isFuelSpawned) {
            fuelRect.x = GetRandomValue(50, screenWidth - 20);
            fuelRect.y = GetRandomValue(20, screenHeight - 50);
            isFuelSpawned = true;
        }
    }
    CloseWindow();

    return 0;
}