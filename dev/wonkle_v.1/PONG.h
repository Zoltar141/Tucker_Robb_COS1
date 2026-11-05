#pragma once
#include "WonkleRuntime.h"
#include "ThemeManager.h"

class PONG
{
public:
    void Run();//controls gameloop
    PONG(ThemeManager& themeManager);

private:
    ThemeManager& themeManager;//colored theme addition

    WonkleRuntime runtime;//creates gamestate

    bool running = false;

    // Game setup
    void InitializeGame();//sets initial positions scores and velocities
    void SetupRules();//regisers gameplay rules

    // Game systems
    void HandleInput();//proccesses input
    void UpdateGame();//advances the gamestate
    void RenderGame();///shows it in console
    void CheckGameOver();//win/loss check

};

