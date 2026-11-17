#pragma once
#include "GameController.h"
#include <chrono>
#include <random>
#include <string>
#include <vector>

class GameObjects : public GameController
{
public:
    GameObjects(ThemeManager& themeManager, const std::string& fileName);

private:
    // Game setup
    void InitializeGame() override;//sets the starting state for the objects the game uses

    // Game systems
    void HandleInput() override;//proccesses input
    void UpdateGame() override;//advances the gamestate
    void RenderGame() override;///shows it in console

    bool Uses(const std::string& object);//true when the game's json has a section for this object
    bool UpdateIsDue();//games with a movement interval only update when it has passed

    // Ball
    void MoveBall(std::map<std::string, int>& state);//moves the ball and bounces it off the walls

    // Paddles
    void MovePaddle(const nlohmann::json& paddle);//moves one paddle with its keys
    int OtherAxisPosition(std::map<std::string, int>& state, const nlohmann::json& paddle);//where a paddle is on the axis it does not move along
    void BounceOffPaddle(std::map<std::string, int>& state, const nlohmann::json& paddle);//ball against one paddle
    void AddPaddle(std::map<std::string, int>& state, const nlohmann::json& paddle);//adds one paddle to the frame

    // Bricks
    void SetupBricks();//builds the brick rows from the layout
    void FindBrickHit(std::map<std::string, int>& state);//hands the rules the brick the ball is on, or the position the json names
    void StoreBrickHit();//puts back what the rules left of that brick
    void DrawBricks();

    // Snake
    void SetupSnake();//lays the starting body out behind the head
    void TurnSnake();//records the direction the player wants
    void MoveSnake(std::map<std::string, int>& state);//one move of the snake
    void GrowSnakeAndPlaceFood(std::map<std::string, int>& state);//what happens after the rules have run
    void ChangeBodyTicks(int amount);//adds to every counter that is above zero
    void DrawSnake(std::map<std::string, int>& state);
    void DrawCell(const std::string& name, int cellX, int cellY);//draws one grid space

    // Text
    void DrawFieldText(std::map<std::string, int>& state);//adds every text entry in the game's json to the frame
    int TextPosition(std::map<std::string, int>& state, const nlohmann::json& position);//a text position is a number or the state value the json names

    // Bricks
    std::vector<std::string> brickRows;//the bricks, each character is how many hits that brick has left
    int brickRow = 0;//the place in the brick rows that was hit this update
    int brickColumn = 0;
    bool onBricks = false;

    // Snake
    std::vector<std::vector<int>> bodyTicks;//tick counter for every grid space, the head leaves one behind and it counts down to the tail
    std::mt19937 randomGenerator;//picks where food is tried

    // Movement timing
    std::chrono::steady_clock::time_point lastFrameTime;//when the previous frame was measured
    double elapsedMilliseconds = 0;//time built up toward the next update
};

