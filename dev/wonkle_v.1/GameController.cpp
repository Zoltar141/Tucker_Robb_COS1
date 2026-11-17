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

void GameController::SaveGameState(const std::map<std::string, int>& state)
{
    for (const auto& value : state)
    {
        runtime.SetStateValue(value.first, value.second);
    }
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
        // Exit the current game.
        if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["exit"].get<int>()))
        {
            running = false;
        }
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

    if (gameWon != 0)
    {
        bottomText = rendering["messages"]["player_wins"].get<std::string>();
    }
    else if (gameLost != 0)
    {
        bottomText = rendering["messages"]["player_loses"].get<std::string>();
    }

    if (gameWon != 0 || gameLost != 0)
    {
        // A game that keeps one score can show it with the result.
        if (rendering["messages"].contains("final_score"))
        {
            bottomText += rendering["messages"]["final_score"].get<std::string>() + std::to_string(score);
        }

        bottomText += rendering["messages"]["restart_prompt"].get<std::string>();
    }

    render.DrawText(rendering["controls_column"].get<int>(),
        gameConfig["game"]["field"]["height"].get<int>() + rendering["controls_row_offset"].get<int>(),
        bottomText,
        rendering["roles"]["ui"].get<std::string>());

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
