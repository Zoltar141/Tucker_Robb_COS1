#pragma once
#include "GameController.h"

class PONG : public GameController
{
public:
    PONG(ThemeManager& themeManager);

private:
    // Game setup
    void InitializeGame() override;//sets initial positions scores and velocities

    // Game systems
    void HandleInput() override;//proccesses input
    void UpdateGame() override;//advances the gamestate
    void RenderGame() override;///shows it in console
    void CheckGameOver() override;//win/loss check
};

