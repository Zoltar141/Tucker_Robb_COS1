#pragma once
#include <map>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "ThemeManager.h"
#include "Render.h"
#include "FileBoss.h"
#include "Boundary.h"
#include "WonkleRuntime.h"

//everything the games share, written once. Each game is a child of this class and only adds what makes it that game
class GameController
{
public:
    GameController(ThemeManager& themeManager, const std::string& fileName);//loads the game's json
    virtual ~GameController() = default;

    void Run();//controls gameloop, the same for every game

protected:
    ThemeManager& themeManager;//colored theme addition
    Render render;//render seperated from the games

    WonkleRuntime runtime;//creates gamestate

    FileBoss fileBoss;
    nlohmann::json gameConfig;//the game's json

    bool running = false;

    // Every game provides its own version of these.
    virtual void InitializeGame() = 0;//sets the starting state
    virtual void HandleInput() = 0;//proccesses input
    virtual void UpdateGame() = 0;//advances the gamestate
    virtual void RenderGame() = 0;//shows it in console
    virtual void CheckGameOver() = 0;//win/loss check

    // Game setup
    void SetupRules();//regisers the gameplay rules written in the game's json

    // Game state
    void SetInitialState();//puts every value in the game's initial_state into the runtime
    std::map<std::string, int> LoadGameState();//reads every value in the game's initial_state back out of the runtime

    // Game loop
    void StartConsole();//hides the cursor and clears the menu off the screen
    void WaitForNextFrame();//keeps the game from running as fast as the processor allows
    void EndConsole();//clears the screen and brings the cursor back

    // Game field
    Boundary GetBoundary();//builds the boundary from the game's field

    // Rendering
    void BeginFrame();//starts a new frame the size of the game's field
    void DrawScoreboard(const std::string& text);
    void DrawBoundary(std::vector<RenderObject>& objects);
    void DrawBottomText(const std::string& text);//the line under the field, controls or the result

    // Game over
    std::string GetGameOverText(bool gameWon, int score);//result, final score and restart prompt
    void ClearKeys();//throws away keys typed during play
    bool AskToRestart();//waits for the restart or exit key, true means restart
};

