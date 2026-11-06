#include "PONG.h"
#include "InputHelper.h"
#include "ThemeManager.h"
#include "Colors.h"
#include "Boundary.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>
#include <vector>

const int FIELD_WIDTH = 80;
const int FIELD_HEIGHT = 25;
const int PADDLE_HEIGHT = 5;
const int WINNING_SCORE = 10;


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
        std::this_thread::sleep_for(std::chrono::milliseconds(50));//This prevents the game from running as fast as the processor allows.
    }
    Colors::SetBackground(Colors::Color::Black);
    Colors::Reset();
}

void PONG::InitializeGame()//sets initial state values
{
    runtime.SetStateValue("ball_position_x", 40);
    runtime.SetStateValue("ball_position_y", 12);

    runtime.SetStateValue("ball_velocity_x", 1);
    runtime.SetStateValue("ball_velocity_y", 1);

    runtime.SetStateValue("player_paddle_y", 10);
    runtime.SetStateValue("opponent_paddle_y", 10);

    runtime.SetStateValue("player_score", 0);
    runtime.SetStateValue("opponent_score", 0);

    runtime.SetStateValue("winning_score", WINNING_SCORE);
}


void PONG::SetupRules()//egisters the gameplay rules
{
    // Rule 1: reverse ball direction when hitting the top wall
    WonkleRuntime::Rule topWall;

    topWall.conditions.push_back({
    "ball_position_y",
    Comparison::LessEqual,
    1
        });

    topWall.transformations.push_back({
        "ball_velocity_y",
        Operation::Multiply,
        -1
        });

    runtime.AddRule(topWall);


    // Rule 2: reverse ball direction when hitting the bottom wall
    WonkleRuntime::Rule bottomWall;

    bottomWall.conditions.push_back({
        "ball_position_y",
        Comparison::GreaterEqual,
        FIELD_HEIGHT - 2
        });

    bottomWall.transformations.push_back({
        "ball_velocity_y",
        Operation::Multiply,
        -1
        });

    runtime.AddRule(bottomWall);


    // Rule 3: award opponent a point when ball leaves the player side
    WonkleRuntime::Rule leftScore;

    leftScore.conditions.push_back({
        "ball_position_x",
        Comparison::LessEqual,
        1
        });

    leftScore.transformations.push_back({
        "opponent_score",
        Operation::Add,
        1
        });

    leftScore.transformations.push_back({
        "ball_position_x",
        Operation::Assign,
        FIELD_WIDTH / 2
        });

    leftScore.transformations.push_back({
        "ball_position_y",
        Operation::Assign,
        FIELD_HEIGHT / 2
        });

    leftScore.transformations.push_back({
        "ball_velocity_x",
        Operation::Assign,
        1
        });
    leftScore.transformations.push_back({
        "ball_velocity_y",
        Operation::Assign,
        1
        });

    runtime.AddRule(leftScore);


    // Rule 4: award player a point when ball leaves the opponet side
    WonkleRuntime::Rule rightScore;

    rightScore.conditions.push_back({
        "ball_position_x",
        Comparison::GreaterEqual,
        FIELD_WIDTH - 2
        });

    rightScore.transformations.push_back({
        "player_score",
        Operation::Add,
        1
        });

    rightScore.transformations.push_back({
        "ball_position_x",
        Operation::Assign,
        FIELD_WIDTH / 2
        });

    rightScore.transformations.push_back({
        "ball_position_y",
        Operation::Assign,
        FIELD_HEIGHT / 2
        });

    rightScore.transformations.push_back({
        "ball_velocity_x",
        Operation::Assign,
        -1
        });
    rightScore.transformations.push_back({
        "ball_velocity_y",
        Operation::Assign,
        1
        });

    runtime.AddRule(rightScore);
}


void PONG::HandleInput()// Processes player keyboard input.
{
    int playerY= 0;

    runtime.GetStateValue("player_paddle_y", playerY);//find position

    if (InputHelper::IsKeyPressed())
    {
        char key = InputHelper::GetKey();

        switch (key)
        {
        case 'w':
        case 'W':
            if (playerY > 0)// Move player paddle up.
            {
                playerY--;
            }
            break;
        case 's':
        case 'S':
            if (playerY < FIELD_HEIGHT - PADDLE_HEIGHT)// Move player paddle down.
            {
                playerY++;
            }
            break;
        case 27://esc key
            // Exit the current game.
            running = false;//can add save state here
            break;
        }
    }
    runtime.SetStateValue("player_paddle_y", playerY);
}

void PONG::UpdateGame()// Advances the game state.

{
    int ballX = 0;
    int ballY = 0;
    int ballVelocityX = 0;
    int ballVelocityY = 0;
    int opponentY = 0;
    int playerY = 0;

    // Retrieve current game state.
    runtime.GetStateValue("ball_position_x", ballX);
    runtime.GetStateValue("ball_position_y", ballY);
    runtime.GetStateValue("ball_velocity_x", ballVelocityX);
    runtime.GetStateValue("ball_velocity_y", ballVelocityY);
    runtime.GetStateValue("opponent_paddle_y", opponentY);
    runtime.GetStateValue("player_paddle_y", playerY);

    // Move the ball using its current velocity.
    ballX += ballVelocityX;
    ballY += ballVelocityY;

    // Player paddle collision.
    if (ballVelocityX < 0 && ballX <= 3 && ballY >= playerY && ballY < playerY + PADDLE_HEIGHT)
    {
        ballVelocityX = 1;

        // Calculate where the ball hits the paddle.
        int hitPosition = ballY - playerY;

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
    if (ballVelocityX > 0 && ballX >= FIELD_WIDTH - 4 && ballY >= opponentY && ballY < opponentY + PADDLE_HEIGHT)
    {
        ballVelocityX = -1;

        // Calculate where the ball hits the paddle.
        int hitPosition = ballY - opponentY;

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

    // Move the opponent paddle toward the ball.
    if (opponentY + PADDLE_HEIGHT / 2 < ballY)
    {
        opponentY++;
    }
    else if (opponentY + PADDLE_HEIGHT / 2 > ballY)
    {
        opponentY--;
    }

    // Keep the opponent paddle inside the field.
    if (opponentY < 0)
    {
        opponentY = 0;
    }
    else if (opponentY > FIELD_HEIGHT - PADDLE_HEIGHT)
    {
        opponentY = FIELD_HEIGHT - PADDLE_HEIGHT;
    }

    // Store updated ball velocity.
    runtime.SetStateValue("ball_velocity_x", ballVelocityX);
    runtime.SetStateValue("ball_velocity_y", ballVelocityY);

    // Store updated game values.
    runtime.SetStateValue("ball_position_x", ballX);
    runtime.SetStateValue("ball_position_y", ballY);
    runtime.SetStateValue("opponent_paddle_y", opponentY);

    // Evaluate registered gameplay rules.
    runtime.Update();

    // Retrieve values after runtime rules have been applied.
    runtime.GetStateValue("ball_position_x", ballX);
    runtime.GetStateValue("ball_position_y", ballY);
    runtime.GetStateValue("ball_velocity_x", ballVelocityX);
    runtime.GetStateValue("ball_velocity_y", ballVelocityY);

    // Keep ball inside the vertical boundaries.
    if (ballY < 1)
    {
        ballY = 1;
    }
    else if (ballY > FIELD_HEIGHT - 2)
    {
        ballY = FIELD_HEIGHT - 2;
    }

    runtime.SetStateValue("ball_position_y", ballY);
}


void PONG::RenderGame()
{
    int ballX = 0;
    int ballY = 0;
    int playerY = 0;
    int opponentY = 0;
    int playerScore = 0;
    int opponentScore = 0;

    // Retrieve current game state.
    runtime.GetStateValue("ball_position_x", ballX);
    runtime.GetStateValue("ball_position_y", ballY);
    runtime.GetStateValue("player_paddle_y", playerY);
    runtime.GetStateValue("opponent_paddle_y", opponentY);
    runtime.GetStateValue("player_score", playerScore);
    runtime.GetStateValue("opponent_score", opponentScore);

    render.SetBackground();

    // Define game objects.
    std::vector<RenderObject> objects;

    // Begin a new frame.
    render.BeginFrame(FIELD_WIDTH, FIELD_HEIGHT + 4, ThemeRoles::Background);

    // Draw scoreboard.
    render.DrawText( 0, 0, "PLAYER: " + std::to_string(playerScore) + "  \t\t\t\tOPPONENT: " + std::to_string(opponentScore), ThemeRoles::UI);

    Boundary boundary(FIELD_WIDTH, FIELD_HEIGHT);

    boundary.Draw(render, objects);

    // Player paddle.
    for (int i = 0; i < PADDLE_HEIGHT; i++)
    {
        objects.push_back({Position(2, playerY + 2 + i), '|', ThemeRoles::Paddle});
    }

    // Opponent paddle.
    for (int i = 0; i < PADDLE_HEIGHT; i++)
    {
        objects.push_back({Position(FIELD_WIDTH - 3, opponentY + 2 + i), '|', ThemeRoles::Paddle});
    }

    // Ball.
    objects.push_back({Position(ballX, ballY + 2), 'O', ThemeRoles::Ball});

    // Submit objects to the renderer.
    render.DrawObjects(objects);

    // Draw controls.
    render.DrawText(0, FIELD_HEIGHT + 3, "W/S: Move Paddle    ESC: Exit", ThemeRoles::UI);

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
        render.SetCursorPosition(0, FIELD_HEIGHT + 4);

        Colors::Print("Final Score: " + std::to_string(playerScore) + " - " 
            + std::to_string(opponentScore) + "\n", ui.foreground);

        Colors::Print("Press any key to return to the menu.\n", ui.foreground);
        InputHelper::GetKey();

        system("cls");
    }
}