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

struct BrickBreakerState
{
    int ballPosX;
    int ballPosY;
    int ballVelX;
    int ballVelY;
    int playerPaddleX;
    int score;
    int bricksRemaining;
    int gameWon;
    int gameLost;
};


BRICKBREAKER::BRICKBREAKER(ThemeManager& themeManager)//constructor, connects BRICKBREAKER to ThemeManager
    : themeManager(themeManager), render(themeManager)
{
    if (!fileBoss.ReadJSON("brickbreaker.json", brickBreakerConfig))
    {
        throw std::runtime_error("Failed to load brickbreaker.json");
    }
    render.SetConfiguration(brickBreakerConfig["game"]["rendering"]);
}

void BRICKBREAKER::Run()//controls the game loop
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
        //This prevents the game from running as fast as the processor allows.
        std::this_thread::sleep_for(std::chrono::milliseconds(brickBreakerConfig["game"]["timing"]["frame_delay_ms"].get<int>()));
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

void BRICKBREAKER::InitializeGame()
{
    const auto& initialState = brickBreakerConfig["game"]["initial_state"];

    runtime.SetStateValue("ball_position_x", initialState["ball"]["position"]["x"].get<int>());
    runtime.SetStateValue("ball_position_y", initialState["ball"]["position"]["y"].get<int>());

    runtime.SetStateValue("ball_velocity_x", initialState["ball"]["velocity"]["x"].get<int>());
    runtime.SetStateValue("ball_velocity_y", initialState["ball"]["velocity"]["y"].get<int>());

    runtime.SetStateValue("player_paddle_x", initialState["paddle"]["player_x"].get<int>());

    runtime.SetStateValue("score", initialState["score"].get<int>());

    runtime.SetStateValue("game_won", initialState["game_won"].get<int>());
    runtime.SetStateValue("game_lost", initialState["game_lost"].get<int>());

    // The bricks are rows of characters, each character is how many hits that brick has left.
    const auto& bricks = brickBreakerConfig["game"]["bricks"];
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


void BRICKBREAKER::SetupRules()
{
    runtime.LoadRules(brickBreakerConfig["game"]["rules"]);
}

void BRICKBREAKER::HandleInput()// Processes player keyboard input.
{
    double playerPaddle = 0;

    runtime.GetStateValue("player_paddle_x", playerPaddle);//find position

    // Move paddle continuously while A or D is held.
    if (InputHelper::IsKeyDown(brickBreakerConfig["game"]["controls"]["left"].get<int>()))
    {
        playerPaddle -= brickBreakerConfig["game"]["paddle"]["movement_step"].get<int>();
    }

    if (InputHelper::IsKeyDown(brickBreakerConfig["game"]["controls"]["right"].get<int>()))
    {
        playerPaddle += brickBreakerConfig["game"]["paddle"]["movement_step"].get<int>();
    }

    // Keep the paddle inside the field, the movement step can be larger than one.
    if (playerPaddle < brickBreakerConfig["game"]["paddle"]["left_limit"].get<int>())
    {
        playerPaddle = brickBreakerConfig["game"]["paddle"]["left_limit"].get<int>();
    }
    else if (playerPaddle > brickBreakerConfig["game"]["paddle"]["right_limit"].get<int>())
    {
        playerPaddle = brickBreakerConfig["game"]["paddle"]["right_limit"].get<int>();
    }

    // Exit the current game.
    if (InputHelper::IsKeyDown(brickBreakerConfig["game"]["controls"]["exit"].get<int>()))
    {
        running = false;
    }

    runtime.SetStateValue("player_paddle_x", playerPaddle);
}

static BrickBreakerState LoadGameState(WonkleRuntime& runtime)
{
    BrickBreakerState state{};

    runtime.GetStateValue("ball_position_x", state.ballPosX);
    runtime.GetStateValue("ball_position_y", state.ballPosY);
    runtime.GetStateValue("ball_velocity_x", state.ballVelX);
    runtime.GetStateValue("ball_velocity_y", state.ballVelY);
    runtime.GetStateValue("player_paddle_x", state.playerPaddleX);
    runtime.GetStateValue("score", state.score);
    runtime.GetStateValue("bricks_remaining", state.bricksRemaining);
    runtime.GetStateValue("game_won", state.gameWon);
    runtime.GetStateValue("game_lost", state.gameLost);

    return state;
}


void BRICKBREAKER::UpdateGame()// Advances the game state.
{
    // Retrieve current game state.
    BrickBreakerState state = LoadGameState(runtime);

    // Move the ball using its current velocity.
    state.ballPosX += state.ballVelX;
    state.ballPosY += state.ballVelY;

    // Check for boundary collisions.
    Boundary boundary(
        brickBreakerConfig["game"]["field"]["width"].get<int>(),
        brickBreakerConfig["game"]["field"]["height"].get<int>(),
        brickBreakerConfig["game"]["field"]["left_edge"].get<int>(),
        brickBreakerConfig["game"]["field"]["top"].get<int>(),
        brickBreakerConfig["game"]["field"]["right_edge"].get<int>(),
        brickBreakerConfig["game"]["field"]["bottom"].get<int>()
    );

    BoundarySide collision = Collision::CheckBoundary(Position(state.ballPosX, state.ballPosY), boundary);

    //the ball only bounces off a wall it is moving toward, so it cannot get stuck on one
    if (collision == BoundarySide::Left && state.ballVelX < 0)
    {
        state.ballVelX *= brickBreakerConfig["game"]["collision"]["horizontal_boundary_bounce_multiplier"].get<int>();
    }
    else if (collision == BoundarySide::Right && state.ballVelX > 0)
    {
        state.ballVelX *= brickBreakerConfig["game"]["collision"]["horizontal_boundary_bounce_multiplier"].get<int>();
    }
    else if (collision == BoundarySide::Top && state.ballVelY < 0)
    {
        state.ballVelY *= brickBreakerConfig["game"]["collision"]["vertical_boundary_bounce_multiplier"].get<int>();
    }
    //the bottom is open, a runtime rule decides when the ball is lost

    // Keep ball inside the side walls.
    if (state.ballPosX < brickBreakerConfig["game"]["field"]["left_edge"].get<int>())
    {
        state.ballPosX = brickBreakerConfig["game"]["field"]["left_edge"].get<int>();
    }
    else if (state.ballPosX > brickBreakerConfig["game"]["field"]["right_edge"].get<int>())
    {
        state.ballPosX = brickBreakerConfig["game"]["field"]["right_edge"].get<int>();
    }

    // Keep ball below the top wall.
    if (state.ballPosY < brickBreakerConfig["game"]["field"]["top"].get<int>())
    {
        state.ballPosY = brickBreakerConfig["game"]["field"]["top"].get<int>();
    }

    // Player paddle collision.
    bool playerPaddleCollision = Collision::CheckRectangle(
        Position(state.ballPosX, state.ballPosY),
        Position(state.playerPaddleX, brickBreakerConfig["game"]["paddle"]["player_y"].get<int>()),
        brickBreakerConfig["game"]["paddle"]["width"].get<int>(),
        brickBreakerConfig["game"]["paddle"]["height"].get<int>()
    );

    if (state.ballVelY > 0 && playerPaddleCollision)
    {
        state.ballVelY = brickBreakerConfig["game"]["collision"]["paddle_bounce_velocity_y"].get<int>();

        // Calculate where the ball hits the paddle.
        int hitPosition = state.ballPosX - state.playerPaddleX;

        // Each space on the paddle sends the ball off at its own angle, the ends are the steepest.
        const auto& bounceVelocities = brickBreakerConfig["game"]["collision"]["paddle_bounce_velocity_x"];

        if (hitPosition >= 0 && hitPosition < static_cast<int>(bounceVelocities.size()))
        {
            state.ballVelX = bounceVelocities[hitPosition].get<int>();
        }
    }

    // Brick collision, find how many hits the brick under the ball has left.
    const auto& bricks = brickBreakerConfig["game"]["bricks"];

    int brickRow = state.ballPosY - bricks["origin_y"].get<int>();
    int brickColumn = state.ballPosX - bricks["origin_x"].get<int>();
    int brickHits = 0;

    bool brickAreaCollision = Collision::CheckRectangle(
        Position(state.ballPosX, state.ballPosY),
        Position(bricks["origin_x"].get<int>(), bricks["origin_y"].get<int>()),
        bricks["width"].get<int>(),
        static_cast<int>(brickRows.size())
    );

    if (brickAreaCollision)
    {
        brickHits = brickRows[brickRow][brickColumn] - EMPTY_BRICK;
    }

    // Store updated ball velocity.
    runtime.SetStateValue("ball_velocity_x", state.ballVelX);
    runtime.SetStateValue("ball_velocity_y", state.ballVelY);

    // Store updated game values.
    runtime.SetStateValue("ball_position_x", state.ballPosX);
    runtime.SetStateValue("ball_position_y", state.ballPosY);
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
    BrickBreakerState state = LoadGameState(runtime);

    render.SetBackground();

    // Define game objects.
    std::vector<RenderObject> objects;

    // Begin a new frame.
    render.BeginFrame(brickBreakerConfig["game"]["field"]["width"].get<int>(),
        brickBreakerConfig["game"]["field"]["height"].get<int>() + brickBreakerConfig["game"]["rendering"]["extra_height"].get<int>(),
        brickBreakerConfig["game"]["rendering"]["roles"]["background"].get<std::string>(),
        brickBreakerConfig["game"]["rendering"]["world_vertical_offset"].get<int>());

    // Draw scoreboard.
    render.DrawText(brickBreakerConfig["game"]["rendering"]["scoreboard_column"].get<int>(),
        brickBreakerConfig["game"]["rendering"]["scoreboard_row"].get<int>(),
        brickBreakerConfig["game"]["rendering"]["scoreboard"]["score_label"].get<std::string>() + " " + std::to_string(state.score) +
        brickBreakerConfig["game"]["rendering"]["scoreboard"]["separator"].get<std::string>() +
        brickBreakerConfig["game"]["rendering"]["scoreboard"]["bricks_label"].get<std::string>() + " " + std::to_string(state.bricksRemaining),
        brickBreakerConfig["game"]["rendering"]["roles"]["ui"].get<std::string>());

    Boundary boundary(
        brickBreakerConfig["game"]["field"]["width"].get<int>(),
        brickBreakerConfig["game"]["field"]["height"].get<int>(),
        brickBreakerConfig["game"]["field"]["left_edge"].get<int>(),
        brickBreakerConfig["game"]["field"]["top"].get<int>(),
        brickBreakerConfig["game"]["field"]["right_edge"].get<int>(),
        brickBreakerConfig["game"]["field"]["bottom"].get<int>());

    const auto& ascii = brickBreakerConfig["game"]["rendering"]["ascii"];

    boundary.Draw(render, objects,
        ascii["top_border"].get<std::string>()[0],
        ascii["bottom_border"].get<std::string>()[0],
        ascii["vertical_border"].get<std::string>()[0],
        brickBreakerConfig["game"]["rendering"]["roles"]["border"].get<std::string>()
    );

    // Player paddle.
    for (int j = 0; j < brickBreakerConfig["game"]["paddle"]["height"].get<int>(); j++)
    {
        for (int i = 0; i < brickBreakerConfig["game"]["paddle"]["width"].get<int>(); i++)
        {
            objects.push_back({
                Position(state.playerPaddleX + i, brickBreakerConfig["game"]["paddle"]["player_y"].get<int>() + j),
                ascii["paddle"].get<std::string>()[0],
                brickBreakerConfig["game"]["rendering"]["roles"]["paddle"].get<std::string>()
                });
        }
    }

    // Ball.
    objects.push_back({ Position(state.ballPosX, state.ballPosY),
        ascii["ball"].get<std::string>()[0],
        brickBreakerConfig["game"]["rendering"]["roles"]["ball"].get<std::string>() });

    // Brick rows, drawn straight into the frame like lines with gaps where bricks are gone.
    const auto& bricks = brickBreakerConfig["game"]["bricks"];

    for (int row = 0; row < brickRows.size(); row++)
    {
        for (int i = 0; i < brickRows[row].size(); i++)
        {
            if (brickRows[row][i] > EMPTY_BRICK)
            {
                render.DrawCharacter(bricks["origin_x"].get<int>() + i,
                    bricks["origin_y"].get<int>() + row + brickBreakerConfig["game"]["rendering"]["world_vertical_offset"].get<int>(),
                    static_cast<char>(ascii["brick"].get<int>()),
                    brickBreakerConfig["game"]["rendering"]["roles"]["brick"].get<std::string>());
            }
        }
    }

    // Submit objects to the renderer.
    render.DrawObjects(objects);

    // Draw controls, or the result once the game has ended.
    std::string bottomText = brickBreakerConfig["game"]["controls"]["display_text"].get<std::string>();

    if (state.gameWon != 0 || state.gameLost != 0)
    {
        const auto& messages = brickBreakerConfig["game"]["rendering"]["messages"];

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

    render.DrawText(brickBreakerConfig["game"]["rendering"]["controls_column"].get<int>(),
        brickBreakerConfig["game"]["field"]["height"].get<int>() + brickBreakerConfig["game"]["rendering"]["controls_row_offset"].get<int>(),
        bottomText,
        brickBreakerConfig["game"]["rendering"]["roles"]["ui"].get<std::string>());

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

        if (std::toupper(static_cast<unsigned char>(key)) == brickBreakerConfig["game"]["controls"]["restart"].get<int>())
        {
            // Restart, the rules stay the same so only the state is reset.
            InitializeGame();
            answered = true;
        }
        else if (key == brickBreakerConfig["game"]["controls"]["exit"].get<int>())
        {
            running = false;
            answered = true;
        }
    }
}