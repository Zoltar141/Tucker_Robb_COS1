#pragma once
#include "GameController.h"
#include <chrono>
#include <random>
#include <string>
#include <vector>

class SNAKE : public GameController
{
public:
    SNAKE(ThemeManager& themeManager);

private:
    // Game setup
    void InitializeGame() override;//sets initial snake positions direction and score
    // Game systems
    void HandleInput() override;//proccesses input
    void UpdateGame() override;//advances the gamestate
    void MoveSnake();//one move of the snake
    void RenderGame() override;///shows it in console
    void DrawCell(int cellX, int cellY, char character, const std::string& role);//draws one grid space
    void CheckGameOver() override;//win/loss check and restart

    // Movement timing
    std::chrono::steady_clock::time_point lastFrameTime;//when the previous frame was measured
    double elapsedMilliseconds = 0;//time built up toward the next move

    std::mt19937 randomGenerator;//picks where food is tried

    std::vector<std::vector<int>> bodyTicks;//tick counter for every grid space, the head leaves one behind and it counts down to the tail
};

