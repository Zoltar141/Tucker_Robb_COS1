#include "GameController.h"
#include "InputHelper.h"
#include "Colors.h"
#include <thread>
#include <chrono>
#include <Windows.h>
#include <stdexcept>
#include <cctype>

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

std::map<std::string, int> GameController::LoadGameState()
{
    std::map<std::string, int> state;

    // Retrieve every value the game listed in initial_state.
    for (const auto& value : gameConfig["game"]["initial_state"].items())
    {
        runtime.GetStateValue(value.key(), state[value.key()]);
    }

    return state;
}


//Loop
void GameController::Run()//controls the game loop
{
    InitializeGame();
    SetupRules();

    StartConsole();

    running = true;

    while (running)
    {
        HandleInput();
        UpdateGame();
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
}

void GameController::DrawScoreboard(const std::string& text)
{
    const auto& rendering = gameConfig["game"]["rendering"];

    render.DrawText(rendering["scoreboard_column"].get<int>(),
        rendering["scoreboard_row"].get<int>(),
        text,
        rendering["roles"]["ui"].get<std::string>());
}

void GameController::DrawBoundary(std::vector<RenderObject>& objects)
{
    const auto& rendering = gameConfig["game"]["rendering"];
    const auto& ascii = rendering["ascii"];

    Boundary boundary = GetBoundary();

    boundary.Draw(render, objects,
        ascii["top_border"].get<std::string>()[0],
        ascii["bottom_border"].get<std::string>()[0],
        ascii["vertical_border"].get<std::string>()[0],
        rendering["roles"]["border"].get<std::string>()
    );
}

void GameController::DrawBottomText(const std::string& text)
{
    const auto& rendering = gameConfig["game"]["rendering"];

    render.DrawText(rendering["controls_column"].get<int>(),
        gameConfig["game"]["field"]["height"].get<int>() + rendering["controls_row_offset"].get<int>(),
        text,
        rendering["roles"]["ui"].get<std::string>());
}


//gameover stuff
std::string GameController::GetGameOverText(bool gameWon, int score)
{
    const auto& messages = gameConfig["game"]["rendering"]["messages"];

    std::string text;

    if (gameWon)
    {
        text = messages["player_wins"].get<std::string>();
    }
    else
    {
        text = messages["player_loses"].get<std::string>();
    }

    text += messages["final_score"].get<std::string>() + std::to_string(score) +
        messages["restart_prompt"].get<std::string>();

    return text;
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
    GameController::ClearKeys();

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
