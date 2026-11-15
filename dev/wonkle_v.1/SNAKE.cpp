#include "SNAKE.h"
#include "InputHelper.h"
#include "ThemeManager.h"
#include "Colors.h"
#include "Boundary.h"
#include "Collision.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>
#include <vector>
#include <stdexcept>
#include <string>
#include <cctype>

constexpr int OUTSIDE_GRID = -1;  //food position while there is no food on the field, the eats_food rule sets it
constexpr int NO_TICKS = 0;       //a grid space the snake is not on

struct SnakeState
{
    int headX;
    int headY;
    int directionX;
    int directionY;
    int requestedDirectionX;
    int requestedDirectionY;
    int snakeLength;
    int score;
    int foodX;
    int foodY;
    int gameWon;
    int gameLost;
};


SNAKE::SNAKE(ThemeManager& themeManager)//constructor, connects SNAKE to ThemeManager
    : themeManager(themeManager), render(themeManager), randomGenerator(std::random_device{}())
{
    if (!fileBoss.ReadJSON("snake.json", snakeConfig))
    {
        throw std::runtime_error("Failed to load snake.json");
    }
    render.SetConfiguration(snakeConfig["game"]["rendering"]);
}

void SNAKE::Run()//controls the game loop
{
    InitializeGame();
    SetupRules();

    //same console setup as PONG, hides the cursor so it does not show up as an artifact
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


    running = true;

    while (running)
    {
        HandleInput();
        UpdateGame();
        RenderGame();
        CheckGameOver();
        //This prevents the game from running as fast as the processor allows, input is polled once per frame.
        std::this_thread::sleep_for(std::chrono::milliseconds(snakeConfig["game"]["timing"]["frame_delay_ms"].get<int>()));
    }

    // Throw away the keys typed during play so they do not end up in the menu prompt.
    while (InputHelper::IsKeyPressed())
    {
        InputHelper::GetKey();
    }

    // Clear the console and force a black background.
    Colors::SetBackground(Colors::Color::Black);
    GetConsoleScreenBufferInfo(console, &screenInfo);

    consoleSize = screenInfo.dwSize.X * screenInfo.dwSize.Y;
    charactersWritten = 0;

    FillConsoleOutputCharacterA(
        console, ' ', consoleSize, homePosition, &charactersWritten
    );

    FillConsoleOutputAttribute(
        console, screenInfo.wAttributes, consoleSize,
        homePosition, &charactersWritten
    );

    SetConsoleCursorPosition(console, homePosition);


    // Restore cursor visibility.
    cursorInfo.bVisible = true;
    SetConsoleCursorInfo(console, &cursorInfo);
}

void SNAKE::InitializeGame()
{
    const auto& initialState = snakeConfig["game"]["initial_state"];
    const auto& grid = snakeConfig["game"]["grid"];

    int headX = initialState["head"]["x"].get<int>();
    int headY = initialState["head"]["y"].get<int>();
    int directionX = initialState["direction"]["x"].get<int>();
    int directionY = initialState["direction"]["y"].get<int>();
    int startingLength = snakeConfig["game"]["snake"]["starting_length"].get<int>();

    runtime.SetStateValue("head_x", headX);
    runtime.SetStateValue("head_y", headY);

    runtime.SetStateValue("direction_x", directionX);
    runtime.SetStateValue("direction_y", directionY);

    runtime.SetStateValue("requested_direction_x", directionX);
    runtime.SetStateValue("requested_direction_y", directionY);

    runtime.SetStateValue("snake_length", startingLength);

    runtime.SetStateValue("score", initialState["score"].get<int>());
    runtime.SetStateValue("winning_score", snakeConfig["game"]["scoring"]["winning_score"].get<int>());

    // There is no food yet, it is placed on the first move.
    runtime.SetStateValue("food_x", OUTSIDE_GRID);
    runtime.SetStateValue("food_y", OUTSIDE_GRID);

    runtime.SetStateValue("body_under_head", NO_TICKS);

    runtime.SetStateValue("game_won", initialState["game_won"].get<int>());
    runtime.SetStateValue("game_lost", initialState["game_lost"].get<int>());

    // The body is a tick counter on every grid space, a space is part of the snake while its counter is above zero.
    bodyTicks.assign(grid["rows"].get<int>(), std::vector<int>(grid["columns"].get<int>(), NO_TICKS));

    // The starting body lines up behind the head, counting down toward the tail.
    for (int i = 1; i < startingLength; i++)
    {
        bodyTicks[headY - i * directionY][headX - i * directionX] = startingLength - i;
    }

    // Movement timing starts over.
    elapsedMilliseconds = 0;
    lastFrameTime = std::chrono::steady_clock::now();
}


void SNAKE::SetupRules()
{
    runtime.LoadRules(snakeConfig["game"]["rules"]);
}

void SNAKE::HandleInput()// Processes player keyboard input.
{
    // Input only records the direction the player wants, the snake turns on its next move.
    if (InputHelper::IsKeyDown(snakeConfig["game"]["controls"]["up"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", 0);
        runtime.SetStateValue("requested_direction_y", -1);
    }
    else if (InputHelper::IsKeyDown(snakeConfig["game"]["controls"]["down"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", 0);
        runtime.SetStateValue("requested_direction_y", 1);
    }
    else if (InputHelper::IsKeyDown(snakeConfig["game"]["controls"]["left"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", -1);
        runtime.SetStateValue("requested_direction_y", 0);
    }
    else if (InputHelper::IsKeyDown(snakeConfig["game"]["controls"]["right"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", 1);
        runtime.SetStateValue("requested_direction_y", 0);
    }

    // Exit the current game.
    if (InputHelper::IsKeyDown(snakeConfig["game"]["controls"]["exit"].get<int>()))
    {
        running = false;
    }
}

static SnakeState LoadGameState(WonkleRuntime& runtime)
{
    SnakeState state{};

    runtime.GetStateValue("head_x", state.headX);
    runtime.GetStateValue("head_y", state.headY);
    runtime.GetStateValue("direction_x", state.directionX);
    runtime.GetStateValue("direction_y", state.directionY);
    runtime.GetStateValue("requested_direction_x", state.requestedDirectionX);
    runtime.GetStateValue("requested_direction_y", state.requestedDirectionY);
    runtime.GetStateValue("snake_length", state.snakeLength);
    runtime.GetStateValue("score", state.score);
    runtime.GetStateValue("food_x", state.foodX);
    runtime.GetStateValue("food_y", state.foodY);
    runtime.GetStateValue("game_won", state.gameWon);
    runtime.GetStateValue("game_lost", state.gameLost);

    return state;
}


void SNAKE::UpdateGame()// Advances the game state.
{
    // Add the time that has passed since the last frame.
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

    elapsedMilliseconds += std::chrono::duration<double, std::milli>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;

    double movementInterval = snakeConfig["game"]["timing"]["movement_interval_ms"].get<double>();

    // The snake only moves once enough time has built up.
    if (elapsedMilliseconds < movementInterval)
    {
        return;
    }

    elapsedMilliseconds -= movementInterval;

    // After a long stall do not let the snake make several moves in a row to catch up.
    if (elapsedMilliseconds >= movementInterval)
    {
        elapsedMilliseconds = 0;
    }

    MoveSnake();
}


void SNAKE::MoveSnake()// Makes one move.
{
    // Retrieve current game state.
    SnakeState state = LoadGameState(runtime);

    const auto& grid = snakeConfig["game"]["grid"];

    // Turn, unless the player asked to go straight back the way the snake came.
    bool reversing = state.requestedDirectionX == -state.directionX && state.requestedDirectionY == -state.directionY;

    if (!reversing)
    {
        state.directionX = state.requestedDirectionX;
        state.directionY = state.requestedDirectionY;
    }

    // Every space the snake is on counts down, when a space reaches zero the tail has left it.
    for (int row = 0; row < bodyTicks.size(); row++)
    {
        for (int column = 0; column < bodyTicks[row].size(); column++)
        {
            if (bodyTicks[row][column] > NO_TICKS)
            {
                bodyTicks[row][column]--;
            }
        }
    }

    // The head leaves a counter behind in the space it is leaving, it lasts until the tail passes.
    bodyTicks[state.headY][state.headX] = state.snakeLength - 1;

    // Move the head.
    state.headX += state.directionX;
    state.headY += state.directionY;

    // Find the counter under the head, anything above zero is the snake's own body.
    int bodyUnderHead = NO_TICKS;

    bool insideGrid = Collision::CheckRectangle(
        Position(state.headX, state.headY),
        Position(0, 0),
        grid["columns"].get<int>(),
        grid["rows"].get<int>()
    );

    if (insideGrid)
    {
        bodyUnderHead = bodyTicks[state.headY][state.headX];
    }

    // Store updated game values.
    runtime.SetStateValue("head_x", state.headX);
    runtime.SetStateValue("head_y", state.headY);
    runtime.SetStateValue("direction_x", state.directionX);
    runtime.SetStateValue("direction_y", state.directionY);
    runtime.SetStateValue("body_under_head", bodyUnderHead);

    // Evaluate registered gameplay rules.
    runtime.Update();

    // If the rules made the snake longer, every counter lasts one more move.
    int newLength = state.snakeLength;

    runtime.GetStateValue("snake_length", newLength);

    if (newLength > state.snakeLength)
    {
        for (int row = 0; row < bodyTicks.size(); row++)
        {
            for (int column = 0; column < bodyTicks[row].size(); column++)
            {
                if (bodyTicks[row][column] > NO_TICKS)
                {
                    bodyTicks[row][column]++;
                }
            }
        }
    }

    // With no food on the field, try one random space per move and use it if the snake is not on it.
    runtime.GetStateValue("food_x", state.foodX);

    if (state.foodX == OUTSIDE_GRID)
    {
        std::uniform_int_distribution<int> randomColumn(0, grid["columns"].get<int>() - 1);
        std::uniform_int_distribution<int> randomRow(0, grid["rows"].get<int>() - 1);

        int candidateX = randomColumn(randomGenerator);
        int candidateY = randomRow(randomGenerator);

        bool onHead = Collision::CheckPosition(Position(candidateX, candidateY), Position(state.headX, state.headY));

        if (!onHead && bodyTicks[candidateY][candidateX] == NO_TICKS)
        {
            runtime.SetStateValue("food_x", candidateX);
            runtime.SetStateValue("food_y", candidateY);
        }
    }
}


// Draws one grid space, a space is more than one character wide so it looks square.
void SNAKE::DrawCell(int cellX, int cellY, char character, const std::string& role)
{
    const auto& grid = snakeConfig["game"]["grid"];

    // Spaces outside the grid are not drawn, this keeps the head off the border when it hits a wall.
    if (!Collision::CheckRectangle(Position(cellX, cellY), Position(0, 0), grid["columns"].get<int>(), grid["rows"].get<int>()))
    {
        return;
    }

    for (int i = 0; i < grid["cell_width"].get<int>(); i++)
    {
        render.DrawCharacter(grid["origin_x"].get<int>() + cellX * grid["cell_width"].get<int>() + i,
            cellY + snakeConfig["game"]["rendering"]["world_vertical_offset"].get<int>(),
            character,
            role);
    }
}


void SNAKE::RenderGame()
{
    // Retrieve current game state.
    SnakeState state = LoadGameState(runtime);

    render.SetBackground();

    // Define game objects.
    std::vector<RenderObject> objects;

    // Begin a new frame.
    render.BeginFrame(snakeConfig["game"]["field"]["width"].get<int>(),
        snakeConfig["game"]["field"]["height"].get<int>() + snakeConfig["game"]["rendering"]["extra_height"].get<int>(),
        snakeConfig["game"]["rendering"]["roles"]["background"].get<std::string>(),
        snakeConfig["game"]["rendering"]["world_vertical_offset"].get<int>());

    // Draw scoreboard.
    render.DrawText(snakeConfig["game"]["rendering"]["scoreboard_column"].get<int>(),
        snakeConfig["game"]["rendering"]["scoreboard_row"].get<int>(),
        snakeConfig["game"]["rendering"]["scoreboard"]["score_label"].get<std::string>() + " " + std::to_string(state.score) +
        snakeConfig["game"]["rendering"]["scoreboard"]["separator"].get<std::string>() +
        snakeConfig["game"]["rendering"]["scoreboard"]["length_label"].get<std::string>() + " " + std::to_string(state.snakeLength),
        snakeConfig["game"]["rendering"]["roles"]["ui"].get<std::string>());

    Boundary boundary(
        snakeConfig["game"]["field"]["width"].get<int>(),
        snakeConfig["game"]["field"]["height"].get<int>(),
        snakeConfig["game"]["field"]["left_edge"].get<int>(),
        snakeConfig["game"]["field"]["top"].get<int>(),
        snakeConfig["game"]["field"]["right_edge"].get<int>(),
        snakeConfig["game"]["field"]["bottom"].get<int>());

    const auto& ascii = snakeConfig["game"]["rendering"]["ascii"];
    const auto& roles = snakeConfig["game"]["rendering"]["roles"];

    boundary.Draw(render, objects,
        ascii["top_border"].get<std::string>()[0],
        ascii["bottom_border"].get<std::string>()[0],
        ascii["vertical_border"].get<std::string>()[0],
        roles["border"].get<std::string>()
    );

    // Submit objects to the renderer.
    render.DrawObjects(objects);

    // Food.
    DrawCell(state.foodX, state.foodY,
        static_cast<char>(ascii["food"].get<int>()),
        roles["food"].get<std::string>());

    // Snake body, every space whose counter has not run out.
    for (int row = 0; row < bodyTicks.size(); row++)
    {
        for (int column = 0; column < bodyTicks[row].size(); column++)
        {
            if (bodyTicks[row][column] > NO_TICKS)
            {
                DrawCell(column, row,
                    static_cast<char>(ascii["snake_body"].get<int>()),
                    roles["snake_body"].get<std::string>());
            }
        }
    }

    // Snake head, drawn last so it is always on top.
    DrawCell(state.headX, state.headY,
        static_cast<char>(ascii["snake_head"].get<int>()),
        roles["snake_head"].get<std::string>());

    // Draw controls, or the result once the game has ended.
    std::string bottomText = snakeConfig["game"]["controls"]["display_text"].get<std::string>();

    if (state.gameWon != 0 || state.gameLost != 0)
    {
        const auto& messages = snakeConfig["game"]["rendering"]["messages"];

        if (state.gameWon != 0)
        {
            bottomText = messages["player_wins"].get<std::string>();
        }
        else
        {
            bottomText = messages["player_loses"].get<std::string>();
        }

        bottomText += messages["final_score"].get<std::string>() + std::to_string(state.score) +
            messages["restart_prompt"].get<std::string>();
    }

    render.DrawText(snakeConfig["game"]["rendering"]["controls_column"].get<int>(),
        snakeConfig["game"]["field"]["height"].get<int>() + snakeConfig["game"]["rendering"]["controls_row_offset"].get<int>(),
        bottomText,
        roles["ui"].get<std::string>());

    // Present completed frame.
    render.PresentFrame();
}


void SNAKE::CheckGameOver()//checks whether the game has ended
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

    // Throw away the movement keys typed during play so they do not answer the prompt.
    while (InputHelper::IsKeyPressed())
    {
        InputHelper::GetKey();
    }

    // RenderGame has already drawn the result and the prompt. Only the restart and exit keys
    // answer it, a movement key still held down when the game ended is ignored.
    bool answered = false;

    while (!answered)
    {
        char key = InputHelper::GetKey();

        if (std::toupper(static_cast<unsigned char>(key)) == snakeConfig["game"]["controls"]["restart"].get<int>())
        {
            // Restart, the rules stay the same so only the state is reset.
            InitializeGame();
            answered = true;
        }
        else if (key == snakeConfig["game"]["controls"]["exit"].get<int>())
        {
            running = false;
            answered = true;
        }
    }
}