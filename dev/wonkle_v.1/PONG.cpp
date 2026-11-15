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
        //This prevents the game from running as fast as the processor allows.
        std::this_thread::sleep_for(std::chrono::milliseconds(pongConfig["game"]["timing"]["frame_delay_ms"].get<int>()));
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

    runtime.SetStateValue("ball_position_x", initialState["ball"]["position"]["x"].get<int>());
    runtime.SetStateValue("ball_position_y", initialState["ball"]["position"]["y"].get<int>());

    runtime.SetStateValue("ball_velocity_x", initialState["ball"]["velocity"]["x"].get<int>());
    runtime.SetStateValue("ball_velocity_y", initialState["ball"]["velocity"]["y"].get<int>());

    runtime.SetStateValue("player_paddle_y", initialState["paddles"]["player_y"].get<int>());
    runtime.SetStateValue("opponent_paddle_y", initialState["paddles"]["opponent_y"].get<int>());

    runtime.SetStateValue("player_score", initialState["score"]["player"].get<int>());
    runtime.SetStateValue("opponent_score", initialState["score"]["opponent"].get<int>());

    runtime.SetStateValue("winning_score", pongConfig["game"]["scoring"]["winning_score"].get<int>());

    runtime.SetStateValue("opponent_reaction_timer", 0);
    runtime.SetStateValue("opponent_reaction_delay",
        pongConfig["game"]["opponent"]["reaction_delay_frames"].get<double>());
}


void PONG::SetupRules()
{
    runtime.LoadRules(pongConfig["game"]["rules"]);
}

void PONG::HandleInput()// Processes player keyboard input.
{
    double playerPaddle= 0;

    runtime.GetStateValue("player_paddle_y", playerPaddle);//find position

    // Move paddle continuously while W or S is held.
    if (InputHelper::IsKeyDown(pongConfig["game"]["controls"]["up"].get<int>()))
    {
        if (playerPaddle > pongConfig["game"]["paddle"]["top_limit"].get<int>())
        {
            playerPaddle--;
        }
    }

    if (InputHelper::IsKeyDown(pongConfig["game"]["controls"]["down"].get<int>()))
    {
        if (playerPaddle < pongConfig["game"]["paddle"]["bottom_limit"].get<int>())
        {
            playerPaddle++;
        }
    }

    // Exit the current game.
    if (InputHelper::IsKeyDown(pongConfig["game"]["controls"]["exit"].get<int>()))
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
    Boundary boundary(
        pongConfig["game"]["field"]["width"].get<int>(),
        pongConfig["game"]["field"]["height"].get<int>(),
        0,
        pongConfig["game"]["field"]["top"].get<int>(),
        pongConfig["game"]["field"]["right_edge"].get<int>(),
        pongConfig["game"]["field"]["bottom"].get<int>()
    );

    BoundarySide collision = Collision::CheckBoundary(Position(state.ballPosX, state.ballPosY), boundary);

    if (collision == BoundarySide::Top || collision == BoundarySide::Bottom)
    {
        state.ballVelY *= -1;
    }

    // Player paddle collision.
    bool playerPaddleCollision = Collision::CheckRectangle(
        Position(state.ballPosX, state.ballPosY),
        Position(pongConfig["game"]["paddle"]["player_x"].get<int>(), state.playerPaddleY),
        pongConfig["game"]["paddle"]["width"].get<int>(),
        pongConfig["game"]["paddle"]["height"].get<int>()
    );

    if (state.ballVelX < 0 && playerPaddleCollision)
    {
        state.ballVelX = pongConfig["game"]["collision"]["player_bounce_velocity_x"].get<int>();

        // Calculate where the ball hits the paddle.
        int hitPosition = state.ballPosY - state.playerPaddleY;

        // Determine vertical ball direction.
        if (hitPosition < pongConfig["game"]["collision"]["paddle_hit_vertical_threshold"].get<int>())
        {
            state.ballVelY = pongConfig["game"]["collision"]["upper_bounce_velocity_y"].get<int>();
        }
        else if (hitPosition > pongConfig["game"]["collision"]["paddle_hit_vertical_threshold"].get<int>())
        {
            state.ballVelY = pongConfig["game"]["collision"]["lower_bounce_velocity_y"].get<int>();
        }
    }

    // Opponent paddle collision.
    bool opponentPaddleCollision = Collision::CheckRectangle(
        Position(state.ballPosX, state.ballPosY),
        Position(pongConfig["game"]["paddle"]["opponent_x"].get<int>(), state.opponentPaddleY),
        pongConfig["game"]["paddle"]["width"].get<int>(),
        pongConfig["game"]["paddle"]["height"].get<int>()
    );

    if (state.ballVelX > 0 && opponentPaddleCollision)
    {
        state.ballVelX = pongConfig["game"]["collision"]["opponent_bounce_velocity_x"].get<int>();

        // Calculate where the ball hits the paddle.
        int hitPosition = state.ballPosY - state.opponentPaddleY;

        // Determine vertical ball direction.
        if (hitPosition < pongConfig["game"]["collision"]["paddle_hit_vertical_threshold"].get<int>())
        {
            state.ballVelY = pongConfig["game"]["collision"]["upper_bounce_velocity_y"].get<int>();
        }
        else if (hitPosition > pongConfig["game"]["collision"]["paddle_hit_vertical_threshold"].get<int>())
        {
            state.ballVelY = pongConfig["game"]["collision"]["lower_bounce_velocity_y"].get<int>();
        }
    }

    // Keep the opponent paddle inside the field.
    if (state.opponentPaddleY < pongConfig["game"]["paddle"]["top_limit"].get<int>())
    {
        state.opponentPaddleY = pongConfig["game"]["paddle"]["top_limit"].get<int>();
    }
    else if (state.opponentPaddleY > pongConfig["game"]["paddle"]["bottom_limit"].get<int>())
    {
        state.opponentPaddleY = pongConfig["game"]["paddle"]["bottom_limit"].get<int>();
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
    if (state.ballPosY < pongConfig["game"]["field"]["top"].get<int>())
    {
        state.ballPosY = pongConfig["game"]["field"]["top"].get<int>();
    }
    else if (state.ballPosY > pongConfig["game"]["field"]["bottom"].get<int>())
    {
        state.ballPosY = pongConfig["game"]["field"]["bottom"].get<int>();
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
    render.BeginFrame(pongConfig["game"]["field"]["width"].get<int>(), 
        pongConfig["game"]["field"]["height"].get<int>() + pongConfig["game"]["rendering"]["extra_height"].get<int>(),
        pongConfig["game"]["rendering"]["roles"]["background"].get<std::string>(), 
        pongConfig["game"]["rendering"]["world_vertical_offset"].get<int>());

    // Draw scoreboard.
    render.DrawText(0, pongConfig["game"]["rendering"]["scoreboard_row"].get<int>(), 
        pongConfig["game"]["rendering"]["scoreboard"]["player_label"].get<std::string>() + " " + std::to_string(state.playerScore) + 
        pongConfig["game"]["rendering"]["scoreboard"]["separator"].get<std::string>() + 
        pongConfig["game"]["rendering"]["scoreboard"]["opponent_label"].get<std::string>() + " " + std::to_string(state.opponentScore), 
        pongConfig["game"]["rendering"]["roles"]["ui"].get<std::string>());

    Boundary boundary(
        pongConfig["game"]["field"]["width"].get<int>(),
        pongConfig["game"]["field"]["height"].get<int>(),
        0,
        pongConfig["game"]["field"]["top"].get<int>(),
        pongConfig["game"]["field"]["right_edge"].get<int>(),
        pongConfig["game"]["field"]["bottom"].get<int>());

    const auto& ascii = pongConfig["game"]["rendering"]["ascii"];

   

    boundary.Draw(render, objects, 
        ascii["top_border"].get<std::string>()[0],
        ascii["bottom_border"].get<std::string>()[0], 
        ascii["vertical_border"].get<std::string>()[0],
        pongConfig["game"]["rendering"]["roles"]["border"].get<std::string>()
    );

    // Player paddle.
    for (int i = 0; i < pongConfig["game"]["paddle"]["height"].get<int>(); i++)
    {
        objects.push_back({
            Position(pongConfig["game"]["paddle"]["player_x"].get<int>(), state.playerPaddleY + i),
            ascii["paddle"].get<std::string>()[0],
            pongConfig["game"]["rendering"]["roles"]["paddle"].get<std::string>()
            });
    }

    // Opponent paddle.
    for (int i = 0; i < pongConfig["game"]["paddle"]["height"].get<int>(); i++)
    {
        objects.push_back({
            Position(pongConfig["game"]["paddle"]["opponent_x"].get<int>(), state.opponentPaddleY + i),
            ascii["paddle"].get<std::string>()[0],
            pongConfig["game"]["rendering"]["roles"]["paddle"].get<std::string>()
            });
    }

    // Ball.
    objects.push_back({Position(state.ballPosX, state.ballPosY), 
        ascii["ball"].get<std::string>()[0],  
        pongConfig["game"]["rendering"]["roles"]["ball"].get<std::string>()});

    // Submit objects to the renderer.
    render.DrawObjects(objects);

    // Draw controls.
    render.DrawText(0, 
        pongConfig["game"]["field"]["height"].get<int>() + pongConfig["game"]["rendering"]["controls_row_offset"].get<int>(),
        pongConfig["game"]["controls"]["display_text"].get<std::string>(), ThemeRoles::UI);

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
        Colors::Print(pongConfig["game"]["rendering"]["messages"]["player_wins"].get<std::string>(), ui.foreground);
        running = false;
    }

    // Check whether the opponent has won.
    else if (opponentScore >= winningScore)
    {
        Colors::Print(pongConfig["game"]["rendering"]["messages"]["opponent_wins"].get<std::string>(), ui.foreground);
        running = false;
    }

    if (!running)
    {
        Colors::SetBackground(Colors::Color::Black);

        render.SetCursorPosition(
            0,
            pongConfig["game"]["field"]["height"].get<int>() +
            pongConfig["game"]["rendering"]["extra_height"].get<int>()
        );

        Colors::Print(
            pongConfig["game"]["rendering"]["messages"]["final_score"].get<std::string>() +
            std::to_string(playerScore) + " - " +
            std::to_string(opponentScore) + "\n",
            ui.foreground
        );

        Colors::Print(
            pongConfig["game"]["rendering"]["messages"]["return_prompt"].get<std::string>(),
            ui.foreground
        );

        InputHelper::GetKey();

        system("cls");
    }
}