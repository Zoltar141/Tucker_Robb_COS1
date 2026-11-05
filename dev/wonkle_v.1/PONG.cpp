#include "PONG.h"
#include "InputHelper.h"
#include "ThemeManager.h"
#include "Colors.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>

const int FIELD_WIDTH = 80;
const int FIELD_HEIGHT = 25;
const int PADDLE_HEIGHT = 5;
const int WINNING_SCORE = 10;


PONG::PONG(ThemeManager& themeManager)//constructor, connects PONG to ThemeManager
    : themeManager(themeManager)
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
    if (ballVelocityX < 0 &&
        ballX <= 3 &&
        ballY >= playerY &&
        ballY < playerY + PADDLE_HEIGHT)
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
    if (ballVelocityX > 0 &&
        ballX >= FIELD_WIDTH - 4 &&
        ballY >= opponentY &&
        ballY < opponentY + PADDLE_HEIGHT)
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


void PONG::RenderGame()//Displays the game in the console.
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

    // Retrieve the currently active theme.
    Theme* activeTheme = themeManager.GetActiveTheme();

    //retrieve colors using semantic roles.
    ThemeColor background = activeTheme->GetColor(ThemeRoles::Background);
    ThemeColor foreground = activeTheme->GetColor(ThemeRoles::Foreground);
    ThemeColor ui = activeTheme->GetColor(ThemeRoles::UI);
    ThemeColor border = activeTheme->GetColor(ThemeRoles::Border);
    ThemeColor paddle = activeTheme->GetColor(ThemeRoles::Paddle);
    ThemeColor ball = activeTheme->GetColor(ThemeRoles::Ball);

    // Move cursor to the top-left corner.
    std::cout << "\033[H";

    Colors::SetBackground(background.background);

    // Display the scoreboard.
    Colors::Print("PLAYER: " + std::to_string(playerScore) + "  \t\t\t\tOPPONENT: " + std::to_string(opponentScore) + "\n", ui.foreground);

    // Draw the top boundary.
    for (int x = 0; x < FIELD_WIDTH; x++)
    {
        Colors::Print('_', border.foreground);
    }

    std::cout << "\n";

    // Draw the playing field.
    for (int y = 0; y < FIELD_HEIGHT; y++)
    {
        for (int x = 0; x < FIELD_WIDTH; x++)
        {
            if (x == 0 || x == FIELD_WIDTH - 1)
            {
                Colors::Print('|', border.foreground);
            }
            else if (x == 2 && y >= playerY && y < playerY + PADDLE_HEIGHT)
            {
                Colors::Print('|', paddle.foreground);
            }
            else if (x == FIELD_WIDTH - 3 && y >= opponentY && y < opponentY + PADDLE_HEIGHT)
            {
                Colors::Print('|', paddle.foreground);
            }
            else if (x == ballX && y == ballY)
            {
                Colors::Print('O', ball.foreground);
            }
            else
            {
                std::cout << " ";
            }
        }

        std::cout << "\n";
    }

    // Draw the bottom boundary.
    for (int x = 0; x < FIELD_WIDTH; x++)
    {
        Colors::Print('-', border.foreground);
    }

    std::cout << "\n";
    Colors::Print("W/S: Move Paddle    ESC: Exit\n", ui.foreground);
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
        Colors::Print("Final Score: " + std::to_string(playerScore) + " - " 
            + std::to_string(opponentScore) + "\n", ui.foreground);

        Colors::Print("Press any key to return to the menu.\n", ui.foreground);
        InputHelper::GetKey();
    }
}