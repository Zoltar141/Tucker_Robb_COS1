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

    void Run(bool loadSave = false);//controls gameloop, the same for every game. true = load game

    // Save file
    // every game's save and high score live in this one file
    static constexpr const char* SAVE_FILE = "wonkle_save.json";

    bool HasSave() const;//true when this game has a save to load, the game menu uses it
    static bool ResetAllSaveData();//wipes every save and high score, for the settings menu
    static inline bool muted = false; //to just turn sound off 

protected:
    ThemeManager& themeManager;//colored theme addition
    Render render;//render seperated from the games

    WonkleRuntime runtime;//creates gamestate

    FileBoss fileBoss;
    nlohmann::json gameConfig;//the game's json

    bool running = false;
    //state value as text to show decimals
    std::string FormatValue(const std::string& key);
    // Every game provides its own version of these.
    virtual void InitializeGame() = 0;//sets the starting state
    virtual void HandleInput() = 0;//proccesses input
    virtual void UpdateGame() = 0;//advances the gamestate
    virtual void RenderGame() = 0;//shows it in console
    virtual void CheckGameOver();//win/loss check

    // A game puts anything it keeps outside the runtime into its save with these, the bricks or the snake's body for example.
     // Games that keep everything in the runtime leave them alone.
    virtual void SaveExtras(nlohmann::json& save) {}
    virtual void LoadExtras(const nlohmann::json& save) {}

    // Game state
    void SetInitialState();//puts every value in the game's initial_state into the runtime
    std::map<std::string, int> GetGameState();//reads every value in the game's initial_state back out of the runtime
    void SetGameState(const std::map<std::string, int>& state);//puts every value back into the runtime

    // Saving
    bool SaveGame();//writes every initial_state value and the game's extras to its slot in the save file
    bool LoadGame();//starts the game over from its slot in the save file, false when it could not
    int GetHighScore() const;//the best score this game has had, for a scoreboard

    // Game field
    Boundary GetBoundary();//builds the boundary from the game's field

    // Rendering
    void BeginFrame();//starts a new frame the size of the game's field
    void DrawScoreboard(const std::string& text);
    void AddRectangle(const std::string& name, int x, int y, int width = 1, int height = 1);//adds a block of a named character, like the paddle or ball
    void AddText(int x, int y, const std::string& text, const std::string& role);//adds text to the field, it lands on top of the boundary and anything added before it
    void EndFrame();//draws everything added, the line under the field, and shows the frame

private:
    std::vector<RenderObject> objects;//everything added to the field this frame

    std::string gameName;//the json's file name without .json, names the game's slot in the save file
    int highScore = 0;//best score so far, read from the save file when the game starts

    bool paused = false;
    std::string pauseStatus;//shown in front of the pause options, PAUSED or what the last choice did

    // a game can list sounds in its json, each one plays when that state value changes.
    // the list and the sound files are both optional, a game without them just plays silent:
    std::map<std::string, double> soundSnapshot;//the watched values as of the last check

    // Game setup
    void SetupRules();//regisers the gameplay rules written in the game's json

    // Game loop
    void StartConsole();//hides the cursor and clears the menu off the screen
    void WaitForNextFrame();//keeps the game from running as fast as the processor allows
    void EndConsole();//clears the screen and brings the cursor back

    // Pause menu
    void RunPauseMenu();//ESC during play, resume, save, load or quit to the menu
    void WaitForKeyRelease(int key);//waits until a held key comes back up

    // Sound
    void TakeSoundSnapshot();//remembers every watched value so only changes from here on play a sound
    void PlayStateSounds();//plays the sound of the first watched value that changed since the last check
    void PlaySoundFile(const std::string& file);//plays a wav without stopping the game, a missing file is skipped

    // Save file
    static nlohmann::json ReadSaveFile();//the whole save file, or an empty one when there is none yet
    static bool WriteSaveFile(const nlohmann::json& saveData);
    bool HasHighScore() const;//games with a "score" in initial_state keep a high score
    void RecordHighScore();//keeps the score when it beats the best one

    // Game over
    void ClearKeys();//throws away keys typed during play
    bool AskToRestart();//waits for the restart or exit key, true means restart
};

