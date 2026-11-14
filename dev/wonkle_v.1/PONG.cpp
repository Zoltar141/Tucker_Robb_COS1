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


PONG::PONG(ThemeManager& themeManager)//constructor, connects PONG to ThemeManager
    : themeManager(themeManager), render(themeManager)
{
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

void PONG::InitializeGame()//sets initial state values
{
    runtime.SetStateValue("ball_position_x", FIELD_WIDTH / 2);
    runtime.SetStateValue("ball_position_y", FIELD_HEIGHT / 2);

    runtime.SetStateValue("ball_velocity_x", 1);
    runtime.SetStateValue("ball_velocity_y", 1);

    runtime.SetStateValue("player_paddle_y", INITIAL_PADDLE_Y);
    runtime.SetStateValue("opponent_paddle_y", INITIAL_PADDLE_Y);

    runtime.SetStateValue("player_score", 0);
    runtime.SetStateValue("opponent_score", 0);

    runtime.SetStateValue("winning_score", WINNING_SCORE);
}


void PONG::SetupRules()//registers the gameplay rules
{
   
    // Rule 1: award opponent a point when ball leaves the player side
    WonkleRuntime::Rule leftScore;
    leftScore.conditions.push_back({ "ball_position_x", Comparison::LessEqual, LEFT_SCORING_BOUNDARY });
    leftScore.transformations.push_back({ "opponent_score", Operation::Add, 1 });
    leftScore.transformations.push_back({ "ball_position_x", Operation::Assign, FIELD_WIDTH / 2 });
    leftScore.transformations.push_back({ "ball_position_y", Operation::Assign, FIELD_HEIGHT / 2 });
    leftScore.transformations.push_back({ "ball_velocity_x", Operation::Assign, 1 });
    leftScore.transformations.push_back({ "ball_velocity_y", Operation::Assign, 1 });
    runtime.AddRule(leftScore);

    // Rule 2: award player a point when ball leaves the opponent side
    WonkleRuntime::Rule rightScore;
    rightScore.conditions.push_back({ "ball_position_x", Comparison::GreaterEqual, RIGHT_SCORING_BOUNDARY });
    rightScore.transformations.push_back({ "player_score", Operation::Add, 1 });
    rightScore.transformations.push_back({ "ball_position_x", Operation::Assign, FIELD_WIDTH / 2 });
    rightScore.transformations.push_back({ "ball_position_y", Operation::Assign, FIELD_HEIGHT / 2 });
    rightScore.transformations.push_back({ "ball_velocity_x", Operation::Assign, -1 });
    rightScore.transformations.push_back({ "ball_velocity_y", Operation::Assign, 1 });
    runtime.AddRule(rightScore);
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
    runtime.SetStateValue("player_paddle_y", playerPaddle);
}

void PONG::UpdateGame()// Advances the game state.

{
    int ballPosX = 0;
    int ballPosY = 0;
    int ballVelocityX = 0;
    int ballVelocityY = 0;
    int opponentPaddle = 0;
    int playerPaddle = 0;

    // Retrieve current game state.
    runtime.GetStateValue("ball_position_x", ballPosX);
    runtime.GetStateValue("ball_position_y", ballPosY);
    runtime.GetStateValue("ball_velocity_x", ballVelocityX);
    runtime.GetStateValue("ball_velocity_y", ballVelocityY);
    runtime.GetStateValue("opponent_paddle_y", opponentPaddle);
    runtime.GetStateValue("player_paddle_y", playerPaddle);

    // Move the ball using its current velocity.
    ballPosX += ballVelocityX;
    ballPosY += ballVelocityY;

    // Check for boundary collisions.
    Boundary boundary(FIELD_WIDTH, FIELD_HEIGHT, 0, PLAYFIELD_TOP, FIELD_RIGHT_EDGE, PLAYFIELD_BOTTOM);

    BoundarySide collision = Collision::CheckBoundary(Position(ballPosX, ballPosY), boundary);

    if (collision == BoundarySide::Top || collision == BoundarySide::Bottom)
    {
        ballVelocityY *= -1;
    }

    // Player paddle collision.
    bool playerPaddleCollision = Collision::CheckRectangle(Position(ballPosX, ballPosY), Position(0, playerPaddle), PADDLE_WIDTH, PADDLE_HEIGHT);

    if (ballVelocityX < 0 && playerPaddleCollision)
    {
        ballVelocityX = 1;

        // Calculate where the ball hits the paddle.
        int hitPosition = ballPosY - playerPaddle;

        // Determine vertical ball direction.
        if (hitPosition < PADDLE_HEIGHT / 2)
        {
            ballVelocityY = -1;
        }
        else if (hitPosition > PADDLE_HEIGHT / 2)
        {
            ballVelocityY = 1;
        }
    }

    // Opponent paddle collision.
    bool opponentPaddleCollision = Collision::CheckRectangle(Position(ballPosX, ballPosY), Position(FIELD_WIDTH - PADDLE_WIDTH, opponentPaddle), PADDLE_WIDTH, PADDLE_HEIGHT);

    if (ballVelocityX > 0 && opponentPaddleCollision)
    {
        ballVelocityX = -1;

        // Calculate where the ball hits the paddle.
        int hitPosition = ballPosY - opponentPaddle;

        // Determine vertical ball direction.
        if (hitPosition < PADDLE_HEIGHT / 2)
        {
            ballVelocityY = -1;
        }
        else if (hitPosition > PADDLE_HEIGHT / 2)
        {
            ballVelocityY = 1;
        }
    }

    // Give the opponent a chance to miss its movement.
    if (rand() % OPPONENT_MISS_CHANCE != 0)
    {
        // Move the opponent paddle toward the ball.
        if (opponentPaddle + PADDLE_HEIGHT / 2 < ballPosY)
        {
            opponentPaddle++;
        }
        else if (opponentPaddle + PADDLE_HEIGHT / 2 > ballPosY)
        {
            opponentPaddle--;
        }
    }

    // Keep the opponent paddle inside the field.
    if (opponentPaddle < PADDLE_TOP_LIMIT)
    {
        opponentPaddle = PADDLE_TOP_LIMIT;
    }
    else if (opponentPaddle > PADDLE_BOTTOM_LIMIT)
    {
        opponentPaddle = PADDLE_BOTTOM_LIMIT;
    }

    // Store updated ball velocity.
    runtime.SetStateValue("ball_velocity_x", ballVelocityX);
    runtime.SetStateValue("ball_velocity_y", ballVelocityY);

    // Store updated game values.
    runtime.SetStateValue("ball_position_x", ballPosX);
    runtime.SetStateValue("ball_position_y", ballPosY);
    runtime.SetStateValue("opponent_paddle_y", opponentPaddle);

    // Evaluate registered gameplay rules.
    runtime.Update();

    // Retrieve values after runtime rules have been applied.
    runtime.GetStateValue("ball_position_x", ballPosX);
    runtime.GetStateValue("ball_position_y", ballPosY);
    runtime.GetStateValue("ball_velocity_x", ballVelocityX);
    runtime.GetStateValue("ball_velocity_y", ballVelocityY);

    // Keep ball inside the vertical boundaries.
    if (ballPosY < PLAYFIELD_TOP)
    {
        ballPosY = PLAYFIELD_TOP;
    }
    else if (ballPosY > PLAYFIELD_BOTTOM)
    {
        ballPosY = PLAYFIELD_BOTTOM;
    }

    runtime.SetStateValue("ball_position_y", ballPosY);
}


void PONG::RenderGame()
{
    int ballPosX = 0;
    int ballPosY = 0;
    int playerPaddle = 0;
    int opponentPaddle = 0;
    int playerScore = 0;
    int opponentScore = 0;

    // Retrieve current game state.
    runtime.GetStateValue("ball_position_x", ballPosX);
    runtime.GetStateValue("ball_position_y", ballPosY);
    runtime.GetStateValue("player_paddle_y", playerPaddle);
    runtime.GetStateValue("opponent_paddle_y", opponentPaddle);
    runtime.GetStateValue("player_score", playerScore);
    runtime.GetStateValue("opponent_score", opponentScore);

    render.SetBackground();

    // Define game objects.
    std::vector<RenderObject> objects;

    // Begin a new frame.
    render.BeginFrame(FIELD_WIDTH, FIELD_HEIGHT + RENDER_EXTRA_HEIGHT, ThemeRoles::Background, WORLD_VERTICAL_OFFSET);

    // Draw scoreboard.
    render.DrawText( 0, FIELD_HEIGHT + CONTROLS_ROW_OFFSET, "PLAYER: " + std::to_string(playerScore) + "  \t\t\t\tOPPONENT: " + std::to_string(opponentScore), ThemeRoles::UI);

    Boundary boundary(FIELD_WIDTH, FIELD_HEIGHT, 0, PLAYFIELD_TOP, FIELD_RIGHT_EDGE, PLAYFIELD_TOP);

    boundary.Draw(render, objects);

    // Player paddle.
    for (int i = 0; i < PADDLE_HEIGHT; i++)
    {
        objects.push_back({ Position(PLAYER_PADDLE_X, playerPaddle + i), '|', ThemeRoles::Paddle });
    }

    // Opponent paddle.
    for (int i = 0; i < PADDLE_HEIGHT; i++)
    {
        objects.push_back({ Position(OPPONENT_PADDLE_X, opponentPaddle + i), '|', ThemeRoles::Paddle });
    }

    // Ball.
    objects.push_back({ Position(ballPosX, ballPosY), 'O', ThemeRoles::Ball });

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