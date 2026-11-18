#include "GameObjects.h"
#include "InputHelper.h"
#include "Collision.h"
#include <algorithm>

constexpr char EMPTY_BRICK = '0';  //a space in a brick row with no brick in it, the other digits are hits left

constexpr int OUTSIDE_GRID = -1;   //food position while there is no food on the field, the eats_food rule sets it
constexpr int NO_TICKS = 0;        //a grid space the snake is not on


GameObjects::GameObjects(ThemeManager& themeManager, const std::string& fileName)//constructor, hands the theme and the game's json to GameController
    : GameController(themeManager, fileName), randomGenerator(std::random_device{}())
{
}

bool GameObjects::Uses(const std::string& object)
{
    return gameConfig["game"].contains(object);
}

//setup
void GameObjects::InitializeGame()
{
    // Every starting value comes from initial_state in the game's json.
    SetInitialState();

    if(Uses("bricks"))
    {
        SetupBricks();
    }

    if(Uses("snake"))
    {
        SetupSnake();
    }

    // Movement timing starts over.
    elapsedMilliseconds = 0;
    lastFrameTime = std::chrono::steady_clock::now();
}


//game systems
void GameObjects::HandleInput()// Processes player keyboard input.
{
    if(Uses("paddles"))
    {
        for (const auto& paddle : gameConfig["game"]["paddles"])
        {
            MovePaddle(paddle);

            // A paddle with a second axis moves on that one the same way.
            if(paddle.contains("other_axis"))
            {
                MovePaddle(paddle["other_axis"]);
            }
        }
    }

    if(Uses("snake"))
    {
        TurnSnake();
    }
}

void GameObjects::UpdateGame()// Advances the game state.
{
    if(!UpdateIsDue())
    {
        return;
    }

    // Retrieve current game state.
    std::map<std::string, int> state = LoadGameState();

    if(Uses("ball"))
    {
        MoveBall(state);
    }

    if(Uses("paddles"))
    {
        for (const auto& paddle : gameConfig["game"]["paddles"])
        {
            if(Uses("ball"))
            {
                BounceOffPaddle(state, paddle);
            }

            // Keep the paddle inside its limits.
            int& position = state[paddle["position"].get<std::string>()];

            position = std::clamp(position, paddle["limits"][0].get<int>(), paddle["limits"][1].get<int>());
        }
    }

    if(Uses("bricks"))
    {
        FindBrickHit(state);
    }

    if(Uses("snake"))
    {
        MoveSnake(state);
    }

    // Store updated game values.
    SaveGameState(state);

    // Evaluate registered gameplay rules.
    runtime.Update();

    if(Uses("bricks"))
    {
        StoreBrickHit();
    }

    if(Uses("snake"))
    {
        GrowSnakeAndPlaceFood(state);
    }
}

bool GameObjects::UpdateIsDue()
{
    const auto& timing = gameConfig["game"]["timing"];

    // Without a movement interval the game updates every frame.
    if(!timing.contains("movement_interval_ms"))
    {
        return true;
    }

    // Add the time that has passed since the last frame.
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

    elapsedMilliseconds += std::chrono::duration<double, std::milli>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;

    double movementInterval = timing["movement_interval_ms"].get<double>();

    // The game only moves once enough time has built up.
    if(elapsedMilliseconds < movementInterval)
    {
        return false;
    }

    elapsedMilliseconds -= movementInterval;

    // After a long stall do not make several moves in a row to catch up.
    if(elapsedMilliseconds >= movementInterval)
    {
        elapsedMilliseconds = 0;
    }

    return true;
}

void GameObjects::RenderGame()
{
    // Retrieve current game state.
    std::map<std::string, int> state = LoadGameState();

    const auto& scoreboard = gameConfig["game"]["rendering"]["scoreboard"];

    // Begin a new frame.
    BeginFrame();

    // Draw scoreboard, each entry is a label and the value it shows.
    std::string scoreboardText;

    for (const auto& entry : scoreboard["entries"])
    {
        if(!scoreboardText.empty())
        {
            scoreboardText += scoreboard["separator"].get<std::string>();
        }

        scoreboardText += entry["label"].get<std::string>() + " " + std::to_string(state[entry["value"].get<std::string>()]);
    }

    DrawScoreboard(scoreboardText);

    if(Uses("paddles"))
    {
        for (const auto& paddle : gameConfig["game"]["paddles"])
        {
            AddPaddle(state, paddle);
        }
    }

    if(Uses("ball"))
    {
        AddRectangle("ball", state["ball_position_x"], state["ball_position_y"]);
    }

    if(Uses("bricks"))
    {
        DrawBricks();
    }

    if(Uses("snake"))
    {
        DrawSnake(state);
    }

    // Text is added last so it is on top of the paddles and the boundary.
    if(Uses("text"))
    {
        DrawFieldText(state);
    }

    // Draw everything and present the frame.
    EndFrame();
}


//ball
void GameObjects::MoveBall(std::map<std::string, int>& state)
{
    const auto& ball = gameConfig["game"]["ball"];
    const auto& field = gameConfig["game"]["field"];

    // Move the ball using its current velocity.
    state["ball_position_x"] += state["ball_velocity_x"];
    state["ball_position_y"] += state["ball_velocity_y"];

    // Check for boundary collisions.
    Boundary boundary = GetBoundary();

    BoundarySide collision = Collision::CheckBoundary(Position(state["ball_position_x"], state["ball_position_y"]), boundary);

    //the ball only bounces off a wall the game lists and that it is moving toward, so it cannot get stuck on one
    for (const auto& wall : ball["bounces_off"])
    {
        if((wall == "left" && collision == BoundarySide::Left && state["ball_velocity_x"] < 0) ||
            (wall == "right" && collision == BoundarySide::Right && state["ball_velocity_x"] > 0))
        {
            state["ball_velocity_x"] *= ball["bounce_multiplier"].get<int>();
        }
        else if((wall == "top" && collision == BoundarySide::Top && state["ball_velocity_y"] < 0) ||
            (wall == "bottom" && collision == BoundarySide::Bottom && state["ball_velocity_y"] > 0))
        {
            state["ball_velocity_y"] *= ball["bounce_multiplier"].get<int>();
        }
    }

    // Keep ball inside the walls the game lists.
    if(ball.contains("keep_inside"))
    {
        for (const auto& wall : ball["keep_inside"])
        {
            if(wall == "left")
            {
                state["ball_position_x"] = std::max(state["ball_position_x"], field["left_edge"].get<int>());
            }
            else if(wall == "right")
            {
                state["ball_position_x"] = std::min(state["ball_position_x"], field["right_edge"].get<int>());
            }
            else if(wall == "top")
            {
                state["ball_position_y"] = std::max(state["ball_position_y"], field["top"].get<int>());
            }
            else if(wall == "bottom")
            {
                state["ball_position_y"] = std::min(state["ball_position_y"], field["bottom"].get<int>());
            }
        }
    }
}


//paddle
void GameObjects::MovePaddle(const nlohmann::json& paddle)
{
    // A paddle with no keys is moved by the rules, like the opponent in Pong.
    if(!paddle.contains("keys"))
    {
        return;
    }

    int position = 0;

    runtime.GetStateValue(paddle["position"].get<std::string>(), position);//find position

    // Move paddle continuously while a key is held, stopping at its limits.
    if(InputHelper::IsKeyDown(paddle["keys"]["back"].get<int>()))
    {
        position = std::max(position - paddle["step"].get<int>(), paddle["limits"][0].get<int>());
    }

    if(InputHelper::IsKeyDown(paddle["keys"]["forward"].get<int>()))
    {
        position = std::min(position + paddle["step"].get<int>(), paddle["limits"][1].get<int>());
    }

    runtime.SetStateValue(paddle["position"].get<std::string>(), position);
}

int GameObjects::OtherAxisPosition(std::map<std::string, int>& state, const nlohmann::json& paddle)
{
    // A paddle that moves on both axes has a second position, the rest sit at a fixed spot.
    if(paddle.contains("other_axis"))
    {
        return state[paddle["other_axis"]["position"].get<std::string>()];
    }

    return paddle["fixed_at"].get<int>();
}

void GameObjects::BounceOffPaddle(std::map<std::string, int>& state, const nlohmann::json& paddle)
{
    // A paddle moves along one axis and sits at a fixed spot on the other.
    bool movesAlongY = paddle["moves_along"] == "y";

    std::string alongAxis = movesAlongY ? "y" : "x";
    std::string acrossAxis = movesAlongY ? "x" : "y";

    int position = state[paddle["position"].get<std::string>()];
    int paddleX = movesAlongY ? OtherAxisPosition(state, paddle) : position;
    int paddleY = movesAlongY ? position : OtherAxisPosition(state, paddle);

    bool paddleCollision = Collision::CheckRectangle(
        Position(state["ball_position_x"], state["ball_position_y"]),
        Position(paddleX, paddleY),
        paddle["width"].get<int>(),
        paddle["height"].get<int>()
    );

    // The ball only bounces off a paddle it is moving toward.
    int bounceVelocity = paddle["bounce_velocity"].get<int>();
    int ballVelocity = state["ball_velocity_" + acrossAxis];

    bool movingToward = (bounceVelocity > 0 && ballVelocity < 0) || (bounceVelocity < 0 && ballVelocity > 0);

    if(!paddleCollision || !movingToward)
    {
        return;
    }

    state["ball_velocity_" + acrossAxis] = bounceVelocity;

    // Calculate where the ball hits the paddle.
    int hitPosition = state["ball_position_" + alongAxis] - position;

    // Each space on the paddle sends the ball off at its own angle, null keeps the angle it had.
    const auto& bounceAngles = paddle["bounce_angles"];

    if(hitPosition >= 0 && hitPosition < bounceAngles.size() && !bounceAngles[hitPosition].is_null())
    {
        state["ball_velocity_" + alongAxis] = bounceAngles[hitPosition].get<int>();
    }
}

void GameObjects::AddPaddle(std::map<std::string, int>& state, const nlohmann::json& paddle)
{
    bool movesAlongY = paddle["moves_along"] == "y";

    int position = state[paddle["position"].get<std::string>()];
    int paddleX = movesAlongY ? OtherAxisPosition(state, paddle) : position;
    int paddleY = movesAlongY ? position : OtherAxisPosition(state, paddle);

    // A paddle can be drawn narrower than the space the ball can hit, and as something other than a paddle.
    AddRectangle(paddle.value("draw_as", "paddle"), paddleX, paddleY,
        paddle.value("draw_width", paddle["width"].get<int>()),
        paddle["height"].get<int>());
}


//bricks 
void GameObjects::SetupBricks()
{
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
            if(row[i] > EMPTY_BRICK)
            {
                brickCount++;
            }
        }

        brickRows.push_back(row);
    }

    runtime.SetStateValue("bricks_remaining", brickCount);
}

void GameObjects::FindBrickHit(std::map<std::string, int>& state)
{
    const auto& bricks = gameConfig["game"]["bricks"];

    // The ball hits the bricks unless the game's json names another position, the player in a maze for example.
    int hitX = state["ball_position_x"];
    int hitY = state["ball_position_y"];

    if(bricks.contains("hit_by"))
    {
        hitX = state[bricks["hit_by"]["x"].get<std::string>()];
        hitY = state[bricks["hit_by"]["y"].get<std::string>()];
    }

    // Brick collision, find how many hits the brick under that position has left.
    brickRow = hitY - bricks["origin_y"].get<int>();
    brickColumn = hitX - bricks["origin_x"].get<int>();

    onBricks = Collision::CheckRectangle(
        Position(hitX, hitY),
        Position(bricks["origin_x"].get<int>(), bricks["origin_y"].get<int>()),
        bricks["width"].get<int>(),
        static_cast<int>(brickRows.size())
    );

    state["brick_hits"] = 0;

    if(onBricks)
    {
        state["brick_hits"] = brickRows[brickRow][brickColumn] - EMPTY_BRICK;
    }
}

void GameObjects::StoreBrickHit()
{
    // The rules take a hit off the brick, put what is left back in the brick rows.
    if(onBricks)
    {
        int brickHits = 0;

        runtime.GetStateValue("brick_hits", brickHits);

        brickRows[brickRow][brickColumn] = static_cast<char>(EMPTY_BRICK + brickHits);
    }
}

void GameObjects::DrawBricks()
{
    const auto& bricks = gameConfig["game"]["bricks"];
    const auto& rendering = gameConfig["game"]["rendering"];

    // Brick rows, drawn straight into the frame like lines with gaps where bricks are gone.
    for (int row = 0; row < brickRows.size(); row++)
    {
        for (int i = 0; i < brickRows[row].size(); i++)
        {
            if(brickRows[row][i] > EMPTY_BRICK)
            {
                int hitsRemaining = brickRows[row][i] - EMPTY_BRICK;
                std::string brickRole = rendering["roles"]["brick_" + std::to_string(hitsRemaining)].get<std::string>();
                
                render.DrawCharacter(bricks["origin_x"].get<int>() + i,
                    bricks["origin_y"].get<int>() + row + rendering["world_vertical_offset"].get<int>(),
                    static_cast<char>(rendering["ascii"]["brick"].get<int>()),
                    brickRole);
            }
        }
    }
}


//snake
void GameObjects::SetupSnake()
{
    std::map<std::string, int> state = LoadGameState();

    const auto& snake = gameConfig["game"]["snake"];

    // The body is a tick counter on every grid space, a space is part of the snake while its counter is above zero.
    bodyTicks.assign(snake["rows"].get<int>(), std::vector<int>(snake["columns"].get<int>(), NO_TICKS));

    // The starting body lines up behind the head, counting down toward the tail.
    for (int i = 1; i < state["snake_length"]; i++)
    {
        bodyTicks[state["head_y"] - i * state["direction_y"]][state["head_x"] - i * state["direction_x"]] = state["snake_length"] - i;
    }
}

void GameObjects::TurnSnake()
{
    const auto& keys = gameConfig["game"]["snake"]["keys"];

    // Input only records the direction the player wants, the snake turns on its next move.
    if(InputHelper::IsKeyDown(keys["up"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", 0);
        runtime.SetStateValue("requested_direction_y", -1);
    }
    else if(InputHelper::IsKeyDown(keys["down"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", 0);
        runtime.SetStateValue("requested_direction_y", 1);
    }
    else if(InputHelper::IsKeyDown(keys["left"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", -1);
        runtime.SetStateValue("requested_direction_y", 0);
    }
    else if(InputHelper::IsKeyDown(keys["right"].get<int>()))
    {
        runtime.SetStateValue("requested_direction_x", 1);
        runtime.SetStateValue("requested_direction_y", 0);
    }
}

void GameObjects::MoveSnake(std::map<std::string, int>& state)// Makes one move.
{
    const auto& snake = gameConfig["game"]["snake"];

    // Turn, unless the player asked to go straight back the way the snake came.
    bool reversing = state["requested_direction_x"] == -state["direction_x"] && state["requested_direction_y"] == -state["direction_y"];

    if(!reversing)
    {
        state["direction_x"] = state["requested_direction_x"];
        state["direction_y"] = state["requested_direction_y"];
    }

    // Every space the snake is on counts down, when a space reaches zero the tail has left it.
    ChangeBodyTicks(-1);

    // The head leaves a counter behind in the space it is leaving, it lasts until the tail passes.
    bodyTicks[state["head_y"]][state["head_x"]] = state["snake_length"] - 1;

    // Move the head.
    state["head_x"] += state["direction_x"];
    state["head_y"] += state["direction_y"];

    // Find the counter under the head, anything above zero is the snake's own body.
    state["body_under_head"] = NO_TICKS;

    if(Collision::CheckRectangle(Position(state["head_x"], state["head_y"]), Position(0, 0), snake["columns"].get<int>(), snake["rows"].get<int>()))
    {
        state["body_under_head"] = bodyTicks[state["head_y"]][state["head_x"]];
    }
}

void GameObjects::GrowSnakeAndPlaceFood(std::map<std::string, int>& state)
{
    const auto& snake = gameConfig["game"]["snake"];

    // Retrieve values after runtime rules have been applied.
    std::map<std::string, int> newState = LoadGameState();

    // if the rules made the snake longer, every counter lasts one more move.
    if(newState["snake_length"] > state["snake_length"])
    {
        ChangeBodyTicks(1);
    }

    // With no food on the field, try one random space per move and use it if the snake is not on it.
    if(newState["food_x"] == OUTSIDE_GRID)
    {
        std::uniform_int_distribution<int> randomColumn(0, snake["columns"].get<int>() - 1);
        std::uniform_int_distribution<int> randomRow(0, snake["rows"].get<int>() - 1);

        int candidateX = randomColumn(randomGenerator);
        int candidateY = randomRow(randomGenerator);

        bool onHead = Collision::CheckPosition(Position(candidateX, candidateY), Position(state["head_x"], state["head_y"]));

        if(!onHead && bodyTicks[candidateY][candidateX] == NO_TICKS)
        {
            runtime.SetStateValue("food_x", candidateX);
            runtime.SetStateValue("food_y", candidateY);
        }
    }
}

// Adds to every counter that is above zero, spaces the snake is not on stay at zero.
void GameObjects::ChangeBodyTicks(int amount)
{
    for (int row = 0; row < bodyTicks.size(); row++)
    {
        for (int column = 0; column < bodyTicks[row].size(); column++)
        {
            if(bodyTicks[row][column] > NO_TICKS)
            {
                bodyTicks[row][column] += amount;
            }
        }
    }
}

void GameObjects::DrawSnake(std::map<std::string, int>& state)
{
    // Food.
    DrawCell("food", state["food_x"], state["food_y"]);

    // Snake body, every space whose counter has not run out.
    for (int row = 0; row < bodyTicks.size(); row++)
    {
        for (int column = 0; column < bodyTicks[row].size(); column++)
        {
            if(bodyTicks[row][column] > NO_TICKS)
            {
                DrawCell("snake_body", column, row);
            }
        }
    }

    // Snake head, drawn last so it is always on top.
    DrawCell("snake_head", state["head_x"], state["head_y"]);
}

// Draws one grid space with a named character, a space is more than one character wide so it looks square.
void GameObjects::DrawCell(const std::string& name, int cellX, int cellY)
{
    const auto& snake = gameConfig["game"]["snake"];
    const auto& rendering = gameConfig["game"]["rendering"];

    // Spaces outside the grid are not drawn, this keeps the head off the border when it hits a wall.
    if(!Collision::CheckRectangle(Position(cellX, cellY), Position(0, 0), snake["columns"].get<int>(), snake["rows"].get<int>()))
    {
        return;
    }

    for (int i = 0; i < snake["cell_width"].get<int>(); i++)
    {
        render.DrawCharacter(snake["origin_x"].get<int>() + cellX * snake["cell_width"].get<int>() + i,
            cellY + rendering["world_vertical_offset"].get<int>(),
            static_cast<char>(rendering["ascii"][name].get<int>()),
            rendering["roles"][name].get<std::string>());
    }
}


//text
void GameObjects::DrawFieldText(std::map<std::string, int>& state)
{
    // Each entry is a position, a color role, and either fixed text or the state value to show.
    for (const auto& entry : gameConfig["game"]["text"])
    {
        // An entry with a "when" value is only shown while that value is not zero, the rules switch it on and off.
        if(entry.contains("when") && state[entry["when"].get<std::string>()] == 0)
        {
            continue;
        }

        std::string text;

        if(entry.contains("value"))
        {
            text = std::to_string(state[entry["value"].get<std::string>()]);
        }
        else
        {
            text = entry["text"].get<std::string>();
        }

        AddText(TextPosition(state, entry["x"]), TextPosition(state, entry["y"]), text, entry["role"].get<std::string>());
    }
}

int GameObjects::TextPosition(std::map<std::string, int>& state, const nlohmann::json& position)
{
    // A position written as { "state": "name" } follows that state value, the same way a rule's value does.
    if(position.is_object())
    {
        return state[position["state"].get<std::string>()];
    }

    return position.get<int>();
}
