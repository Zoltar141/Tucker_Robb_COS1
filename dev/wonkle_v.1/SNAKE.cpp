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

SNAKE::SNAKE(ThemeManager& themeManager)//constructor, connects SNAKE to ThemeManager
    : GameController(themeManager, "snake.json"), randomGenerator(std::random_device{}())
{
}

void SNAKE::InitializeGame()
{
    // Every starting value comes from initial_state in snake.json.
    SetInitialState();

    std::map<std::string, int> state = LoadGameState();

    const auto& grid = gameConfig["game"]["grid"];

    // The body is a tick counter on every grid space, a space is part of the snake while its counter is above zero.
    bodyTicks.assign(grid["rows"].get<int>(), std::vector<int>(grid["columns"].get<int>(), NO_TICKS));

    // The starting body lines up behind the head, counting down toward the tail.
    for (int i = 1; i < state["snake_length"]; i++)
    {
        bodyTicks[state["head_y"] - i * state["direction_y"]][state["head_x"] - i * state["direction_x"]] = state["snake_length"] - i;
    }

    // Movement timing starts over.
    elapsedMilliseconds = 0;
    lastFrameTime = std::chrono::steady_clock::now();
}

void SNAKE::HandleInput()// Processes player keyboard input.
{
    // Input only records the direction the player wants, the snake turns on its next move.
    if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["up"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", 0);
        runtime.SetStateValue("requested_direction_y", -1);
    }
    else if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["down"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", 0);
        runtime.SetStateValue("requested_direction_y", 1);
    }
    else if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["left"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", -1);
        runtime.SetStateValue("requested_direction_y", 0);
    }
    else if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["right"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", 1);
        runtime.SetStateValue("requested_direction_y", 0);
    }

    // Exit the current game.
    if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["exit"].get<int>()))
    {
        running = false;
    }
}

void SNAKE::UpdateGame()// Advances the game state.
{
    // Add the time that has passed since the last frame.
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

    elapsedMilliseconds += std::chrono::duration<double, std::milli>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;

    double movementInterval = gameConfig["game"]["timing"]["movement_interval_ms"].get<double>();

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
    std::map<std::string, int> state = LoadGameState();

    const auto& grid = gameConfig["game"]["grid"];

    // Turn, unless the player asked to go straight back the way the snake came.
    bool reversing = state["requested_direction_x"] == -state["direction_x"] && state["requested_direction_y"] == -state["direction_y"];

    if (!reversing)
    {
        state["direction_x"] = state["requested_direction_x"];
        state["direction_y"] = state["requested_direction_y"];
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
    bodyTicks[state["head_y"]][state["head_x"]] = state["snake_length"] - 1;

    // Move the head.
    state["head_x"] += state["direction_x"];
    state["head_y"] += state["direction_y"];

    // Find the counter under the head, anything above zero is the snake's own body.
    int bodyUnderHead = NO_TICKS;

    bool insideGrid = Collision::CheckRectangle(
        Position(state["head_x"], state["head_y"]),
        Position(0, 0),
        grid["columns"].get<int>(),
        grid["rows"].get<int>()
    );

    if (insideGrid)
    {
        bodyUnderHead = bodyTicks[state["head_y"]][state["head_x"]];
    }

    // Store updated game values.
    runtime.SetStateValue("head_x", state["head_x"]);
    runtime.SetStateValue("head_y", state["head_y"]);
    runtime.SetStateValue("direction_x", state["direction_x"]);
    runtime.SetStateValue("direction_y", state["direction_y"]);
    runtime.SetStateValue("body_under_head", bodyUnderHead);

    // Evaluate registered gameplay rules.
    runtime.Update();

    // If the rules made the snake longer, every counter lasts one more move.
    int newLength = state["snake_length"];

    runtime.GetStateValue("snake_length", newLength);

    if (newLength > state["snake_length"])
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
    runtime.GetStateValue("food_x", state["food_x"]);

    if (state["food_x"] == OUTSIDE_GRID)
    {
        std::uniform_int_distribution<int> randomColumn(0, grid["columns"].get<int>() - 1);
        std::uniform_int_distribution<int> randomRow(0, grid["rows"].get<int>() - 1);

        int candidateX = randomColumn(randomGenerator);
        int candidateY = randomRow(randomGenerator);

        bool onHead = Collision::CheckPosition(Position(candidateX, candidateY), Position(state["head_x"], state["head_y"]));

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
    const auto& grid = gameConfig["game"]["grid"];

    // Spaces outside the grid are not drawn, this keeps the head off the border when it hits a wall.
    if (!Collision::CheckRectangle(Position(cellX, cellY), Position(0, 0), grid["columns"].get<int>(), grid["rows"].get<int>()))
    {
        return;
    }

    for (int i = 0; i < grid["cell_width"].get<int>(); i++)
    {
        render.DrawCharacter(grid["origin_x"].get<int>() + cellX * grid["cell_width"].get<int>() + i,
            cellY + gameConfig["game"]["rendering"]["world_vertical_offset"].get<int>(),
            character,
            role);
    }
}


void SNAKE::RenderGame()
{
    // Retrieve current game state.
    std::map<std::string, int> state = LoadGameState();

    // Define game objects.
    std::vector<RenderObject> objects;

    // Begin a new frame.
    BeginFrame();
    // Draw scoreboard.
    DrawScoreboard(
        gameConfig["game"]["rendering"]["scoreboard"]["score_label"].get<std::string>() + " " + std::to_string(state["score"]) +
        gameConfig["game"]["rendering"]["scoreboard"]["separator"].get<std::string>() +
        gameConfig["game"]["rendering"]["scoreboard"]["length_label"].get<std::string>() + " " + std::to_string(state["snake_length"])
    );

    const auto& ascii = gameConfig["game"]["rendering"]["ascii"];
    const auto& roles = gameConfig["game"]["rendering"]["roles"];

    DrawBoundary(objects);

    // Submit objects to the renderer.
    render.DrawObjects(objects);

    // Food.
    DrawCell(state["food_x"], state["food_y"],
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
    DrawCell(state["head_x"], state["head_y"],
        static_cast<char>(ascii["snake_head"].get<int>()),
        roles["snake_head"].get<std::string>());

    // Draw controls, or the result once the game has ended.
    std::string bottomText = gameConfig["game"]["controls"]["display_text"].get<std::string>();

    if (state["game_won"] != 0 || state["game_lost"] != 0)
    {
        bottomText = GetGameOverText(state["game_won"] != 0, state["score"]);
    }

    DrawBottomText(bottomText);

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