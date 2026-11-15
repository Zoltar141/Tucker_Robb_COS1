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

PONG::PONG(ThemeManager& themeManager)//constructor, connects PONG to ThemeManager
    : GameController(themeManager, "pong.json")
{
}

void PONG::InitializeGame()
{
    // Every starting value comes from initial_state in pong.json.
    SetInitialState();
}

void PONG::HandleInput()// Processes player keyboard input.
{
    double playerPaddle= 0;

    runtime.GetStateValue("player_paddle_y", playerPaddle);//find position

    // Move paddle continuously while W or S is held.
    if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["up"].get<int>()))
    {
        if (playerPaddle > gameConfig["game"]["paddle"]["top_limit"].get<int>())
        {
            playerPaddle -= gameConfig["game"]["paddle"]["movement_step"].get<int>();
        }
    }

    if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["down"].get<int>()))
    {
        if (playerPaddle < gameConfig["game"]["paddle"]["bottom_limit"].get<int>())
        {
            playerPaddle += gameConfig["game"]["paddle"]["movement_step"].get<int>();
        }
    }

    // Exit the current game.
    if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["exit"].get<int>()))
    {
        running = false;
    }

    runtime.SetStateValue("player_paddle_y", playerPaddle);
}

void PONG::UpdateGame()// Advances the game state.
{
    // Retrieve current game state.
    std::map<std::string, int> state = LoadGameState();

    // Move the ball using its current velocity.
    state["ball_position_x"] += state["ball_velocity_x"];
    state["ball_position_y"] += state["ball_velocity_y"];

    Boundary boundary = GetBoundary();

    BoundarySide collision = Collision::CheckBoundary(Position(state["ball_position_x"], state["ball_position_y"]), boundary);

    if (collision == BoundarySide::Top || collision == BoundarySide::Bottom)
    {
        state["ball_velocity_y"] *= gameConfig["game"]["collision"]["vertical_boundary_bounce_multiplier"].get<int>();
    }

    // Player paddle collision.
    bool playerPaddleCollision = Collision::CheckRectangle(
        Position(state["ball_position_x"], state["ball_position_y"]),
        Position(gameConfig["game"]["paddle"]["player_x"].get<int>(), state["player_paddle_y"]),
        gameConfig["game"]["paddle"]["width"].get<int>(),
        gameConfig["game"]["paddle"]["height"].get<int>()
    );

    if (state["ball_velocity_x"] < 0 && playerPaddleCollision)
    {
        state["ball_velocity_x"] = gameConfig["game"]["collision"]["player_bounce_velocity_x"].get<int>();

        // Calculate where the ball hits the paddle.
        int hitPosition = state["ball_position_y"] - state["player_paddle_y"];

        // Determine vertical ball direction.
        if (hitPosition < gameConfig["game"]["collision"]["paddle_hit_vertical_threshold"].get<int>())
        {
            state["ball_velocity_y"] = gameConfig["game"]["collision"]["upper_bounce_velocity_y"].get<int>();
        }
        else if (hitPosition > gameConfig["game"]["collision"]["paddle_hit_vertical_threshold"].get<int>())
        {
            state["ball_velocity_y"] = gameConfig["game"]["collision"]["lower_bounce_velocity_y"].get<int>();
        }
    }

    // Opponent paddle collision.
    bool opponentPaddleCollision = Collision::CheckRectangle(
        Position(state["ball_position_x"], state["ball_position_y"]),
        Position(gameConfig["game"]["paddle"]["opponent_x"].get<int>(), state["opponent_paddle_y"]),
        gameConfig["game"]["paddle"]["width"].get<int>(),
        gameConfig["game"]["paddle"]["height"].get<int>()
    );

    if (state["ball_velocity_x"] > 0 && opponentPaddleCollision)
    {
        state["ball_velocity_x"] = gameConfig["game"]["collision"]["opponent_bounce_velocity_x"].get<int>();

        // Calculate where the ball hits the paddle.
        int hitPosition = state["ball_position_y"] - state["opponent_paddle_y"];

        // Determine vertical ball direction.
        if (hitPosition < gameConfig["game"]["collision"]["paddle_hit_vertical_threshold"].get<int>())
        {
            state["ball_velocity_y"] = gameConfig["game"]["collision"]["upper_bounce_velocity_y"].get<int>();
        }
        else if (hitPosition > gameConfig["game"]["collision"]["paddle_hit_vertical_threshold"].get<int>())
        {
            state["ball_velocity_y"] = gameConfig["game"]["collision"]["lower_bounce_velocity_y"].get<int>();
        }
    }

    // Keep the opponent paddle inside the field.
    if (state["opponent_paddle_y"] < gameConfig["game"]["paddle"]["top_limit"].get<int>())
    {
        state["opponent_paddle_y"] = gameConfig["game"]["paddle"]["top_limit"].get<int>();
    }
    else if (state["opponent_paddle_y"] > gameConfig["game"]["paddle"]["bottom_limit"].get<int>())
    {
        state["opponent_paddle_y"] = gameConfig["game"]["paddle"]["bottom_limit"].get<int>();
    }

    // Store updated ball velocity.
    runtime.SetStateValue("ball_velocity_x", state["ball_velocity_x"]);
    runtime.SetStateValue("ball_velocity_y", state["ball_velocity_y"]);

    // Store updated game values.
    runtime.SetStateValue("ball_position_x", state["ball_position_x"]);
    runtime.SetStateValue("ball_position_y", state["ball_position_y"]);
    runtime.SetStateValue("opponent_paddle_y", state["opponent_paddle_y"]);

    // Evaluate registered gameplay rules.
    runtime.Update();

    // Retrieve values after runtime rules have been applied.
    runtime.GetStateValue("ball_position_x", state["ball_position_x"]);
    runtime.GetStateValue("ball_position_y", state["ball_position_y"]);
    runtime.GetStateValue("ball_velocity_x", state["ball_velocity_x"]);
    runtime.GetStateValue("ball_velocity_y", state["ball_velocity_y"]);

    // Keep ball inside the vertical boundaries.
    if (state["ball_position_y"] < gameConfig["game"]["field"]["top"].get<int>())
    {
        state["ball_position_y"] = gameConfig["game"]["field"]["top"].get<int>();
    }
    else if (state["ball_position_y"] > gameConfig["game"]["field"]["bottom"].get<int>())
    {
        state["ball_position_y"] = gameConfig["game"]["field"]["bottom"].get<int>();
    }

    runtime.SetStateValue("ball_position_y", state["ball_position_y"]);
}


void PONG::RenderGame()
{
    // Retrieve current game state.
    std::map<std::string, int> state = LoadGameState();

    // Define game objects.
    std::vector<RenderObject> objects;

    // Begin a new frame.
    BeginFrame();

    // Draw scoreboard.
    DrawScoreboard(
        gameConfig["game"]["rendering"]["scoreboard"]["player_label"].get<std::string>() + " " + std::to_string(state["player_score"]) +
        gameConfig["game"]["rendering"]["scoreboard"]["separator"].get<std::string>() +
        gameConfig["game"]["rendering"]["scoreboard"]["opponent_label"].get<std::string>() + " " + std::to_string(state["opponent_score"]));

    const auto& ascii = gameConfig["game"]["rendering"]["ascii"];

    DrawBoundary(objects);

    // Player paddle.
    for (int i = 0; i < gameConfig["game"]["paddle"]["height"].get<int>(); i++)
    {
        objects.push_back({
            Position(gameConfig["game"]["paddle"]["player_x"].get<int>(), state["player_paddle_y"] + i),
            ascii["paddle"].get<std::string>()[0],
            gameConfig["game"]["rendering"]["roles"]["paddle"].get<std::string>()
            });
    }

    // Opponent paddle.
    for (int i = 0; i < gameConfig["game"]["paddle"]["height"].get<int>(); i++)
    {
        objects.push_back({
            Position(gameConfig["game"]["paddle"]["opponent_x"].get<int>(), state["opponent_paddle_y"] + i),
            ascii["paddle"].get<std::string>()[0],
            gameConfig["game"]["rendering"]["roles"]["paddle"].get<std::string>()
            });
    }

    // Ball.
    objects.push_back({Position(state["ball_position_x"], state["ball_position_y"]), 
        ascii["ball"].get<std::string>()[0],  
        gameConfig["game"]["rendering"]["roles"]["ball"].get<std::string>()});

    // Submit objects to the renderer.
    render.DrawObjects(objects);

    // Draw controls.
    DrawBottomText(gameConfig["game"]["controls"]["display_text"].get<std::string>());

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
        Colors::Print(gameConfig["game"]["rendering"]["messages"]["player_wins"].get<std::string>(), ui.foreground);
        running = false;
    }

    // Check whether the opponent has won.
    else if (opponentScore >= winningScore)
    {
        Colors::Print(gameConfig["game"]["rendering"]["messages"]["opponent_wins"].get<std::string>(), ui.foreground);
        running = false;
    }

    if (!running)
    {
        Colors::SetBackground(Colors::Color::Black);

        render.SetCursorPosition(
            gameConfig["game"]["rendering"]["game_over_column"].get<int>(),
            gameConfig["game"]["field"]["height"].get<int>() +
            gameConfig["game"]["rendering"]["extra_height"].get<int>()
        );

        Colors::Print(
            gameConfig["game"]["rendering"]["messages"]["final_score"].get<std::string>() +
            std::to_string(playerScore) + 
            gameConfig["game"]["rendering"]["messages"]["score_separator"].get<std::string>() +
            std::to_string(opponentScore) + "\n",
            ui.foreground
        );

        Colors::Print(
            gameConfig["game"]["rendering"]["messages"]["return_prompt"].get<std::string>(),
            ui.foreground
        );

        InputHelper::GetKey();

        system("cls");
    }
}