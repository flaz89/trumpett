#include <iostream>
#include <raylib.h>

struct AnimData {
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningtime;
};

int main () {

    const int windowDimension[2] {512, 380};

    InitWindow(windowDimension[0], windowDimension[1], "Trumpett");

    // set gravity (pixel/second)/second
    const int gravity {1000};
    // set background intro
    Texture2D bacgroundIntro = LoadTexture("textures/Background_flyer.png");
    AnimData bacgroundIntroData {
        {0.0, 0.0, static_cast<float>(bacgroundIntro.width), static_cast<float>(bacgroundIntro.height)}, //rect
        {0.0, 0.0}, //position
        0,
        1.0,
        0.0
    };
    bool intro {true};

    // set trumpet (raylib)
    Texture2D trumpett = LoadTexture("textures/trumpett_2.png");
    AnimData trumpettData {
        {0.0, 0.0, static_cast<float>(trumpett.width) / 8, static_cast<float>(trumpett.height)}, // rect
        {windowDimension[0] / 2 - static_cast<float>(trumpett.width) / 7 / 2, windowDimension[1] - static_cast<float>(trumpett.height)}, // position
        0, // frame
        1.0 / 6.0, // update time
        0.0 // running time
    };
    const int jumpVelocity {-600}; // pixel/second


    // set obstacle (raylib)
    Texture2D obstacle = LoadTexture("textures/obstacles.png");
    AnimData obstacleData {
        {-256.0, 0.0, static_cast<float>(obstacle.width) / 8, static_cast<float>(obstacle.height) / 2}, // rect
        {static_cast<float>(windowDimension[0]), static_cast<float>(windowDimension[1]) - obstacle.height / 2}, // position
        0, // frame
        1.0 / 12.0, // update time
        0.0 // running time
    };
    // obstacle velocity on X axis (pixels/second)
    int obstacleVelocity {-400};
    bool obstacleToGain {};

    // set background image
    Texture2D background = LoadTexture("textures/Meteo_background_final.png");
    float backgroundX {0.0};
    Texture2D forground = LoadTexture("textures/foreground2.png");
    float foregroundX {0.0};
    Texture2D backgroundGameOver = LoadTexture("textures/gameOver_background.png");



    // set soundtrack
    InitAudioDevice();
    Music soundtrack = LoadMusicStream("audio/the-return-of-the-8-bit-era-301292.mp3");
    soundtrack.looping = true;
    PlayMusicStream(soundtrack);
    SetMusicVolume(soundtrack, 0.4f);

    // set audio feedback
    Sound gainMoney = LoadSound("audio/arcade-ui-14-229514.mp3");
    Sound gameOverSound = LoadSound("audio/mixkit-sad-game-over-trombone-471.wav");
    
    
    
    bool isStarted {false};
    bool playerInPosition {false};
    bool isJumping {false};
    bool gameOver {false};
    float cashCount {0.0};
 
   
    SetTargetFPS(60);

    int velocity {0};
    
    // main game loop (tick) --------------------------------------------

    while(!WindowShouldClose()) {

        // delta time
        const float dt {GetFrameTime()};
        UpdateMusicStream(soundtrack);


        // draw intro
        if (intro) {
            DrawTextureRec(bacgroundIntro, bacgroundIntroData.rec, bacgroundIntroData.pos, WHITE);
            DrawText("Press S to skip", (windowDimension[0] / 2) - 55, windowDimension[1] - 25, 15, WHITE);
            if (IsKeyDown(KEY_S)) {
                bacgroundIntroData.pos.y =  - bacgroundIntroData.rec.height;
                intro = false;
            }
        }
        
        // draw text start
        if (!intro && !playerInPosition) {
            Vector2 backgroundPos {backgroundX,0.0};
            DrawTextureEx(background, backgroundPos, 0.0, 1.0, WHITE);
            DrawText("Press SPACE_BAR to start", (windowDimension[0] / 2) * 0.5, windowDimension[1] / 2, 20, WHITE);
        }

        // start drawing
        BeginDrawing();
        ClearBackground(BLACK);

        // perform ground check
        
        if (trumpettData.pos.y >= windowDimension[1] - trumpettData.rec.height) {
             // reset animation on landing
            if (trumpettData.frame == 3) {
                trumpettData.frame = 4;
            }
            // trumpett on the ground
            isJumping = false;
            velocity = 0;
            trumpettData.pos.y = windowDimension[1] - trumpettData.rec.height;
        } else {
            // trumpett in the air
            isJumping = true;
            // apply gravity
            velocity += gravity * dt;
            trumpettData.frame = 3;
        }



        // check if player started the game and set player position
        if (isStarted) {
            if (trumpettData.pos.x >= 50) {
                trumpettData.pos.x += (jumpVelocity / 3) * dt;
                trumpettData.updateTime = 1.0 / 13.0;
            }

            if (trumpettData.pos.x <= 50) {
                playerInPosition = true;
                isStarted = false;
                trumpettData.frame = 4;
            }
        }

        // background scrolling
        if (!isStarted && playerInPosition && !gameOver) {
            backgroundX += -20 * dt;
            foregroundX += -40 *dt;

            if (backgroundX <= -background.width) {
                backgroundX = 0;
            }
            Vector2 backgroundPos {backgroundX,0.0};
            DrawTextureEx(background, backgroundPos, 0.0, 1.0, WHITE);
            Vector2 bacground2scrolling {backgroundX + background.width, 0.0};
            DrawTextureEx(background, bacground2scrolling, 0.0, 1.0, WHITE);

            if (foregroundX <= -forground.width) {
                foregroundX = 0;
            }
            Vector2 foregroundPos {foregroundX, 0.0};
            DrawTextureEx(forground, foregroundPos, 0.0, 1.0, WHITE);
            Vector2 foreground2scrolling {foregroundX + forground.width, 0.0};
            DrawTextureEx(forground, foreground2scrolling, 0.0, 1.0, WHITE);

            DrawText(TextFormat("Cash: %.0f$", cashCount), 10, 10, 30, GREEN);
        }




        if (IsKeyDown(KEY_SPACE)&& !intro && !playerInPosition) {
            isStarted = true;
        }

        if (IsKeyDown(KEY_SPACE) && !isJumping && playerInPosition && !intro) {
            velocity += jumpVelocity;
        }


        // update obstacle position
        if (playerInPosition && !gameOver) {
            obstacleData.pos.x += obstacleVelocity * dt;
        }
        

        // update trumpett position
        trumpettData.pos.y += velocity * dt;

        // update trumpett animation
        trumpettData.runningtime += dt;
        if (trumpettData.runningtime >= trumpettData.updateTime) {
            trumpettData.runningtime = 0;
            trumpettData.rec.x = trumpettData.frame * trumpettData.rec.width;
            trumpettData.frame ++;
            if (gameOver) {
                trumpettData.frame = 7;
                trumpettData.pos.x = windowDimension[0] / 2 - trumpettData.rec.width / 2;
            } else {
                if (!playerInPosition) {
                    if (trumpettData.frame > 2) {
                        trumpettData.frame = 0;
                    }
                } else {
                    if (trumpettData.frame > 6) {
                        trumpettData.frame = 4;
                    }
                }
            }
        }

        // update obstacle animation
        obstacleData.runningtime += dt;
        int randomObstacleX {rand() % 8};
        int randomObstacleY {rand() % 2};
        if (obstacleData.pos.x < 0 - obstacleData.rec.width) {
            if (randomObstacleX < 6 && randomObstacleY == 0) {
                obstacleToGain = true;
            } else {
                obstacleToGain = false;
            }
            //std::cout << "obstacleX: " << randomObstacleX << std::endl;
            obstacleData.pos.x = windowDimension[0];
            obstacleData.rec.x = randomObstacleX * obstacleData.rec.width;
            obstacleData.rec.y = randomObstacleY * obstacleData.rec.height;
        }

        // check collision
        int pad {30};
        Rectangle obstacleRec {
            obstacleData.pos.x + pad,
            obstacleData.pos.y + pad,
            obstacleData.rec.width - 2 * pad,
            obstacleData.rec.height - 2 * pad
        };

        Rectangle trumpettRec {
            trumpettData.pos.x + pad,
            trumpettData.pos.y + pad,
            trumpettData.rec.width - 2 * pad,
            trumpettData.rec.height - 2 * pad
        };

        if (CheckCollisionRecs(trumpettRec, obstacleRec)) {
            if (obstacleToGain) {
                PlaySound(gainMoney);
                cashCount += 100.0;
                // reset obstacle position
                obstacleData.pos.x = windowDimension[0] - (windowDimension[0] + obstacleData.rec.width);
                obstacleData.rec.x = (rand() % 8) * obstacleData.rec.width;
                obstacleToGain = false;
            } else {
                gameOver = true;
                StopMusicStream(soundtrack);
                PlaySound(gameOverSound);
            }

            
        }

        // draw obstacle
        if (!gameOver) {
            DrawTextureRec(obstacle, obstacleData.rec, obstacleData.pos, WHITE);
        }

        // draw trumpett
        if (!intro) {
            DrawTextureRec(trumpett, trumpettData.rec, trumpettData.pos, WHITE);
        }
        
        if (gameOver) {
            DrawTextureEx(backgroundGameOver, {0.0, 0.0}, 0.0, 1.0, WHITE);
            DrawText("GAME OVER", (windowDimension[0] / 2) - 61, (windowDimension[1] / 2 + 30), 20, RED);
            DrawText("From \"beloved\" Europe", (windowDimension[0] / 2) - 80, 25, 14, WHITE);
        }
        // stop drawing
        EndDrawing();
    }
    // unload texture
    UnloadTexture(bacgroundIntro);
    UnloadTexture(trumpett);
    UnloadTexture(obstacle);
    UnloadTexture(background);
    UnloadTexture(forground);
    UnloadTexture(backgroundGameOver);

    UnloadMusicStream(soundtrack);
    UnloadSound(gainMoney);

    // close audio device
    CloseAudioDevice();
    // close window
    CloseWindow();

    return 0;
}