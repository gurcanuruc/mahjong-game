#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 1600
#define HEIGHT 900
#define REC_WIDTH_SIZE 60
#define REC_HEIGHT_SIZE 100
#define BOARD_WIDTH_SIZE 8
#define BOARD_HEIGHT_SIZE 6
#define LAYER 3
#define MAX_VALUE 12
#define MAX_IMAGES 12
Texture2D backgroundTexture;
Texture2D tileTextures[MAX_IMAGES * 2];
Sound sound;
Sound soundx;
Sound soundm;
Sound sound1;
Sound sound2;
Sound sound3;

static bool musicLoaded = false;
static Music menuMusic;

// Oyun başladığında zamanlayıcıyı başlat
float timer = 0;
int countdown = 300; // 5 dakika = 300 saniye
bool gameLost = false;

// tas yapisi
typedef struct {
    int value; // tasin degeri
    bool isOpen; // tas kapali mi acik mi
} Tile;

// 3 boyutlu oyun tahtasi yapisi
typedef struct {
    Tile tiles[BOARD_HEIGHT_SIZE][BOARD_WIDTH_SIZE][LAYER]; // taslarin bulundugu matris
} Board3D;
typedef enum { MENU, GAME1, GAME2, GAME3, END } GameState;

Board3D gameBoard; // 3 boyutlu oyun tahtasi
Board3D gameBoard2; // 3 boyutlu oyun tahtasi 2
Vector3 selectedTile1 = { -1, -1, -1 }; // Secili taslarin kordinatlari
Vector3 selectedTile2 = { -1, -1, -1 };
int score = 0;

int boardX, boardY, boardWidth, boardHeight; // Tahta konum ve boyutlar
int buttonWidth = 200;
int buttonHeight = 50;
int buttonSpacing = 20;

// Oyun durumu
GameState gameState = MENU;

// Frekans dizisi
int frequency[MAX_VALUE + 1] = { 0 }; // Taş frekans değerlerini saklamak için dizi

// Fonksiyon prototipleri
void initializeBoard();
void initializeBoard1();
void initializeBoard2();
void drawMenu();
void selectTile(int x, int y, int z);
void checkMatch();
bool canSelectTile(int x, int y, int z);
void drawBoard();
void drawBoard1();
void loadResources();

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Mahjong");

    InitAudioDevice();

    loadResources();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (gameState)
        {
        case MENU:
            drawMenu();
            break;
        case GAME3:
            drawBoard();
            checkMatch();
            DrawText(TextFormat("Score: %d", score), 10, 10, 30, RAYWHITE);
            break;

        case GAME1:
            drawBoard1();
            checkMatch();
            DrawText(TextFormat("Score: %d", score), 10, 10, 30, RAYWHITE);
            break;

        case GAME2:
            drawBoard1();
            checkMatch();
            DrawText(TextFormat("Score: %d", score), 10, 10, 30, RAYWHITE);
            break;

        case END:
            DrawText(
                "Game Over", GetScreenWidth() / 2 - MeasureText("Game Over", 40) / 2, GetScreenHeight() / 2 - 20, 40, RED);
            break;
        }


        EndDrawing();
        UpdateMusicStream(menuMusic);
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
void initializeBoard()
{
    // Initialize random seed
    srand(time(NULL));

    // Load textures
    for (int i = 0; i < MAX_IMAGES * 2; i++)
    {
        char filename[25];
        sprintf(filename, "%d.png", i + 1); // Assuming your image files are named 1.png, 2.png, ..., 12.png
        tileTextures[i] = LoadTexture(filename);
    }
    // Initialize game board matrix
    for (int i = 0; i < BOARD_HEIGHT_SIZE; i++)
    {
        for (int j = 0; j < BOARD_WIDTH_SIZE; j++)
        {
            for (int k = 0; k < LAYER; k++)
            {
                gameBoard.tiles[i][j][k].value = (rand() % MAX_VALUE) + 1;
                gameBoard.tiles[i][j][k].isOpen = false;
            }
        }
    }
}
void initializeBoard1()
{
    // Load textures
    for (int i = 0; i < MAX_IMAGES ; i++)
    {
        char filename[25];
        sprintf(filename, "%d.png", i + 1); // Assuming your image files are named 1.png, 2.png, ..., 12.png
        tileTextures[i] = LoadTexture(filename);
    }

    // Initialize game board matrix for z = 0
    int board1_z0[BOARD_HEIGHT_SIZE][BOARD_WIDTH_SIZE] = {
        {1, 8, 6, 10, 5, 9, 6, 5},
        {4, -1, 1, -1, 2, 8, -1, 9},
        {2, -1, 3, 3, -1, 9, -1, 4},
        {8, -1, 8, -1, 12, 7, -1, 6},
        {9, -1, 1, 12, -1, 1, -1, 10},
        {11, 11, 6, 7, 10, 4, 4, 10}
    };

    // Initialize game board matrix for z = 1
    int board1_z1[BOARD_HEIGHT_SIZE][BOARD_WIDTH_SIZE] = {
        {7, 2, 9, 12, 5, 3, 11, 12},
        {2, -1, 9, -1, 6, 12, -1, 3},
        {10, -1, 6, 1, -1, 10, -1, 7},
        {3, -1, 1, -1, 8, 8, -1, 4},
        {3, -1, 7, 4, -1, 7, -1, 5},
        {12, 11, 11, 5, 11, 2, 5, 2}
    };

    // Initialize game board matrix for z = 2
    int board1_z2[BOARD_HEIGHT_SIZE][BOARD_WIDTH_SIZE] = {
        {5, 10, 8, 12, 9, 3, 8, 12},
        {4, -1, 4, -1, -1, 12, -1, 2},
        {7, -1, 12, -1, -1, 1, -1, 11},
        {10, -1, 6, -1, -1, 1, -1, 11},
        {5, -1, 7, -1, -1, 9, -1, 3},
        {3, 10, 4, 3, 6, 2, 4, 10}
    };

    // Initialize game board matrix for z = 0
    for (int i = 0; i < BOARD_HEIGHT_SIZE; i++)
    {
        for (int j = 0; j < BOARD_WIDTH_SIZE; j++)
        {
            gameBoard.tiles[i][j][0].value = board1_z0[i][j];
            gameBoard.tiles[i][j][0].isOpen = false;
        }
    }

    // Initialize game board matrix for z = 1
    for (int i = 0; i < BOARD_HEIGHT_SIZE; i++)
    {
        for (int j = 0; j < BOARD_WIDTH_SIZE; j++)
        {
            gameBoard.tiles[i][j][1].value = board1_z1[i][j];
            gameBoard.tiles[i][j][1].isOpen = false;
        }
    }

    // Initialize game board matrix for z = 2
    for (int i = 0; i < BOARD_HEIGHT_SIZE; i++)
    {
        for (int j = 0; j < BOARD_WIDTH_SIZE; j++)
        {
            gameBoard.tiles[i][j][2].value = board1_z2[i][j];
            gameBoard.tiles[i][j][2].isOpen = false;
        }
    }
}
void initializeBoard2()//normal
{
    // Load textures
    for (int i = 0; i < MAX_IMAGES ; i++)
    {
        char filename[25];
        sprintf(filename, "%d.png", i + 1); // Assuming your image files are named 1.png, 2.png, ..., 12.png
        tileTextures[i] = LoadTexture(filename);
    }

    // Initialize game board matrix for z = 0
    int board1_z0[BOARD_HEIGHT_SIZE][BOARD_WIDTH_SIZE] = {
        {-1, 2, 9, 11, 5, 11, 7, -1},
        {1, -1, 4, 1, 5, 9, -1, 6},
        {-1, 12, -1, 10, 10, -1, 9, -1},
        {-1, 11, -1, 1, 7, -1, 11, -1},
        {6, -1, 7, 3, 4, 12, -1, 2},
        {-1, 1, 7, 3, 9, 8, 8, -1}
    };

    // Initialize game board matrix for z = 1
    int board1_z1[BOARD_HEIGHT_SIZE][BOARD_WIDTH_SIZE] = {
        {-1, 12, 8, 11, 12, 11, 9, -1},
        {5, -1, 3, 2, 5, 1, -1, 7},
        {-1, 8, -1, 4, 7, -1, 4, -1},
        {-1, 9, -1, 6, 2, -1, 3, -1},
        {5, -1, 10, 12, 5, 2, -1, 12},
        {-1, 6, 11, 2, 1, 11, 10, -1}
    };

    // Initialize game board matrix for z = 2
    int board1_z2[BOARD_HEIGHT_SIZE][BOARD_WIDTH_SIZE] = {
        {-1, 5, 7, 1, 11, 12, 6, -1},
        {4, -1, 3, 6, 8, 3, -1, 12},
        {-1, 11, -1, 9, 8, -1, 7, -1},
        {-1, 12, -1, 4, 3, -1, 6, -1},
        {2, -1, 4, 2, 9, 10, -1, 5},
        {-1, 10, 12, 6, 1, 4, 3, -1}
    };

    // Initialize game board matrix for z = 0
    for (int i = 0; i < BOARD_HEIGHT_SIZE; i++)
    {
        for (int j = 0; j < BOARD_WIDTH_SIZE; j++)
        {
            gameBoard.tiles[i][j][0].value = board1_z0[i][j];
            gameBoard.tiles[i][j][0].isOpen = false;
        }
    }

    // Initialize game board matrix for z = 1
    for (int i = 0; i < BOARD_HEIGHT_SIZE; i++)
    {
        for (int j = 0; j < BOARD_WIDTH_SIZE; j++)
        {
            gameBoard.tiles[i][j][1].value = board1_z1[i][j];
            gameBoard.tiles[i][j][1].isOpen = false;
        }
    }

    // Initialize game board matrix for z = 2
    for (int i = 0; i < BOARD_HEIGHT_SIZE; i++)
    {
        for (int j = 0; j < BOARD_WIDTH_SIZE; j++)
        {
            gameBoard.tiles[i][j][2].value = board1_z2[i][j];
            gameBoard.tiles[i][j][2].isOpen = false;
        }
    }
}
void loadResources()
{
    backgroundTexture = LoadTexture("arka.png");
    sound = LoadSound("click.mp3");
    soundx = LoadSound("match.mp3");
    soundm = LoadSound("menu_sound.mp3");

    menuMusic = LoadMusicStream("menu_sound.mp3"); // menuMusic'i yükle
    PlayMusicStream(menuMusic); // Müziği çal
}

void drawBoard()
{
    // Calculate board dimensions
    boardWidth = BOARD_WIDTH_SIZE * REC_WIDTH_SIZE;
    boardHeight = BOARD_HEIGHT_SIZE * REC_HEIGHT_SIZE;

    // Calculate board position
    boardX = (GetScreenWidth() - boardWidth) / 2;
    boardY = (GetScreenHeight() - boardHeight) / 2;

    // Draw background image
    DrawTexture(backgroundTexture, 0, 0, WHITE);

    // Iterate over each tile on the board
    for (int k = LAYER - 1; k >= 0; k--)
    {
        for (int i = 0; i < BOARD_HEIGHT_SIZE; i++)
        {
            for (int j = 0; j < BOARD_WIDTH_SIZE; j++)
            {
                if (gameBoard.tiles[i][j][k].value != -1)
                {
                    // Get the texture index based on the tile value
                    int textureIndex = gameBoard.tiles[i][j][k].value - 1;

                    // Draw the texture
                    DrawTexture(tileTextures[textureIndex], boardX + j * REC_WIDTH_SIZE + k * 6, boardY + i * REC_HEIGHT_SIZE + k * 6, WHITE);

                    if (selectedTile1.x == j && selectedTile1.y == i && selectedTile1.z == k)
                    {
                        // Seçili taşın üzerini maviye boyayın
                        DrawTexture(tileTextures[textureIndex], boardX + j * REC_WIDTH_SIZE + k * 6, boardY + i * REC_HEIGHT_SIZE + k * 6, Fade(DARKBLUE, 0.7f));
                    }
                }
            }
        }
    }
    // Draw back to menu button
    DrawRectangle(GetScreenWidth() / 2 - buttonWidth / 2, GetScreenHeight() - buttonHeight - 20, buttonWidth, buttonHeight, GRAY);
    DrawText("Back to Menu", GetScreenWidth() / 2 - MeasureText("Back to Menu", 20) / 2, GetScreenHeight() - buttonHeight - 10, 20, WHITE);

    // Check if the back to menu button is pressed
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { GetScreenWidth() / 2 - buttonWidth / 2, GetScreenHeight() - buttonHeight - 20, buttonWidth, buttonHeight }))
    {
        DrawRectangle(GetScreenWidth() / 2 - buttonWidth / 2, GetScreenHeight() - buttonHeight - 20, buttonWidth, buttonHeight, Fade(SKYBLUE, 0.4f));
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            timer = 0;
            // Change the game state to MENU
            gameState = MENU;
        }
    }

    // Zamanlayıcıyı güncelle
    timer += GetFrameTime();

    // Zamanlayıcıyı ekrana çiz (dakika ve saniye cinsinden)
    int minutes = (int)timer / 60;
    int seconds = (int)timer % 60;
    DrawText(TextFormat("Time: %02d:%02d", minutes, seconds), GetScreenWidth() - 150, 10, 25, RAYWHITE);

    // Zamanlayıcı 5 dakikayı geçti mi kontrol et
    if (timer > countdown) // 5 dakika = 300 saniye
    {
        gameLost = true;
    }

    // Oyun kaybedildiyse, "Retry" butonunu çiz
    if (gameLost)
    {
        DrawRectangle(GetScreenWidth() / 2 - 50, GetScreenHeight() / 2 - 25, 100, 50, RED);
        DrawText("Retry", GetScreenWidth() / 2 - MeasureText("Retry", 20) / 2, GetScreenHeight() / 2 - 10, 20, WHITE);

        // "Retry" butonuna tıklandı mı kontrol et
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { GetScreenWidth() / 2 - 50, GetScreenHeight() / 2 - 25, 100, 50 }) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            // Oyunu yeniden başlat
            initializeBoard();
            timer = 0; // Zamanlayıcıyı sıfırla
            gameLost = false; // Oyun kaybedilmedi olarak işaretle
        }
    }
}
void drawBoard1()
{
    // Calculate board dimensions
    boardWidth = BOARD_WIDTH_SIZE * REC_WIDTH_SIZE;
    boardHeight = BOARD_HEIGHT_SIZE * REC_HEIGHT_SIZE;

    // Calculate board position
    boardX = (GetScreenWidth() - boardWidth) / 2;
    boardY = (GetScreenHeight() - boardHeight) / 2;

    // Draw background image
    DrawTexture(backgroundTexture, 0, 0, WHITE);

    // Iterate over each tile on the board
    for (int k = 3 - 1; k >= 0; k--)
    {
        for (int i = 0; i < BOARD_HEIGHT_SIZE; i++)
        {
            for (int j = 0; j < BOARD_WIDTH_SIZE; j++)
            {
                if (gameBoard.tiles[i][j][k].value != -1)
                {
                    // Get the texture index based on the tile value
                    int textureIndex = gameBoard.tiles[i][j][k].value - 1;

                    // Draw the texture
                    DrawTexture(tileTextures[textureIndex], boardX + j * REC_WIDTH_SIZE + k * 6, boardY + i * REC_HEIGHT_SIZE + k * 6, WHITE);

                    if (selectedTile1.x == j && selectedTile1.y == i && selectedTile1.z == k)
                    {
                        // Seçili taşın üzerini maviye boyayın
                        DrawTexture(tileTextures[textureIndex], boardX + j * REC_WIDTH_SIZE + k * 6, boardY + i * REC_HEIGHT_SIZE + k * 6, Fade(DARKBLUE, 0.7f));
                    }
                }
            }
        }
    }
    // Draw back to menu button
    DrawRectangle(GetScreenWidth() / 2 - buttonWidth / 2, GetScreenHeight() - buttonHeight - 20, buttonWidth, buttonHeight, GRAY);
    DrawText("Back to Menu", GetScreenWidth() / 2 - MeasureText("Back to Menu", 20) / 2, GetScreenHeight() - buttonHeight - 10, 20, WHITE);

    // Check if the back to menu button is pressed
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { GetScreenWidth() / 2 - buttonWidth / 2, GetScreenHeight() - buttonHeight - 20, buttonWidth, buttonHeight }))
    {
        DrawRectangle(GetScreenWidth() / 2 - buttonWidth / 2, GetScreenHeight() - buttonHeight - 20, buttonWidth, buttonHeight, Fade(SKYBLUE, 0.4f));
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            timer = 0;
            // Change the game state to MENU
            gameState = MENU;
        }
    }
    // Zamanlayıcıyı güncelle
    timer += GetFrameTime();

    // Zamanlayıcıyı ekrana çiz (dakika ve saniye cinsinden)
    int minutes = (int)timer / 60;
    int seconds = (int)timer % 60;
    DrawText(TextFormat("Time: %02d:%02d", minutes, seconds), GetScreenWidth() - 150, 10, 25, RAYWHITE);

    // Zamanlayıcı 5 dakikayı geçti mi kontrol et
    if (timer > countdown) // 5 dakika = 300 saniye
    {
        gameLost = true;
    }

    // Oyun kaybedildiyse, "Retry" butonunu çiz
    if (gameLost)
    {
        DrawRectangle(GetScreenWidth() / 2 - 50, GetScreenHeight() / 2 - 25, 100, 50, RED);
        DrawText("Retry", GetScreenWidth() / 2 - MeasureText("Retry", 20) / 2, GetScreenHeight() / 2 - 10, 20, WHITE);

        // "Retry" butonuna tıklandı mı kontrol et
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { GetScreenWidth() / 2 - 50, GetScreenHeight() / 2 - 25, 100, 50 }) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            // Oyunu yeniden başlat
            initializeBoard();
            timer = 0; // Zamanlayıcıyı sıfırla
            gameLost = false; // Oyun kaybedilmedi olarak işaretle
        }
    }
}

void drawMenu() {
    // Müziği çal veya duraklat
    if (!IsMusicStreamPlaying(menuMusic)) {
        PlayMusicStream(menuMusic);
    }

    DrawTexture(backgroundTexture, 0, 0, WHITE);
    int buttonX = GetScreenWidth() / 2 - buttonWidth / 2;
    int buttonSpacing = 20;
    int level1ButtonY = GetScreenHeight() / 2 - buttonHeight - buttonSpacing;
    int level2ButtonY = level1ButtonY + buttonHeight + buttonSpacing;
    int level3ButtonY = level2ButtonY + buttonHeight + buttonSpacing;

    DrawText("MAHJONG", GetScreenWidth() / 2 - MeasureText("MAHJONG", 100) / 2, 80, 100, RAYWHITE);

    DrawRectangle(buttonX, level1ButtonY, buttonWidth, buttonHeight, GRAY);
    DrawRectangle(buttonX, level2ButtonY, buttonWidth, buttonHeight, GRAY);
    DrawRectangle(buttonX, level3ButtonY, buttonWidth, buttonHeight, GRAY);

    DrawText("EASY", buttonX + buttonWidth / 2 - MeasureText("LEVEL 1", 15) / 2, level1ButtonY + buttonHeight / 2 - 10, 20, WHITE);
    DrawText("MEDIUM", buttonX + buttonWidth / 2 - MeasureText("LEVEL 2", 20) / 2, level2ButtonY + buttonHeight / 2 - 10, 20, WHITE);
    DrawText("HARD", buttonX + buttonWidth / 2 - MeasureText("LEVEL 3", 15) / 2, level3ButtonY + buttonHeight / 2 - 10, 20, WHITE);

    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { buttonX, level1ButtonY, buttonWidth, buttonHeight })) {
        DrawRectangle(buttonX, level1ButtonY, buttonWidth, buttonHeight, Fade(SKYBLUE, 0.4f));
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            StopMusicStream(menuMusic);
            initializeBoard1();
            // Diğer başlangıç ayarlarını yapabilirsiniz, örneğin skoru sıfırlayabilirsiniz
            score = 0;
            // Oyun durumunu oynanabilir hale getirir, başka düğmelerin etkileşimini etkinleştirebilir
            gameState = GAME1;
        }
    }
    else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { buttonX, level2ButtonY, buttonWidth, buttonHeight })) {
        DrawRectangle(buttonX, level2ButtonY, buttonWidth, buttonHeight, Fade(SKYBLUE, 0.4f));
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            StopMusicStream(menuMusic);
            initializeBoard2();
            score = 0;
            gameState = GAME2;
        }
    }
    else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { buttonX, level3ButtonY, buttonWidth, buttonHeight })) {
        DrawRectangle(buttonX, level3ButtonY, buttonWidth, buttonHeight, Fade(SKYBLUE, 0.4f));
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            StopMusicStream(menuMusic);
            initializeBoard();
            score = 0;
            gameState = GAME3;
        }
    }
}
void checkMatch()
{
    bool selected[BOARD_HEIGHT_SIZE][BOARD_WIDTH_SIZE][LAYER] = { false };

    int matchValues[BOARD_WIDTH_SIZE * BOARD_HEIGHT_SIZE * LAYER] = { 0 }; // eşleşebilecek taşların değerlerini saklamak için dizi
    int matchCount[BOARD_WIDTH_SIZE * BOARD_HEIGHT_SIZE * LAYER] = { 0 }; // Eşleşen taşların frekansını saklamak için dizi

    // Her fare tıklamasında kontrol edilecek
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        // Fare konumunu al
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();

        // tahtanın içinde mi kontrol et
        if (mouseX >= boardX && mouseX < boardX + boardWidth && mouseY >= boardY && mouseY < boardY + boardHeight)
        {
            int x = (mouseX - boardX) / REC_WIDTH_SIZE;
            int y = (mouseY - boardY) / REC_HEIGHT_SIZE;

            // Tüm katmanları gezmek için döngü
            for (int z = 0; z < LAYER; z++) {
                // Seçilen taş tıklanabilir mi kontrol et
                if (canSelectTile(x, y, z))
                {
                    // Seçili taşı güncelle
                    selectTile(x, y, z);

                    // İki taş seçildi mi kontrol et
                    if (selectedTile1.x != -1 && selectedTile1.y != -1 && selectedTile2.x != -1 && selectedTile2.y != -1)
                    {
                        // Seçili taşlar -1 değilse ve eşleşiyorsa
                        if (gameBoard.tiles[(int)selectedTile1.y][(int)selectedTile1.x][(int)selectedTile1.z].value != -1 &&
                            gameBoard.tiles[(int)selectedTile2.y][(int)selectedTile2.x][(int)selectedTile2.z].value != -1 &&
                            gameBoard.tiles[(int)selectedTile1.y][(int)selectedTile1.x][(int)selectedTile1.z].value == gameBoard.tiles[(int)selectedTile2.y][(int)selectedTile2.x][(int)selectedTile2.z].value)
                        {
                            // Eşleşen taşları kaldır
                            gameBoard.tiles[(int)selectedTile1.y][(int)selectedTile1.x][(int)selectedTile1.z].value = -1;
                            gameBoard.tiles[(int)selectedTile2.y][(int)selectedTile2.x][(int)selectedTile2.z].value = -1;
                            selectedTile1.x = -1;
                            selectedTile1.y = -1;
                            selectedTile1.z = -1;
                            selectedTile2.x = -1;
                            selectedTile2.y = -1;
                            selectedTile2.z = -1;

                            PlaySound(soundx);
                            score += 5;
                        }
                        else
                        {
                            // Eşleşme yoksa taşları seçilmemiş yap
                            gameBoard.tiles[(int)selectedTile1.y][(int)selectedTile1.x][(int)selectedTile1.z].isOpen = false;
                            gameBoard.tiles[(int)selectedTile2.y][(int)selectedTile2.x][(int)selectedTile2.z].isOpen = false;
                            selectedTile1.x = -1;
                            selectedTile1.y = -1;
                            selectedTile1.z = -1;
                            selectedTile2.x = -1;
                            selectedTile2.y = -1;
                            selectedTile2.z = -1;
                        }
                    }
                    break;
                }
            }
        }
    }
    // Tüm taşların değerlerini kontrol et
    bool allTilesMatched = true;
    for (int i = 0; i < BOARD_HEIGHT_SIZE; i++)
    {
        for (int j = 0; j < BOARD_WIDTH_SIZE; j++)
        {
            for (int k = 0; k < LAYER; k++)
            {
                if (gameBoard.tiles[i][j][k].value != -1)
                {
                    // En az bir taş eşleşmediyse, oyun devam ediyor demektir
                    allTilesMatched = false;
                    break;
                }
            }
            if (!allTilesMatched)
            {
                break;
            }
        }
        if (!allTilesMatched)
        {
            break;
        }
    }

    // Eğer tüm taşlar eşleştiyse, yeni bir oyun tahtası oluştur
    if (allTilesMatched)
    {
        if (gameState == GAME1) {
            WaitTime(3);
            initializeBoard2();
        }
        else if (gameState == GAME2) {
            WaitTime(3);
            initializeBoard();
        }
    }
}

bool isFirstTileSelected = false; // ilk taş seçildi mi?
bool isSecondTileSelected = false; // ikinci taş seçildi mi?

void selectTile(int x, int y, int z)
{
    // İlk taş seçildiğinde
    if (!isFirstTileSelected) {
        selectedTile1.x = x;
        selectedTile1.y = y;
        selectedTile1.z = z;
        isFirstTileSelected = true;
        gameBoard.tiles[y][x][z].isOpen = true; // Taş aç
        PlaySound(sound);

    }

    else if (!isSecondTileSelected) {
        selectedTile2.x = x;
        selectedTile2.y = y;
        selectedTile2.z = z;
        isSecondTileSelected = true;
        gameBoard.tiles[y][x][z].isOpen = true; // Taşı aç
        PlaySound(sound);

        //Eşleşme kontrolünden sonra frekansı kontrol et
        checkMatch();

        // iki taş seçildikten sonra seçimleri sıfırla
        isFirstTileSelected = false;
        isSecondTileSelected = false;
    }
}
bool canSelectTile(int x, int y, int z)
{
    // Taşın board sınırları içinde olup olmadığını kontrol et
    if (x >= 0 && x < BOARD_WIDTH_SIZE && y >= 0 && y < BOARD_HEIGHT_SIZE && z >= 0 && z < LAYER) {
        // Taşın seçilip seçilemeyeceğini kontrol et
        if (!gameBoard.tiles[y][x][z].isOpen && !isSecondTileSelected) {
            // Eğer hiçbir taş seçili değilse veya sadece bir taş seçiliyse, bu taş seçilebilir
            if (!isFirstTileSelected || (selectedTile1.x != x || selectedTile1.y != y || selectedTile1.z != z)) {
                // Üstteki taşın durumunu kontrol et
                bool topTileEmpty = false;
                if (z < LAYER - 1) {
                    topTileEmpty = gameBoard.tiles[y][x][z + 1].value == -1; // Altındaki taşın olmamasını kontrol et
                }
                // Sağdaki taşın durumunu kontrol et
                bool rightTileFilled = false;
                if (x < BOARD_WIDTH_SIZE - 1) {
                    rightTileFilled = gameBoard.tiles[y][x + 1][z].value != -1; // Sağdaki taşın olup olmadığını kontrol et
                }
                // Soldaki taşın durumunu kontrol et
                bool leftTileFilled = false;
                if (x > 0) {
                    leftTileFilled = gameBoard.tiles[y][x - 1][z].value != -1; // Soldaki taşın olup olmadığını kontrol et
                }
                // Koşullara göre taşın seçilebilirliğini belirle
                if (topTileEmpty || (!leftTileFilled || !rightTileFilled)) {
                    return true;
                }
            }
        }
    }
    return false;
}
