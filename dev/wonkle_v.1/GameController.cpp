#include "GameController.h"
#include "InputHelper.h"
#include "Colors.h"
#include <thread>
#include <chrono>
#include <Windows.h>
#include <stdexcept>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <filesystem>

// PlaySoundA lives in winmm, this links it without touching the project settings.
#pragma comment(lib, "winmm.lib")

// Pause menu keys, numbers like the rest of Wonkle's menus.
constexpr char PAUSE_RESUME = '1';
constexpr char PAUSE_SAVE = '2';
constexpr char PAUSE_LOAD = '3';
constexpr char PAUSE_QUIT = '4';

// _getch hands arrow and function keys over as two keys, one of these and then the key itself.
constexpr char EXTENDED_KEY = 0;
constexpr char EXTENDED_KEY_ALT = static_cast<char>(224);

constexpr int KEY_RELEASE_CHECK_MS = 10;

// Pause menu text, it takes the line under the field while the game is paused.
const std::string PAUSE_OPTIONS = "    1: Resume    2: Save    3: Load    4: Quit to Menu";
const std::string PAUSED_TEXT = "PAUSED";
const std::string SAVED_TEXT = "GAME SAVED";
const std::string SAVE_FAILED_TEXT = "SAVE FAILED";
const std::string LOADED_TEXT = "GAME LOADED";
const std::string LOAD_FAILED_TEXT = "LOAD FAILED";
const std::string NO_SAVE_TEXT = "NO SAVE FOUND";

const std::string HIGH_SCORE_TEXT = "  High Score: ";

// A sound entry needs a key to watch and a file to play, anything else in the list is skipped.
static bool IsSoundEntry(const nlohmann::json& sound)
{
    return sound.is_object()
        && sound.contains("key") && sound["key"].is_string()
        && sound.contains("file") && sound["file"].is_string();
}

//setup
GameController::GameController(ThemeManager& themeManager, const std::string& fileName)//constructor, connects the game to ThemeManager and loads its json
    : themeManager(themeManager), render(themeManager)
{
    if (!fileBoss.ReadJSON(fileName, gameConfig))
    {
        throw std::runtime_error("Failed to load " + fileName);
    }
    render.SetConfiguration(gameConfig["game"]["rendering"]);
}

void GameController::SetupRules()
{
    runtime.LoadRules(gameConfig["game"]["rules"]);
}


//gamestate
void GameController::SetInitialState()
{
    // initial_state in the json is the list of every value the game keeps and what it starts at.
    for (const auto& value : gameConfig["game"]["initial_state"].items())
    {
        runtime.SetStateValue(value.key(), value.value().get<double>());
    }
}

std::map<std::string, int> GameController::GetGameState()
{
    std::map<std::string, int> state;

    // Retrieve every value the game listed in initial_state.
    for (const auto& value : gameConfig["game"]["initial_state"].items())
    {
        runtime.GetStateValue(value.key(), state[value.key()]);
    }

    return state;
}

void GameController::SetGameState(const std::map<std::string, int>& state)
{
    for (const auto& value : state)
    {
        int current = 0;
        runtime.SetStateValue(value.first, current);
        
        //keep decimals written in json
        if (current != value.second)
        {
            runtime.SetStateValue(value.first, value.second);
        }
    }
}

std::string GameController::FormatValue(const std::string& key)
{
    double value = 0;
    runtime.GetStateValue(key, value);

    if (value == std::floor(value))
    {
        return std::to_string(static_cast<long long>(value));
    }

    std::ostringstream text;

    text << std::fixed << std::setprecision(2) << value;
    
    std::string result = text.str();
    
    result.erase(result.find_last_not_of('0') + 1);

    return result;
}

//Loop
void GameController::Run(bool loadSave)//controls the game loop
{
    InitializeGame();
    SetupRules();

    // The best score so far comes out of the save file.
    highScore = ReadSaveFile()["high_scores"].value(gameName, 0);

    // Picking Load Save in the game menu starts from the save instead.
    if (loadSave)
    {
        LoadGame();
    }

    // Only changes from here on play a sound.
    TakeSoundSnapshot();

    StartConsole();

    running = true;

    while (running)
    {
        HandleInput();
        // Exit the current game.
        if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["exit"].get<int>()))
        {
            RunPauseMenu();

            if (!running)
            {
                break;
            }
        }
        UpdateGame();
        PlayStateSounds();
        RenderGame();
        CheckGameOver();
        WaitForNextFrame();
    }

    EndConsole();
}

void GameController::StartConsole()
{
    //there was a stange artifact showing up in the console when the game ran that I cant capture a screenshot of
    // identified it as the cursor and solved the problem by just coloring it black
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(console, &cursorInfo);

    // Clear the previous menu from the console buffer.
    CONSOLE_SCREEN_BUFFER_INFO screenInfo;
    GetConsoleScreenBufferInfo(console, &screenInfo);

    DWORD consoleSize = screenInfo.dwSize.X * screenInfo.dwSize.Y;
    DWORD charactersWritten;

    COORD homePosition = { 0, 0 };

    FillConsoleOutputCharacterA(console, ' ', consoleSize, homePosition, &charactersWritten);

    FillConsoleOutputAttribute(console, screenInfo.wAttributes, consoleSize, homePosition, &charactersWritten);

    SetConsoleCursorPosition(console, homePosition);
}

void GameController::WaitForNextFrame()
{
    //This prevents the game from running as fast as the processor allows.
    std::this_thread::sleep_for(std::chrono::milliseconds(gameConfig["game"]["timing"]["frame_delay_ms"].get<int>()));
}

void GameController::EndConsole()
{
    // Throw away the keys typed during play so they do not end up in the menu prompt.
    ClearKeys();

    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    // Clear the console and force a black background.
    Colors::SetBackground(Colors::Color::Black);

    CONSOLE_SCREEN_BUFFER_INFO screenInfo;
    GetConsoleScreenBufferInfo(console, &screenInfo);

    DWORD consoleSize = screenInfo.dwSize.X * screenInfo.dwSize.Y;
    DWORD charactersWritten = 0;

    COORD homePosition = { 0, 0 };

    FillConsoleOutputCharacterA(
        console, ' ', consoleSize, homePosition, &charactersWritten
    );

    FillConsoleOutputAttribute(
        console, screenInfo.wAttributes, consoleSize,
        homePosition, &charactersWritten
    );

    SetConsoleCursorPosition(console, homePosition);

    // Restore cursor visibility.
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = true;
    SetConsoleCursorInfo(console, &cursorInfo);
}

//pause menu
void GameController::RunPauseMenu()
{
    const int exitKey = gameConfig["game"]["controls"]["exit"].get<int>();

    // Wait for the ESC that opened the menu to come back up so it does not close the menu too.
    WaitForKeyRelease(exitKey);
    ClearKeys();

    paused = true;
    pauseStatus = PAUSED_TEXT;

    while (paused)
    {
        // The frame is drawn again with the pause menu on the line under the field.
        RenderGame();

        char key = InputHelper::GetKey();

        // An arrow or function key, throw away its second half.
        if (key == EXTENDED_KEY || key == EXTENDED_KEY_ALT)
        {
            InputHelper::GetKey();
            continue;
        }

        if (key == PAUSE_RESUME || key == exitKey)
        {
            paused = false;
        }
        else if (key == PAUSE_SAVE)
        {
            pauseStatus = SaveGame() ? SAVED_TEXT : SAVE_FAILED_TEXT;
        }
        else if (key == PAUSE_LOAD)
        {
            // The game stays paused on the loaded save so the player can get ready.
            if (!HasSave())
            {
                pauseStatus = NO_SAVE_TEXT;
            }
            else
            {
                pauseStatus = LoadGame() ? LOADED_TEXT : LOAD_FAILED_TEXT;
            }
        }
        else if (key == PAUSE_QUIT)
        {
            paused = false;
            running = false;
        }
    }

    // ESC can close the menu as well, wait for it to come back up so it does not open the menu again.
    WaitForKeyRelease(exitKey);
    ClearKeys();
}

void GameController::WaitForKeyRelease(int key)
{
    while (InputHelper::IsKeyDown(key))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(KEY_RELEASE_CHECK_MS));
    }
}


//sound
void GameController::TakeSoundSnapshot()
{
    soundSnapshot.clear();

    // A game without a sounds list plays silent.
    if (!gameConfig["game"].contains("sounds"))
    {
        return;
    }

    for (const auto& sound : gameConfig["game"]["sounds"])
    {
        if (!IsSoundEntry(sound))
        {
            continue;
        }

        std::string key = sound["key"].get<std::string>();
        double value = 0.0;

        // Read straight from the GameState so a change in the decimals still counts.
        if (runtime.GetState().GetValue(key, value))
        {
            soundSnapshot[key] = value;
        }
    }
}

void GameController::PlayStateSounds()
{
    if (!gameConfig["game"].contains("sounds"))
    {
        return;
    }

    bool played = false;

    for (const auto& sound : gameConfig["game"]["sounds"])
    {
        if (!IsSoundEntry(sound))
        {
            continue;
        }

        std::string key = sound["key"].get<std::string>();
        double value = 0.0;

        // A value the game does not have yet has nothing to watch.
        if (!runtime.GetState().GetValue(key, value))
        {
            continue;
        }

        // A value that shows up for the first time counts as a change too, a rule can create one with Assign.
        auto previous = soundSnapshot.find(key);
        bool changed = previous == soundSnapshot.end() || previous->second != value;

        soundSnapshot[key] = value;

        // PlaySound can only play one sound at a time, so when several change at once the first one listed wins.
        if (changed && !played)
        {
            PlaySoundFile(sound["file"].get<std::string>());
            played = true;
        }
    }
}

void GameController::PlaySoundFile(const std::string& file)
{
    // A sound file that is not there is skipped, the game plays fine without it.
    if (!fileBoss.FileExists(file))
    {
        return;
    }

    // SND_ASYNC keeps the game running while it plays, a new sound cuts off the one before it.
    PlaySoundA(file.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}


//saving
bool GameController::SaveGame()
{
    nlohmann::json saveData = ReadSaveFile();
    nlohmann::json save;

    save["state"] = nlohmann::json::object();

    // initial_state is the list of every value the game keeps, so it is the list of what gets saved.
    for (const auto& value : gameConfig["game"]["initial_state"].items())
    {
        double stateValue = 0.0;

        // Read straight from the GameState, WonkleRuntime's double GetStateValue cuts off decimals.
        if (runtime.GetState().GetValue(value.key(), stateValue))
        {
            // Whole numbers are written without a decimal point so the save reads like initial_state.
            if (stateValue == std::floor(stateValue))
            {
                save["state"][value.key()] = static_cast<long long>(stateValue);
            }
            else
            {
                save["state"][value.key()] = stateValue;
            }
        }
    }

    // Anything the game keeps outside the runtime.
    SaveExtras(save);

    // Saving again replaces the game's old save.
    saveData["saves"][gameName] = save;

    return WriteSaveFile(saveData);
}

bool GameController::LoadGame()
{
    nlohmann::json saveData = ReadSaveFile();

    if (!saveData["saves"].contains(gameName))
    {
        return false;
    }

    const nlohmann::json& save = saveData["saves"][gameName];

    try
    {
        // Read every value first so a broken save leaves the game the way it was.
        std::map<std::string, double> values;

        for (const auto& value : save.at("state").items())
        {
            values[value.key()] = value.value().get<double>();
        }

        // Start the game over, then put the saved values on top.
        InitializeGame();

        for (const auto& value : values)
        {
            runtime.SetStateValue(value.first, value.second);
        }

        LoadExtras(save);

        // Loading moves every value at once, that should not set off every sound.
        TakeSoundSnapshot();
    }
    catch (const nlohmann::json::exception&)
    {
        return false;
    }

    return true;
}

bool GameController::HasSave() const
{
    return ReadSaveFile()["saves"].contains(gameName);
}

int GameController::GetHighScore() const
{
    return highScore;
}

bool GameController::ResetAllSaveData()
{
    nlohmann::json saveData;

    saveData["high_scores"] = nlohmann::json::object();
    saveData["saves"] = nlohmann::json::object();

    return WriteSaveFile(saveData);
}

nlohmann::json GameController::ReadSaveFile()
{
    FileBoss files;
    nlohmann::json saveData;

    // No save file yet, or one that will not read, starts out empty.
    if (!files.ReadJSON(SAVE_FILE, saveData) || !saveData.is_object())
    {
        saveData = nlohmann::json::object();
    }

    if (!saveData.contains("high_scores") || !saveData["high_scores"].is_object())
    {
        saveData["high_scores"] = nlohmann::json::object();
    }

    if (!saveData.contains("saves") || !saveData["saves"].is_object())
    {
        saveData["saves"] = nlohmann::json::object();
    }

    return saveData;
}

bool GameController::WriteSaveFile(const nlohmann::json& saveData)
{
    FileBoss files;

    return files.WriteFile(SAVE_FILE, saveData.dump(2));
}

bool GameController::HasHighScore() const
{
    return gameConfig.at("game").at("initial_state").contains("score");
}

void GameController::RecordHighScore()
{
    if (!HasHighScore())
    {
        return;
    }

    int score = 0;
    runtime.GetStateValue("score", score);

    if (score <= highScore)
    {
        return;
    }

    highScore = score;

    nlohmann::json saveData = ReadSaveFile();
    saveData["high_scores"][gameName] = highScore;
    WriteSaveFile(saveData);
}

//field
Boundary GameController::GetBoundary()
{
    const auto& field = gameConfig["game"]["field"];

    return Boundary(
        field["width"].get<int>(),
        field["height"].get<int>(),
        field["left_edge"].get<int>(),
        field["top"].get<int>(),
        field["right_edge"].get<int>(),
        field["bottom"].get<int>()
    );
}


//render
void GameController::BeginFrame()
{
    const auto& field = gameConfig["game"]["field"];
    const auto& rendering = gameConfig["game"]["rendering"];

    render.SetBackground();

    // Begin a new frame.
    render.BeginFrame(field["width"].get<int>(),
        field["height"].get<int>() + rendering["extra_height"].get<int>(),
        rendering["roles"]["background"].get<std::string>(),
        rendering["world_vertical_offset"].get<int>());
    // Start the frame's objects over with the boundary.
    objects.clear();

    Boundary boundary = GetBoundary();

    boundary.Draw(render, objects,
        rendering["ascii"]["top_border"].get<std::string>()[0],
        rendering["ascii"]["bottom_border"].get<std::string>()[0],
        rendering["ascii"]["vertical_border"].get<std::string>()[0],
        rendering["roles"]["border"].get<std::string>()
    );
}

void GameController::DrawScoreboard(const std::string& text)
{
    const auto& rendering = gameConfig["game"]["rendering"];

    render.DrawText(rendering["scoreboard_column"].get<int>(),
        rendering["scoreboard_row"].get<int>(),
        text,
        rendering["roles"]["ui"].get<std::string>());
}

void GameController::AddRectangle(const std::string& name, int x, int y, int width, int height)
{
    const auto& rendering = gameConfig["game"]["rendering"];

    // The name picks the character and the color role out of the json, "paddle" or "ball" for example.
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            objects.push_back({
                Position(x + i, y + j),
                rendering["ascii"][name].get<std::string>()[0],
                rendering["roles"][name].get<std::string>()
                });
        }
    }
}

void GameController::AddText(int x, int y, const std::string& text, const std::string& role)
{
    const auto& rendering = gameConfig["game"]["rendering"];

    // Text goes in with the frame's objects one character at a time, so it is drawn in the order it was added.
    for (int i = 0; i < text.size(); i++)
    {
        objects.push_back({
            Position(x + i, y),
            text[i],
            rendering["roles"][role].get<std::string>()
            });
    }
}

void GameController::EndFrame()
{
    const auto& rendering = gameConfig["game"]["rendering"];

    // Submit objects to the renderer.
    render.DrawObjects(objects);

    // Draw controls, or the result once the game has ended.
    int gameWon = 0;
    int gameLost = 0;
    int score = 0;

    runtime.GetStateValue("game_won", gameWon);
    runtime.GetStateValue("game_lost", gameLost);
    runtime.GetStateValue("score", score);

    std::string bottomText = gameConfig["game"]["controls"]["display_text"].get<std::string>();


    if (gameWon != 0 || gameLost != 0)
    {
        // A game that keeps one score can show it with the result.
        if (rendering["messages"].contains("final_score"))
        {
            bottomText += rendering["messages"]["final_score"].get<std::string>() + std::to_string(score);
        }

        bottomText += rendering["messages"]["restart_prompt"].get<std::string>();
    }

    // While paused the pause menu takes the line instead.
    if (paused)
    {
        bottomText = pauseStatus + PAUSE_OPTIONS;
    }

    render.DrawText(rendering["controls_column"].get<int>(),
        gameConfig["game"]["field"]["height"].get<int>() + rendering["controls_row_offset"].get<int>(),
        bottomText,
        rendering["roles"]["ui"].get<std::string>());

    if (gameWon != 0 || gameLost != 0)
    {
        std::string result = rendering["messages"][gameWon != 0 ? "player_wins" : "player_loses"].get<std::string>();
        int x = (gameConfig["game"]["field"]["height"].get<int>() - static_cast<int>(result.size())) / 2;
        int y = rendering["world_vertical_offset"].get<int>() + gameConfig["game"]["field"]["height"].get<int>() / 2;
        render.DrawText(x, y, result, rendering["roles"]["ui"].get<std::string>());
    }
    // Present completed frame.
    render.PresentFrame();
}


//gameover stuff
void GameController::CheckGameOver()
{
    int gameWon = 0;
    int gameLost = 0;

    // The rules decide when the game is won or lost.
    runtime.GetStateValue("game_won", gameWon);
    runtime.GetStateValue("game_lost", gameLost);

    if (gameWon == 0 && gameLost == 0)
    {
        return;
    }

    // Keep the score if it is the best one this game has had.
    RecordHighScore();

    // RenderGame has already drawn the result and the prompt.
    if (AskToRestart())
    {
        // Restart, the rules stay the same so only the state is reset.
        InitializeGame();
    }
    else
    {
        running = false;
    }
}

void GameController::ClearKeys()
{
    while (InputHelper::IsKeyPressed())
    {
        InputHelper::GetKey();
    }
}

bool GameController::AskToRestart()
{
    const auto& controls = gameConfig["game"]["controls"];

    // Throw away the movement keys typed during play so they do not answer the prompt.
    ClearKeys();

    // Only the restart and exit keys answer the prompt, a movement key still held down
    // when the game ended is ignored.
    while (true)
    {
        char key = InputHelper::GetKey();

        if (std::toupper(static_cast<unsigned char>(key)) == controls["restart"].get<int>())
        {
            return true;
        }

        if (key == controls["exit"].get<int>())
        {
            return false;
        }
    }
}
