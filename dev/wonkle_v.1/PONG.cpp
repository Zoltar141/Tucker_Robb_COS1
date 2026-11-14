#include "PONG.h"
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
#include <filesystem>
#include <string>

const int FIELD_WIDTH = 80;
const int FIELD_HEIGHT = 25;
const int PADDLE_HEIGHT = 5;
const int WINNING_SCORE = 10;
constexpr std::chrono::milliseconds FRAME_DELAY{ 50 };
constexpr int OPPONENT_MISS_CHANCE = 4;
constexpr int PLAYFIELD_TOP = 1;
constexpr int PLAYFIELD_BOTTOM = FIELD_HEIGHT - 2;
constexpr int PADDLE_WIDTH = 4;
constexpr int INITIAL_PADDLE_Y =(FIELD_HEIGHT - PADDLE_HEIGHT) / 2;
constexpr int LEFT_SCORING_BOUNDARY = 1;
constexpr int RIGHT_SCORING_BOUNDARY = FIELD_WIDTH - 2;
constexpr int RENDER_EXTRA_HEIGHT = 4;
constexpr int CONTROLS_ROW_OFFSET = 3;
constexpr int WORLD_VERTICAL_OFFSET = 2;
constexpr int PADDLE_TOP_LIMIT = 0;
constexpr int PADDLE_BOTTOM_LIMIT = FIELD_HEIGHT - PADDLE_HEIGHT;
constexpr int FIELD_RIGHT_EDGE = FIELD_WIDTH - 1;
constexpr int PLAYER_PADDLE_X = 2;
constexpr int OPPONENT_PADDLE_X = FIELD_WIDTH - 3;

struct PongState
{
    int ballPosX;
    int ballPosY;
    int ballVelX;
    int ballVelY;
    int playerPaddleY;
    int opponentPaddleY;
    int playerScore;
    int opponentScore;
    int winningScore;
};


PONG::PONG(ThemeManager& themeManager)//constructor, connects PONG to ThemeManager
    : themeManager(themeManager), render(themeManager)
{
    if (!fileBoss.ReadJSON("pong.json", pongConfig))
    {
        throw std::runtime_error("Failed to load pong.json");
    }
    render.SetConfiguration(pongConfig["game"]["rendering"]);
}

void PONG::Run()//controls the game loop
{
    InitializeGame();
    SetupRules();

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

    FillConsoleOutputAttribute( console, screenInfo.wAttributes, consoleSize, homePosition, &charactersWritten);

    SetConsoleCursorPosition(console, homePosition);


    running = true;

    while (running)
    {
        HandleInput();
        UpdateGame();
        RenderGame();
        CheckGameOver();
        std::this_thread::sleep_for(FRAME_DELAY);//This prevents the game from running as fast as the processor allows.
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

void PONG::InitializeGame()
{
    const auto& initialState = pongConfig["game"]["initial_state"];

    runtime.SetStateValue("ball_position_x", initialState["ball"]["position"]["x"]);
    runtime.SetStateValue("ball_position_y", initialState["ball"]["position"]["y"]);

    runtime.SetStateValue("ball_velocity_x", initialState["ball"]["velocity"]["x"]);
    runtime.SetStateValue("ball_velocity_y", initialState["ball"]["velocity"]["y"]);

    runtime.SetStateValue("player_paddle_y", initialState["paddles"]["player_y"]);
    runtime.SetStateValue("opponent_paddle_y", initialState["paddles"]["opponent_y"]);

    runtime.SetStateValue("player_score", initialState["score"]["player"]);
    runtime.SetStateValue("opponent_score", initialState["score"]["opponent"]);

    runtime.SetStateValue("winning_score", pongConfig["game"]["scoring"]["winning_score"]);
}


void PONG::SetupRules()
{
    runtime.LoadRules(pongConfig["game"]["rules"]);
}

void PONG::HandleInput()// Processes player keyboard input.
{
    int playerPaddle= 0;

    runtime.GetStateValue("player_paddle_y", playerPaddle);//find position

    // Move paddle continuously while W or S is held.
    if (InputHelper::IsKeyDown('W'))
    {
        if (playerPaddle > PADDLE_TOP_LIMIT)
        {
            playerPaddle--;
        }
    }

    if (InputHelper::IsKeyDown('S'))
    {
        if (playerPaddle < PADDLE_BOTTOM_LIMIT)
        {
            playerPaddle++;
        }
    }

    // Exit the current game.
    if (InputHelper::IsKeyDown(VK_ESCAPE))
    {
        running = false;
    }

    runtime.SetStateValue("player_paddle_y", playerPaddle);
}

PongState LoadGameState(WonkleRuntime& runtime)
{
    PongState state{};

    runtime.GetStateValue("ball_position_x", state.ballPosX);
    runtime.GetStateValue("ball_position_y", state.ballPosY);
    runtime.GetStateValue("ball_velocity_x", state.ballVelX);
    runtime.GetStateValue("ball_velocity_y", state.ballVelY);
    runtime.GetStateValue("player_paddle_y", state.playerPaddleY);
    runtime.GetStateValue("opponent_paddle_y", state.opponentPaddleY);
    runtime.GetStateValue("player_score", state.playerScore);
    runtime.GetStateValue("opponent_score", state.opponentScore);
    runtime.GetStateValue("winning_score", state.winningScore);

    return state;
}


void PONG::UpdateGame()// Advances the game state.

{
   

    // Retrieve current game state.
    PongState state = LoadGameState(runtime);


    // Move the ball using its current velocity.
    state.ballPosX += state.ballVelX;
    state.ballPosY += state.ballVelY;

    // Check for boundary collisions.
    Boundary boundary(FIELD_WIDTH, FIELD_HEIGHT, 0, PLAYFIELD_TOP, FIELD_RIGHT_EDGE, PLAYFIELD_BOTTOM);

    BoundarySide collision = Collision::CheckBoundary(Position(state.ballPosX, state.ballPosY), boundary);

    if (collision == BoundarySide::Top || collision == BoundarySide::Bottom)
    {
        state.ballVelY *= -1;
    }

    // Player paddle collision.
    bool playerPaddleCollision = Collision::CheckRectangle(Position(state.ballPosX, state.ballPosY), Position(0, state.playerPaddleY), PADDLE_WIDTH, PADDLE_HEIGHT);

    if (state.ballVelX < 0 && playerPaddleCollision)
    {
        state.ballVelX = 1;

        // Calculate where the ball hits the paddle.
        int hitPosition = state.ballPosY - state.playerPaddleY;

        // Determine vertical ball direction.
        if (hitPosition < PADDLE_HEIGHT / 2)
        {
            state.ballVelY = -1;
        }
        else if (hitPosition > PADDLE_HEIGHT / 2)
        {
            state.ballVelY = 1;
        }
    }

    // Opponent paddle collision.
    bool opponentPaddleCollision = Collision::CheckRectangle(Position(state.ballPosX, state.ballPosY), Position(FIELD_WIDTH - PADDLE_WIDTH, state.opponentPaddleY), PADDLE_WIDTH, PADDLE_HEIGHT);

    if (state.ballVelX > 0 && opponentPaddleCollision)
    {
        state.ballVelX = -1;

        // Calculate where the ball hits the paddle.
        int hitPosition = state.ballPosY - state.opponentPaddleY;

        // Determine vertical ball direction.
        if (hitPosition < PADDLE_HEIGHT / 2)
        {
            state.ballVelY = -1;
        }
        else if (hitPosition > PADDLE_HEIGHT / 2)
        {
            state.ballVelY = 1;
        }
    }

    // Give the opponent a chance to miss its movement.
    if (rand() % OPPONENT_MISS_CHANCE != 0)
    {
        // Move the opponent paddle toward the ball.
        if (state.opponentPaddleY + PADDLE_HEIGHT / 2 < state.ballPosY)
        {
            state.opponentPaddleY++;
        }
        else if (state.opponentPaddleY + PADDLE_HEIGHT / 2 > state.ballPosY)
        {
            state.opponentPaddleY--;
        }
    }

    // Keep the opponent paddle inside the field.
    if (state.opponentPaddleY < PADDLE_TOP_LIMIT)
    {
        state.opponentPaddleY = PADDLE_TOP_LIMIT;
    }
    else if (state.opponentPaddleY > PADDLE_BOTTOM_LIMIT)
    {
        state.opponentPaddleY = PADDLE_BOTTOM_LIMIT;
    }

    // Store updated ball velocity.
    runtime.SetStateValue("ball_velocity_x", state.ballVelX);
    runtime.SetStateValue("ball_velocity_y", state.ballVelY);

    // Store updated game values.
    runtime.SetStateValue("ball_position_x", state.ballPosX);
    runtime.SetStateValue("ball_position_y", state.ballPosY);
    runtime.SetStateValue("opponent_paddle_y", state.opponentPaddleY);

    // Evaluate registered gameplay rules.
    runtime.Update();

    // Retrieve values after runtime rules have been applied.
    runtime.GetStateValue("ball_position_x", state.ballPosX);
    runtime.GetStateValue("ball_position_y", state.ballPosY);
    runtime.GetStateValue("ball_velocity_x", state.ballVelX);
    runtime.GetStateValue("ball_velocity_y", state.ballVelY);

    // Keep ball inside the vertical boundaries.
    if (state.ballPosY < PLAYFIELD_TOP)
    {
        state.ballPosY = PLAYFIELD_TOP;
    }
    else if (state.ballPosY > PLAYFIELD_BOTTOM)
    {
        state.ballPosY = PLAYFIELD_BOTTOM;
    }

    runtime.SetStateValue("ball_position_y", state.ballPosY);
}


void PONG::RenderGame()
{
    

    // Retrieve current game state.
    PongState state = LoadGameState(runtime);

    render.SetBackground();

    // Define game objects.
    std::vector<RenderObject> objects;

    // Begin a new frame.
    render.BeginFrame(FIELD_WIDTH, FIELD_HEIGHT + RENDER_EXTRA_HEIGHT, ThemeRoles::Background, WORLD_VERTICAL_OFFSET);

    // Draw scoreboard.
    render.DrawText( 0, 0, "PLAYER: " + std::to_string(state.playerScore) + "  \t\t\t\tOPPONENT: " + std::to_string(state.opponentScore), ThemeRoles::UI);

    Boundary boundary(FIELD_WIDTH, FIELD_HEIGHT, 0, PLAYFIELD_TOP, FIELD_RIGHT_EDGE, PLAYFIELD_TOP);

    const auto& ascii = pongConfig["game"]["rendering"]["ascii"];

    boundary.Draw(render, objects, ascii["top_border"].get<std::string>()[0],
        ascii["bottom_border"].get<std::string>()[0], ascii["vertical_border"].get<std::string>()[0],
        pongConfig["game"]["rendering"]["border_role"].get<std::string>()
    );

    // Player paddle.
    for (int i = 0; i < PADDLE_HEIGHT; i++)
    {
        objects.push_back({ Position(PLAYER_PADDLE_X, state.playerPaddleY + i), '|', ThemeRoles::Paddle });
    }

    // Opponent paddle.
    for (int i = 0; i < PADDLE_HEIGHT; i++)
    {
        objects.push_back({ Position(OPPONENT_PADDLE_X, state.opponentPaddleY + i), '|', ThemeRoles::Paddle });
    }

    // Ball.
    objects.push_back({ Position(state.ballPosX, state.ballPosY), 'O', ThemeRoles::Ball });

    // Submit objects to the renderer.
    render.DrawObjects(objects);

    // Draw controls.
    render.DrawText(0, FIELD_HEIGHT + CONTROLS_ROW_OFFSET, "W/S: Move Paddle    ESC: Exit", ThemeRoles::UI);

    // Present completed frame.
    render.PresentFrame();
}


void PONG::CheckGameOver()//checks whether the game has ended
{
    int playerScore = 0;
    int opponentScore = 0;
    int winningScore = 0;

    // Retrieve current scores and winning score.
    runtime.GetStateValue("player_score", playerScore);
    runtime.GetStateValue("opponent_score", opponentScore);
    runtime.GetStateValue("winning_score", winningScore);

    Theme* activeTheme = themeManager.GetActiveTheme();
    ThemeColor ui = activeTheme->GetColor(ThemeRoles::UI);

    // Check whether the player has won.
    if (playerScore >= winningScore)
    {
        Colors::Print("\nYOU WIN!!!\n", ui.foreground);
        running = false;
    }

    // Check whether the opponent has won.
    else if (opponentScore >= winningScore)
    {
        Colors::Print("\nYOU LOSE!!!\n", ui.foreground);
        running = false;
    }

    if (!running)
    {
        Colors::SetBackground(Colors::Color::Black);
        render.SetCursorPosition(0, FIELD_HEIGHT + RENDER_EXTRA_HEIGHT);

        Colors::Print("Final Score: " + std::to_string(playerScore) + " - " 
            + std::to_string(opponentScore) + "\n", ui.foreground);

        Colors::Print("Press any key to return to the menu.\n", ui.foreground);
        InputHelper::GetKey();

        system("cls");
    }
}