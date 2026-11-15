#pragma once
#include "GameController.h"
#include <string>
#include <vector>

class BRICKBREAKER : public GameController
{
public:
    BRICKBREAKER(ThemeManager& themeManager);

private:
    // Game setup
    void InitializeGame() override;//sets initial positions score bricks and velocities

    // Game systems
    void HandleInput() override;//proccesses input
    void UpdateGame() override;//advances the gamestate
    void RenderGame() override;///shows it in console
    void CheckGameOver() override;//win/loss check and restart

    std::vector<std::string> brickRows;//the bricks, each character is how many hits that brick has left
};


