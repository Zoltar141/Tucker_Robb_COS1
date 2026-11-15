#pragma once
#include "WonkleRuntime.h"
#include "ThemeManager.h"
#include "Render.h"
#include "FileBoss.h"
#include <string>
#include <vector>

//so this is basically a copy of PONG.h but for brickbreaker
// i will have to refactor and possibly make a game controller class 
// for things that are duplicated

class BRICKBREAKER
{
public:
    void Run();//controls gameloop
    BRICKBREAKER(ThemeManager& themeManager);

private:
    ThemeManager& themeManager;// theme addition
    Render render;

    WonkleRuntime runtime;//creates gamestate

    bool running = false;

    // Game setup
    void InitializeGame();//sets initial positions score bricks and velocities
    void SetupRules();//regisers gameplay rules

    // Game systems
    void HandleInput();//proccesses input
    void UpdateGame();//advances the gamestate
    void RenderGame();///shows it in console
    void CheckGameOver();//win/loss check and restart

    FileBoss fileBoss;
    nlohmann::json brickBreakerConfig;

    std::vector<std::string> brickRows;//the bricks, each character is how many hits that brick has left
};


