#include "BRICKBREAKER.h"
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

constexpr char EMPTY_BRICK = '0';  //a space in a brick row with no brick in it, the other digits are hits left

BRICKBREAKER::BRICKBREAKER(ThemeManager& themeManager)//constructor, hands the theme and brickbreaker.json to GameController
    : GameController(themeManager, "brickbreaker.json")
{
}

void BRICKBREAKER::InitializeGame()
{
    // Every starting value comes from initial_state in brickbreaker.json.
    SetInitialState();

    // The bricks are rows of characters, each character is how many hits that brick has left.
    const auto& bricks = gameConfig["game"]["bricks"];
    int brickCount = 0;

    brickRows.clear();

    for (const auto& layoutRow : bricks["layout"])
    {
        std::string row = layoutRow.get<std::string>();

        row.resize(bricks["width"].get<int>(), EMPTY_BRICK);//every row is the same width

        for (int i = 0; i < row.size(); i++)
        {
            if (row[i] > EMPTY_BRICK)
            {
                brickCount++;
            }
        }

        brickRows.push_back(row);
    }

    runtime.SetStateValue("bricks_remaining", brickCount);
}

void BRICKBREAKER::HandleInput()// Processes player keyboard input.
{
    double playerPaddle = 0;

    runtime.GetStateValue("player_paddle_x", playerPaddle);//find position

    // Move paddle continuously while A or D is held.
    if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["left"].get<int>()))
    {
        if (playerPaddle < gameConfig["game"]["paddle"]["left_limit"].get<int>())
        {
            playerPaddle -= gameConfig["game"]["paddle"]["movement_step"].get<int>();
        }
    }
    if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["right"].get<int>()))
    {
        if (playerPaddle > gameConfig["game"]["paddle"]["right_limit"].get<int>())
        {
            playerPaddle += gameConfig["game"]["paddle"]["movement_step"].get<int>();
        }
    }

    // Exit the current game.
    if (InputHelper::IsKeyDown(gameConfig["game"]["controls"]["exit"].get<int>()))
    {
        running = false;
    }

    runtime.SetStateValue("player_paddle_x", playerPaddle);
}

void BRICKBREAKER::UpdateGame()// Advances the game state.
{
    // Retrieve current game state.
    std::map<std::string, int> state = LoadGameState();

    // Move the ball using its current velocity.
    state["ball_position_x"] += state["ball_velocity_x"];
    state["ball_position_y"] += state["ball_velocity_y"];

    // Check for boundary collisions.
    Boundary boundary = GetBoundary();

    BoundarySide collision = Collision::CheckBoundary(Position(state["ball_position_x"], state["ball_position_y"]), boundary);

    //the ball only bounces off a wall it is moving toward, so it cannot get stuck on one
    if (collision == BoundarySide::Left && state["ball_velocity_x"] < 0)
    {
        state["ball_velocity_x"] *= gameConfig["game"]["collision"]["horizontal_boundary_bounce_multiplier"].get<int>();
    }
    else if (collision == BoundarySide::Right && state["ball_velocity_x"] > 0)
    {
        state["ball_velocity_x"] *= gameConfig["game"]["collision"]["horizontal_boundary_bounce_multiplier"].get<int>();
    }
    else if (collision == BoundarySide::Top && state["ball_velocity_y"] < 0)
    {
        state["ball_velocity_y"] *= gameConfig["game"]["collision"]["vertical_boundary_bounce_multiplier"].get<int>();
    }
    //the bottom is open, a runtime rule decides when the ball is lost

    // Keep ball inside the side walls.
    if (state["ball_position_x"] < gameConfig["game"]["field"]["left_edge"].get<int>())
    {
        state["ball_position_x"] = gameConfig["game"]["field"]["left_edge"].get<int>();
    }
    else if (state["ball_position_x"] > gameConfig["game"]["field"]["right_edge"].get<int>())
    {
        state["ball_position_x"] = gameConfig["game"]["field"]["right_edge"].get<int>();
    }

    // Keep ball below the top wall.
    if (state["ball_position_y"] < gameConfig["game"]["field"]["top"].get<int>())
    {
        state["ball_position_y"] = gameConfig["game"]["field"]["top"].get<int>();
    }

    // Player paddle collision.
    bool playerPaddleCollision = Collision::CheckRectangle(
        Position(state["ball_position_x"], state["ball_position_y"]),
        Position(state["player_paddle_x"], gameConfig["game"]["paddle"]["player_y"].get<int>()),
        gameConfig["game"]["paddle"]["width"].get<int>(),
        gameConfig["game"]["paddle"]["height"].get<int>()
    );

    if (state["ball_velocity_y"] > 0 && playerPaddleCollision)
    {
        state["ball_velocity_y"] = gameConfig["game"]["collision"]["paddle_bounce_velocity_y"].get<int>();

        // Calculate where the ball hits the paddle.
        int hitPosition = state["ball_position_x"] - state["player_paddle_x"];

        // Each space on the paddle sends the ball off at its own angle, the ends are the steepest.
        const auto& bounceVelocities = gameConfig["game"]["collision"]["paddle_bounce_velocity_x"];

        if (hitPosition >= 0 && hitPosition < static_cast<int>(bounceVelocities.size()))
        {
            state["ball_velocity_x"] = bounceVelocities[hitPosition].get<int>();
        }
    }

    // Brick collision, find how many hits the brick under the ball has left.
    const auto& bricks = gameConfig["game"]["bricks"];

    int brickRow = state["ball_position_y"] - bricks["origin_y"].get<int>();
    int brickColumn = state["ball_position_x"] - bricks["origin_x"].get<int>();
    int brickHits = 0;

    bool brickAreaCollision = Collision::CheckRectangle(
        Position(state["ball_position_x"], state["ball_position_y"]),
        Position(bricks["origin_x"].get<int>(), bricks["origin_y"].get<int>()),
        bricks["width"].get<int>(),
        static_cast<int>(brickRows.size())
    );

    if (brickAreaCollision)
    {
        brickHits = brickRows[brickRow][brickColumn] - EMPTY_BRICK;
    }

    // Store updated ball velocity.
    runtime.SetStateValue("ball_velocity_x", state["ball_velocity_x"]);
    runtime.SetStateValue("ball_velocity_y", state["ball_velocity_y"]);

    // Store updated game values.
    runtime.SetStateValue("ball_position_x", state["ball_position_x"]);
    runtime.SetStateValue("ball_position_y", state["ball_position_y"]);
    runtime.SetStateValue("brick_hits", brickHits);

    // Evaluate registered gameplay rules.
    runtime.Update();

    // The rules take a hit off the brick, put what is left back in the brick rows.
    if (brickAreaCollision)
    {
        runtime.GetStateValue("brick_hits", brickHits);

        brickRows[brickRow][brickColumn] = static_cast<char>(EMPTY_BRICK + brickHits);
    }
}

void BRICKBREAKER::RenderGame()
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
        gameConfig["game"]["rendering"]["scoreboard"]["bricks_label"].get<std::string>() + " " + std::to_string(state["bricks_remaining"]));

    const auto& ascii = gameConfig["game"]["rendering"]["ascii"];

    DrawBoundary(objects);

    // Player paddle.
    for (int j = 0; j < gameConfig["game"]["paddle"]["height"].get<int>(); j++)
    {
        for (int i = 0; i < gameConfig["game"]["paddle"]["width"].get<int>(); i++)
        {
            objects.push_back({
                Position(state["player_paddle_x"] + i, gameConfig["game"]["paddle"]["player_y"].get<int>() + j),
                ascii["paddle"].get<std::string>()[0],
                gameConfig["game"]["rendering"]["roles"]["paddle"].get<std::string>()
                });
        }
    }

    // Ball.
    objects.push_back({ Position(state["ball_position_x"], state["ball_position_y"]),
        ascii["ball"].get<std::string>()[0],
        gameConfig["game"]["rendering"]["roles"]["ball"].get<std::string>() });

    // Brick rows, drawn straight into the frame like lines with gaps where bricks are gone.
    const auto& bricks = gameConfig["game"]["bricks"];

    for (int row = 0; row < brickRows.size(); row++)
    {
        for (int i = 0; i < brickRows[row].size(); i++)
        {
            if (brickRows[row][i] > EMPTY_BRICK)
            {
                int hitsRemaining = brickRows[row][i] - EMPTY_BRICK;

                std::string brickRole = gameConfig["game"]["rendering"]["roles"]
                    ["brick_" + std::to_string(hitsRemaining)].get<std::string>();

                render.DrawCharacter(bricks["origin_x"].get<int>() + i,
                    bricks["origin_y"].get<int>() + row + gameConfig["game"]["rendering"]["world_vertical_offset"].get<int>(),
                    static_cast<char>(ascii["brick"].get<int>()),
                    brickRole);
            }
        }
    }

    // Submit objects to the renderer.
    render.DrawObjects(objects);

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


void BRICKBREAKER::CheckGameOver()//checks whether the game has ended
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