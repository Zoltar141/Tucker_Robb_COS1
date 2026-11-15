#pragma once
#include "WonkleRuntime.h"
#include "ThemeManager.h"
#include "Render.h"
#include "FileBoss.h"
#include <chrono>
#include <random>
#include <string>
#include <vector>

class SNAKE
{
public:
    void Run();//controls gameloop
    SNAKE(ThemeManager& themeManager);

private:
    ThemeManager& themeManager;//theme addition
    Render render;

    WonkleRuntime runtime;//creates gamestate

    bool running = false;

    // Game setup
    void InitializeGame();//sets initial snake positions direction and score
    void SetupRules();//regisers gameplay rules

    // Game systems
    void HandleInput();//proccesses input
    void UpdateGame();//advances the gamestate
    void MoveSnake();//one move of the snake
    void RenderGame();///shows it in console
    void DrawCell(int cellX, int cellY, char character, const std::string& role);//draws one grid space
    void CheckGameOver();//win/loss check and restart

    FileBoss fileBoss;
    nlohmann::json snakeConfig;

    // Movement timing
    std::chrono::steady_clock::time_point lastFrameTime;//when the previous frame was measured
    double elapsedMilliseconds = 0;//time built up toward the next move

    std::mt19937 randomGenerator;//picks where food is tried

    std::vector<std::vector<int>> bodyTicks;//tick counter for every grid space, the head leaves one behind and it counts down to the tail
};

