#include <iostream>
#include "Tests.h"
#include "WonkleRuntime.h"
#include "GameState.h"
#include "Position.h"
#include "Collision.h"
#include "Boundary.h"
#include "Theme.h"
#include "ThemeManager.h"
#include "Colors.h"
#include "FileBoss.h"
#include "Render.h"
#include "InputHelper.h"
#include "Menu.h"
#include "PONG.h"

#include <filesystem>
#include <fstream>
#include <vector>

// --------------------------------------------------
// GAME STATE TESTS
// --------------------------------------------------

void RunGameStateTests()
{
    GameState state;
    int value = 0;

    // Test 1: Store and retrieve a value
    state.SetValue("ball_x", 10);

    if (state.GetValue("ball_x", value) && value == 10)
        std::cout << "PASS: GetValue retrieves stored values.\n";
    else
        std::cout << "FAIL: GetValue could not retrieve the value.\n";

    // Test 2: Update an existing value
    state.SetValue("ball_x", 20);

    if (state.GetValue("ball_x", value) && value == 20)
        std::cout << "PASS: SetValue updates existing values.\n";
    else
        std::cout << "FAIL: SetValue did not update the value.\n";

    // Test 3: Detect an existing key
    if (state.HasValue("ball_x"))
        std::cout << "PASS: HasValue detects existing keys.\n";
    else
        std::cout << "FAIL: HasValue missed an existing key.\n";

    // Test 4: Detect a missing key
    if (!state.HasValue("missing_key"))
        std::cout << "PASS: HasValue detects missing keys.\n";
    else
        std::cout << "FAIL: HasValue incorrectly found a missing key.\n";

    // Test 5: Retrieve a missing key
    if (!state.GetValue("missing_key", value))
        std::cout << "PASS: GetValue handles missing keys.\n";
    else
        std::cout << "FAIL: GetValue incorrectly found a missing key.\n";

    // Test 6: Store multiple independent values
    state.SetValue("ball_y", 15);
    state.SetValue("score", 100);

    if (state.GetValue("ball_x", value) && value == 20 &&
        state.GetValue("ball_y", value) && value == 15 &&
        state.GetValue("score", value) && value == 100)
        std::cout << "PASS: GameState stores multiple independent values.\n";
    else
        std::cout << "FAIL: GameState did not preserve multiple values.\n";

    // Test 7: Store a negative value
    state.SetValue("velocity", -5);

    if (state.GetValue("velocity", value) && value == -5)
        std::cout << "PASS: GameState stores negative values.\n";
    else
        std::cout << "FAIL: GameState could not store a negative value.\n";

    // Test 8: Store zero
    state.SetValue("velocity", 0);

    if (state.GetValue("velocity", value) && value == 0)
        std::cout << "PASS: GameState stores zero values.\n";
    else
        std::cout << "FAIL: GameState could not store zero.\n";

    // Test 9: Store a value using an empty key
    state.SetValue("", 25);

    if (state.GetValue("", value) && value == 25)
        std::cout << "PASS: GameState handles empty keys.\n";
    else
        std::cout << "FAIL: GameState could not retrieve an empty key.\n";
}


// --------------------------------------------------
// WONKLE RUNTIME TESTS
// --------------------------------------------------

void RunWonkleRuntimeTest()
{
    WonkleRuntime runtime;
    int score = 0;

    runtime.SetStateValue("score", 0);

    WonkleRuntime::Rule rule;

    rule.conditions.push_back({
        "score",
        Comparison::Equal,
        0
        });

    rule.transformations.push_back({
        "score",
        Operation::Add,
        10
        });

    runtime.AddRule(rule);
    runtime.Update();

    if (runtime.GetStateValue("score", score) && score == 10)
        std::cout << "PASS: WonkleRuntime evaluates rules and updates state.\n";
    else
        std::cout << "FAIL: WonkleRuntime did not update the score correctly.\n";

    // Test Assign creates a missing value
    WonkleRuntime::Rule assignRule;

    assignRule.transformations.push_back({
        "new_value",
        Operation::Assign,
        42
        });

    runtime.AddRule(assignRule);
    runtime.Update();

    int value = 0;

    if (runtime.GetStateValue("new_value", value) && value == 42)
        std::cout << "PASS: Assign creates missing state entries.\n";
    else
        std::cout << "FAIL: Assign did not create the state entry.\n";
}


// --------------------------------------------------
// WONKLE RUNTIME COMPARISON TESTS
// --------------------------------------------------

void RunWonkleRuntimeComparisonTests()
{
    // Equal
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({ "score", Comparison::Equal, 10 });
        rule.transformations.push_back({ "result", Operation::Assign, 1 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("result", value) && value == 1)
            std::cout << "PASS: Equal comparison works.\n";
        else
            std::cout << "FAIL: Equal comparison failed.\n";
    }

    // Not Equal
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({ "score", Comparison::NotEqual, 5 });
        rule.transformations.push_back({ "result", Operation::Assign, 1 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("result", value) && value == 1)
            std::cout << "PASS: NotEqual comparison works.\n";
        else
            std::cout << "FAIL: NotEqual comparison failed.\n";
    }

    // Less
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 5);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({ "score", Comparison::Less, 10 });
        rule.transformations.push_back({ "result", Operation::Assign, 1 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("result", value) && value == 1)
            std::cout << "PASS: Less comparison works.\n";
        else
            std::cout << "FAIL: Less comparison failed.\n";
    }

    // Less Equal
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({ "score", Comparison::LessEqual, 10 });
        rule.transformations.push_back({ "result", Operation::Assign, 1 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("result", value) && value == 1)
            std::cout << "PASS: LessEqual comparison works.\n";
        else
            std::cout << "FAIL: LessEqual comparison failed.\n";
    }

    // Greater
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 15);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({ "score", Comparison::Greater, 10 });
        rule.transformations.push_back({ "result", Operation::Assign, 1 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("result", value) && value == 1)
            std::cout << "PASS: Greater comparison works.\n";
        else
            std::cout << "FAIL: Greater comparison failed.\n";
    }

    // Greater Equal
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({ "score", Comparison::GreaterEqual, 10 });
        rule.transformations.push_back({ "result", Operation::Assign, 1 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("result", value) && value == 1)
            std::cout << "PASS: GreaterEqual comparison works.\n";
        else
            std::cout << "FAIL: GreaterEqual comparison failed.\n";
    }
}


// --------------------------------------------------
// WONKLE RUNTIME OPERATION TESTS
// --------------------------------------------------

void RunWonkleRuntimeOperationTests()
{
    // Assign
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({ "score", Operation::Assign, 25 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", value) && value == 25)
            std::cout << "PASS: Assign operation works.\n";
        else
            std::cout << "FAIL: Assign operation failed.\n";
    }

    // Add
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({ "score", Operation::Add, 5 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", value) && value == 15)
            std::cout << "PASS: Add operation works.\n";
        else
            std::cout << "FAIL: Add operation failed.\n";
    }

    // Subtract
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({ "score", Operation::Subtract, 5 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", value) && value == 5)
            std::cout << "PASS: Subtract operation works.\n";
        else
            std::cout << "FAIL: Subtract operation failed.\n";
    }

    // Multiply
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({ "score", Operation::Multiply, 5 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", value) && value == 50)
            std::cout << "PASS: Multiply operation works.\n";
        else
            std::cout << "FAIL: Multiply operation failed.\n";
    }

    // Divide
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 20);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({ "score", Operation::Divide, 5 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", value) && value == 4)
            std::cout << "PASS: Divide operation works.\n";
        else
            std::cout << "FAIL: Divide operation failed.\n";
    }
}


// --------------------------------------------------
// WONKLE RUNTIME CONDITION TESTS
// --------------------------------------------------

void RunWonkleRuntimeConditionTests()
{
    // A false condition should prevent transformation
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 5);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({ "score", Comparison::Equal, 10 });
        rule.transformations.push_back({ "score", Operation::Add, 5 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", value) && value == 5)
            std::cout << "PASS: False conditions prevent transformations.\n";
        else
            std::cout << "FAIL: False condition allowed a transformation.\n";
    }

    // An empty condition list allows the rule to run
    {
        WonkleRuntime runtime;
        int value = 0;

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({ "score", Operation::Assign, 10 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", value) && value == 10)
            std::cout << "PASS: Rules without conditions can execute.\n";
        else
            std::cout << "FAIL: Rule without conditions did not execute.\n";
    }
}


// --------------------------------------------------
// WONKLE RUNTIME TRANSFORMATION TESTS
// --------------------------------------------------

void RunWonkleRuntimeTransformationTests()
{
    // Multiple transformations in one rule
    {
        WonkleRuntime runtime;
        int first = 0;
        int second = 0;

        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({ "score", Operation::Add, 5 });
        rule.transformations.push_back({ "lives", Operation::Assign, 3 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", first) && first == 15 &&
            runtime.GetStateValue("lives", second) && second == 3)
            std::cout << "PASS: Multiple transformations execute in one rule.\n";
        else
            std::cout << "FAIL: Multiple transformations did not execute correctly.\n";
    }

    // Transformation against a missing value
    {
        WonkleRuntime runtime;
        int value = 0;

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({ "missing", Operation::Add, 5 });

        runtime.AddRule(rule);
        runtime.Update();

        if (!runtime.GetStateValue("missing", value))
            std::cout << "PASS: Non-Assign transformations ignore missing values.\n";
        else
            std::cout << "FAIL: Transformation unexpectedly created a missing value.\n";
    }

    // Division by zero
    {
        WonkleRuntime runtime;
        int value = 0;
        runtime.SetStateValue("score", 20);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({ "score", Operation::Divide, 0 });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", value) && value == 20)
            std::cout << "PASS: Division by zero leaves the value unchanged.\n";
        else
            std::cout << "FAIL: Division by zero changed the value.\n";
    }
}


// --------------------------------------------------
// WONKLE RUNTIME MULTIPLE RULE TESTS
// --------------------------------------------------

void RunWonkleRuntimeMultipleRuleTests()
{
    WonkleRuntime runtime;
    int value = 0;

    runtime.SetStateValue("score", 0);

    WonkleRuntime::Rule firstRule;
    firstRule.transformations.push_back({ "score", Operation::Add, 10 });

    WonkleRuntime::Rule secondRule;
    secondRule.transformations.push_back({ "score", Operation::Add, 5 });

    runtime.AddRule(firstRule);
    runtime.AddRule(secondRule);
    runtime.Update();

    if (runtime.GetStateValue("score", value) && value == 15)
        std::cout << "PASS: Multiple rules execute in registration order.\n";
    else
        std::cout << "FAIL: Multiple rules did not execute correctly.\n";
}


// --------------------------------------------------
// WONKLE RUNTIME MISSING VALUE TESTS
// --------------------------------------------------

void RunWonkleRuntimeMissingValueTests()
{
    WonkleRuntime runtime;
    int value = 0;

    // Missing state value
    if (!runtime.GetStateValue("missing", value))
        std::cout << "PASS: Runtime handles missing state values.\n";
    else
        std::cout << "FAIL: Runtime returned a missing state value.\n";

    // Missing condition key
    {
        WonkleRuntime::Rule rule;
        rule.conditions.push_back({ "missing", Comparison::Equal, 0 });
        rule.transformations.push_back({ "result", Operation::Assign, 1 });

        runtime.AddRule(rule);
        runtime.Update();

        if (!runtime.GetStateValue("result", value))
            std::cout << "PASS: Missing condition keys prevent rule execution.\n";
        else
            std::cout << "FAIL: Missing condition key allowed rule execution.\n";
    }
}

// --------------------------------------------------
// WONKLE RUNTIME OPERAND TESTS
// --------------------------------------------------

void RunWonkleRuntimeOperandTests()
{
    // Test 1: State-reference comparison - Equal
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("ball_x", 10);
        runtime.SetStateValue("target_x", 10);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({
            "ball_x",
            Comparison::Equal,
            Operand::State("target_x")
            });

        rule.transformations.push_back({
            "result",
            Operation::Assign,
            1
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("result", value) && value == 1)
            std::cout << "PASS: State-reference Equal comparison works.\n";
        else
            std::cout << "FAIL: State-reference Equal comparison failed.\n";
    }

    // Test 2: State-reference comparison - Greater
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("ball_x", 15);
        runtime.SetStateValue("paddle_x", 10);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({
            "ball_x",
            Comparison::Greater,
            Operand::State("paddle_x")
            });

        rule.transformations.push_back({
            "result",
            Operation::Assign,
            1
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("result", value) && value == 1)
            std::cout << "PASS: State-reference Greater comparison works.\n";
        else
            std::cout << "FAIL: State-reference Greater comparison failed.\n";
    }

    // Test 3: State-reference comparison - Less
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("ball_x", 5);
        runtime.SetStateValue("paddle_x", 10);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({
            "ball_x",
            Comparison::Less,
            Operand::State("paddle_x")
            });

        rule.transformations.push_back({
            "result",
            Operation::Assign,
            1
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("result", value) && value == 1)
            std::cout << "PASS: State-reference Less comparison works.\n";
        else
            std::cout << "FAIL: State-reference Less comparison failed.\n";
    }

    // Test 4: State-reference comparison - Not Equal
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("ball_x", 5);
        runtime.SetStateValue("paddle_x", 10);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({
            "ball_x",
            Comparison::NotEqual,
            Operand::State("paddle_x")
            });

        rule.transformations.push_back({
            "result",
            Operation::Assign,
            1
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("result", value) && value == 1)
            std::cout << "PASS: State-reference NotEqual comparison works.\n";
        else
            std::cout << "FAIL: State-reference NotEqual comparison failed.\n";
    }

    // Test 5: State-reference assignment
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("source", 42);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({
            "destination",
            Operation::Assign,
            Operand::State("source")
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("destination", value) && value == 42)
            std::cout << "PASS: State-reference Assign copies another state value.\n";
        else
            std::cout << "FAIL: State-reference Assign failed.\n";
    }

    // Test 6: State-reference addition
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("score", 10);
        runtime.SetStateValue("bonus", 5);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({
            "score",
            Operation::Add,
            Operand::State("bonus")
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", value) && value == 15)
            std::cout << "PASS: State-reference Add works.\n";
        else
            std::cout << "FAIL: State-reference Add failed.\n";
    }

    // Test 7: State-reference subtraction
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("health", 100);
        runtime.SetStateValue("damage", 25);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({
            "health",
            Operation::Subtract,
            Operand::State("damage")
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("health", value) && value == 75)
            std::cout << "PASS: State-reference Subtract works.\n";
        else
            std::cout << "FAIL: State-reference Subtract failed.\n";
    }

    // Test 8: State-reference multiplication
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("speed", 5);
        runtime.SetStateValue("multiplier", 3);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({
            "speed",
            Operation::Multiply,
            Operand::State("multiplier")
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("speed", value) && value == 15)
            std::cout << "PASS: State-reference Multiply works.\n";
        else
            std::cout << "FAIL: State-reference Multiply failed.\n";
    }

    // Test 9: State-reference division
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("damage", 100);
        runtime.SetStateValue("resistance", 4);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({
            "damage",
            Operation::Divide,
            Operand::State("resistance")
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("damage", value) && value == 25)
            std::cout << "PASS: State-reference Divide works.\n";
        else
            std::cout << "FAIL: State-reference Divide failed.\n";
    }

    // Test 10: Missing comparison reference
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({
            "score",
            Comparison::Equal,
            Operand::State("missing")
            });

        rule.transformations.push_back({
            "result",
            Operation::Assign,
            1
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (!runtime.GetStateValue("result", value))
            std::cout << "PASS: Missing comparison references prevent rule execution.\n";
        else
            std::cout << "FAIL: Missing comparison reference allowed rule execution.\n";
    }

    // Test 11: Missing transformation reference
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({
            "score",
            Operation::Add,
            Operand::State("missing")
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", value) && value == 10)
            std::cout << "PASS: Missing transformation references leave state unchanged.\n";
        else
            std::cout << "FAIL: Missing transformation reference changed state.\n";
    }

    // Test 12: Self-reference
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({
            "score",
            Operation::Add,
            Operand::State("score")
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", value) && value == 20)
            std::cout << "PASS: Self-reference transformation works.\n";
        else
            std::cout << "FAIL: Self-reference transformation failed.\n";
    }

    // Test 13: Multiple state-reference transformations
    {
        WonkleRuntime runtime;
        int score = 0;
        int health = 0;

        runtime.SetStateValue("score", 10);
        runtime.SetStateValue("bonus", 5);
        runtime.SetStateValue("health", 100);
        runtime.SetStateValue("damage", 20);

        WonkleRuntime::Rule rule;

        rule.transformations.push_back({
            "score",
            Operation::Add,
            Operand::State("bonus")
            });

        rule.transformations.push_back({
            "health",
            Operation::Subtract,
            Operand::State("damage")
            });

        runtime.AddRule(rule);
        runtime.Update();

        if (runtime.GetStateValue("score", score) && score == 15 &&
            runtime.GetStateValue("health", health) && health == 80)
            std::cout << "PASS: Multiple state-reference transformations work.\n";
        else
            std::cout << "FAIL: Multiple state-reference transformations failed.\n";
    }

    // Test 14: Chained rule evaluation
    {
        WonkleRuntime runtime;
        int value = 0;

        runtime.SetStateValue("score", 10);
        runtime.SetStateValue("bonus", 5);

        WonkleRuntime::Rule firstRule;
        firstRule.transformations.push_back({
            "score",
            Operation::Add,
            Operand::State("bonus")
            });

        WonkleRuntime::Rule secondRule;
        secondRule.conditions.push_back({
            "score",
            Comparison::Greater,
            Operand::State("bonus")
            });

        secondRule.transformations.push_back({
            "result",
            Operation::Assign,
            Operand::State("score")
            });

        runtime.AddRule(firstRule);
        runtime.AddRule(secondRule);
        runtime.Update();

        if (runtime.GetStateValue("result", value) && value == 15)
            std::cout << "PASS: Chained rules use updated state values.\n";
        else
            std::cout << "FAIL: Chained rules did not use updated state values.\n";
    }
}

// --------------------------------------------------
// POSITION TESTS
// --------------------------------------------------

void RunPositionTests()
{
    Position position(10, 20);

    // Test 1: Constructor
    if (position.GetX() == 10 && position.GetY() == 20)
        std::cout << "PASS: Position constructor sets coordinates.\n";
    else
        std::cout << "FAIL: Position constructor set incorrect coordinates.\n";

    // Test 2: SetX
    position.SetX(30);

    if (position.GetX() == 30)
        std::cout << "PASS: SetX updates the X coordinate.\n";
    else
        std::cout << "FAIL: SetX did not update the X coordinate.\n";

    // Test 3: SetY
    position.SetY(40);

    if (position.GetY() == 40)
        std::cout << "PASS: SetY updates the Y coordinate.\n";
    else
        std::cout << "FAIL: SetY did not update the Y coordinate.\n";

    // Test 4: Move
    position.Move(5, 10);

    if (position.GetX() == 35 && position.GetY() == 50)
        std::cout << "PASS: Move adjusts both coordinates.\n";
    else
        std::cout << "FAIL: Move did not adjust both coordinates.\n";

    // Test 5: Negative coordinates
    position.SetX(-10);
    position.SetY(-20);

    if (position.GetX() == -10 && position.GetY() == -20)
        std::cout << "PASS: Position supports negative coordinates.\n";
    else
        std::cout << "FAIL: Position did not preserve negative coordinates.\n";

    // Test 6: Move with negative values
    position.Move(-5, -5);

    if (position.GetX() == -15 && position.GetY() == -25)
        std::cout << "PASS: Move supports negative movement.\n";
    else
        std::cout << "FAIL: Move failed with negative movement.\n";
}


// --------------------------------------------------
// COLLISION TESTS
// --------------------------------------------------

void RunCollisionTests()
{
    // Test 1: Same position
    Position first(5, 5);
    Position second(5, 5);

    if (Collision::CheckPosition(first, second))
        std::cout << "PASS: CheckPosition detects matching coordinates.\n";
    else
        std::cout << "FAIL: CheckPosition missed matching coordinates.\n";

    // Test 2: Different positions
    Position third(5, 5);
    Position fourth(6, 5);

    if (!Collision::CheckPosition(third, fourth))
        std::cout << "PASS: CheckPosition detects different coordinates.\n";
    else
        std::cout << "FAIL: CheckPosition reported different coordinates as equal.\n";

    // Test 3: Inside rectangle
    Position inside(5, 5);
    Position rectangle(2, 2);

    if (Collision::CheckRectangle(inside, rectangle, 5, 5))
        std::cout << "PASS: CheckRectangle detects positions inside a rectangle.\n";
    else
        std::cout << "FAIL: CheckRectangle missed a position inside the rectangle.\n";

    // Test 4: Outside rectangle
    Position outside(10, 10);

    if (!Collision::CheckRectangle(outside, rectangle, 5, 5))
        std::cout << "PASS: CheckRectangle detects positions outside a rectangle.\n";
    else
        std::cout << "FAIL: CheckRectangle detected an outside position as inside.\n";

    // Test 5: Left edge is included
    Position leftEdge(2, 4);

    if (Collision::CheckRectangle(leftEdge, rectangle, 5, 5))
        std::cout << "PASS: CheckRectangle includes the left edge.\n";
    else
        std::cout << "FAIL: CheckRectangle excluded the left edge.\n";

    // Test 6: Right edge is excluded
    Position rightEdge(7, 4);

    if (!Collision::CheckRectangle(rightEdge, rectangle, 5, 5))
        std::cout << "PASS: CheckRectangle excludes the right edge.\n";
    else
        std::cout << "FAIL: CheckRectangle included the right edge.\n";

    // Test 7: Top edge is included
    Position topEdge(4, 2);

    if (Collision::CheckRectangle(topEdge, rectangle, 5, 5))
        std::cout << "PASS: CheckRectangle includes the top edge.\n";
    else
        std::cout << "FAIL: CheckRectangle excluded the top edge.\n";

    // Test 8: Bottom edge is excluded
    Position bottomEdge(4, 7);

    if (!Collision::CheckRectangle(bottomEdge, rectangle, 5, 5))
        std::cout << "PASS: CheckRectangle excludes the bottom edge.\n";
    else
        std::cout << "FAIL: CheckRectangle included the bottom edge.\n";

    // Boundary tests
    Boundary boundary(80, 25, 0, 1, 79, 23);

    if (Collision::CheckBoundary(Position(0, 10), boundary) == BoundarySide::Left)
        std::cout << "PASS: CheckBoundary detects the left boundary.\n";
    else
        std::cout << "FAIL: CheckBoundary missed the left boundary.\n";

    if (Collision::CheckBoundary(Position(79, 10), boundary) == BoundarySide::Right)
        std::cout << "PASS: CheckBoundary detects the right boundary.\n";
    else
        std::cout << "FAIL: CheckBoundary missed the right boundary.\n";

    if (Collision::CheckBoundary(Position(10, 1), boundary) == BoundarySide::Top)
        std::cout << "PASS: CheckBoundary detects the top boundary.\n";
    else
        std::cout << "FAIL: CheckBoundary missed the top boundary.\n";

    if (Collision::CheckBoundary(Position(10, 23), boundary) == BoundarySide::Bottom)
        std::cout << "PASS: CheckBoundary detects the bottom boundary.\n";
    else
        std::cout << "FAIL: CheckBoundary missed the bottom boundary.\n";

    if (Collision::CheckBoundary(Position(10, 10), boundary) == BoundarySide::None)
        std::cout << "PASS: CheckBoundary detects positions inside the boundary.\n";
    else
        std::cout << "FAIL: CheckBoundary reported an interior position as a collision.\n";
}


// --------------------------------------------------
// BOUNDARY TESTS
// --------------------------------------------------

void RunBoundaryTests()
{
    Boundary boundary(80, 25, 0, 1, 79, 23);

    // Test 1: Width
    if (boundary.GetWidth() == 80)
        std::cout << "PASS: Boundary stores its width.\n";
    else
        std::cout << "FAIL: Boundary returned an incorrect width.\n";

    // Test 2: Height
    if (boundary.GetHeight() == 25)
        std::cout << "PASS: Boundary stores its height.\n";
    else
        std::cout << "FAIL: Boundary returned an incorrect height.\n";

    // Test 3: Left
    if (boundary.GetLeft() == 0)
        std::cout << "PASS: Boundary stores its left edge.\n";
    else
        std::cout << "FAIL: Boundary returned an incorrect left edge.\n";

    // Test 4: Top
    if (boundary.GetTop() == 1)
        std::cout << "PASS: Boundary stores its top edge.\n";
    else
        std::cout << "FAIL: Boundary returned an incorrect top edge.\n";

    // Test 5: Right
    if (boundary.GetRight() == 79)
        std::cout << "PASS: Boundary stores its right edge.\n";
    else
        std::cout << "FAIL: Boundary returned an incorrect right edge.\n";

    // Test 6: Bottom
    if (boundary.GetBottom() == 23)
        std::cout << "PASS: Boundary stores its bottom edge.\n";
    else
        std::cout << "FAIL: Boundary returned an incorrect bottom edge.\n";

    // Test 7: Draw creates the expected number of objects
    ThemeManager themes;
    Render render(themes);
    std::vector<RenderObject> objects;

    boundary.Draw(render, objects);

    int expectedCount = (80 * 2) + (25 * 2);

    if (objects.size() == expectedCount)
        std::cout << "PASS: Boundary Draw creates the expected number of objects.\n";
    else
        std::cout << "FAIL: Boundary Draw created an unexpected number of objects.\n";

    // Test 8: Top boundary position
    if (!objects.empty() &&
        objects[0].position.GetX() == 0 &&
        objects[0].position.GetY() == -1)
        std::cout << "PASS: Boundary Draw positions the top edge correctly.\n";
    else
        std::cout << "FAIL: Boundary Draw positioned the top edge incorrectly.\n";

    // Test 9: Top boundary role
    if (!objects.empty() && objects[0].role == ThemeRoles::Border)
        std::cout << "PASS: Boundary Draw assigns the border role.\n";
    else
        std::cout << "FAIL: Boundary Draw assigned an incorrect role.\n";
}


// --------------------------------------------------
// THEME TESTS
// --------------------------------------------------

void RunThemeTests()
{
    Theme theme("TestTheme");

    // Test 1: Name
    if (theme.GetName() == "TestTheme")
        std::cout << "PASS: Theme stores its name.\n";
    else
        std::cout << "FAIL: Theme returned an incorrect name.\n";

    // Test 2: Set and retrieve a color
    theme.SetColor(ThemeRoles::Ball, Colors::Color::Red, Colors::Color::Black);

    ThemeColor ball = theme.GetColor(ThemeRoles::Ball);

    if (ball.foreground == Colors::Color::Red &&
        ball.background == Colors::Color::Black)
        std::cout << "PASS: Theme stores and retrieves role colors.\n";
    else
        std::cout << "FAIL: Theme did not retrieve the assigned role colors.\n";

    // Test 3: Update an existing role
    theme.SetColor(ThemeRoles::Ball, Colors::Color::Green, Colors::Color::Blue);

    ball = theme.GetColor(ThemeRoles::Ball);

    if (ball.foreground == Colors::Color::Green &&
        ball.background == Colors::Color::Blue)
        std::cout << "PASS: Theme updates existing role colors.\n";
    else
        std::cout << "FAIL: Theme did not update the existing role colors.\n";

    // Test 4: Store multiple roles
    theme.SetColor(ThemeRoles::Paddle, Colors::Color::Yellow, Colors::Color::Black);
    theme.SetColor(ThemeRoles::MenuText, Colors::Color::White, Colors::Color::DarkBlue);

    ThemeColor paddle = theme.GetColor(ThemeRoles::Paddle);
    ThemeColor menu = theme.GetColor(ThemeRoles::MenuText);

    if (paddle.foreground == Colors::Color::Yellow &&
        menu.foreground == Colors::Color::White &&
        menu.background == Colors::Color::DarkBlue)
        std::cout << "PASS: Theme stores multiple independent roles.\n";
    else
        std::cout << "FAIL: Theme did not preserve multiple roles.\n";

    // Test 5: Missing role fallback
    ThemeColor missing = theme.GetColor("MissingRole");

    if (missing.foreground == Colors::Color::White &&
        missing.background == Colors::Color::Black)
        std::cout << "PASS: Theme returns default colors for missing roles.\n";
    else
        std::cout << "FAIL: Theme returned incorrect fallback colors.\n";
}


// --------------------------------------------------
// THEME MANAGER TESTS
// --------------------------------------------------

void RunThemeManagerTests()
{
    ThemeManager manager;

    Theme first("TestThemeOne");
    Theme second("TestThemeTwo");

    // Test 1: Register a theme
    if (manager.RegisterTheme(first))
        std::cout << "PASS: ThemeManager registers a new theme.\n";
    else
        std::cout << "FAIL: ThemeManager could not register a new theme.\n";

    // Test 2: Retrieve a registered theme
    if (manager.GetTheme("TestThemeOne") != nullptr)
        std::cout << "PASS: ThemeManager retrieves registered themes.\n";
    else
        std::cout << "FAIL: ThemeManager could not retrieve a registered theme.\n";

    // Test 3: Missing theme
    if (manager.GetTheme("MissingTheme") == nullptr)
        std::cout << "PASS: ThemeManager handles missing themes.\n";
    else
        std::cout << "FAIL: ThemeManager returned a nonexistent theme.\n";

    // Test 4: Reject duplicate names
    if (!manager.RegisterTheme(first))
        std::cout << "PASS: ThemeManager rejects duplicate theme names.\n";
    else
        std::cout << "FAIL: ThemeManager accepted a duplicate theme name.\n";

    // Test 5: Register another theme
    if (manager.RegisterTheme(second))
        std::cout << "PASS: ThemeManager registers multiple themes.\n";
    else
        std::cout << "FAIL: ThemeManager could not register a second theme.\n";

    // Test 6: GetThemes collection
    if (manager.GetThemes().size() >= 2)
        std::cout << "PASS: ThemeManager exposes its registered theme collection.\n";
    else
        std::cout << "FAIL: ThemeManager returned an incomplete theme collection.\n";

    // Test 7: First registered theme becomes active
    if (manager.GetActiveTheme() != nullptr &&
        manager.GetActiveTheme()->GetName() != "")
        std::cout << "PASS: ThemeManager has an active theme.\n";
    else
        std::cout << "FAIL: ThemeManager does not have an active theme.\n";

    // Test 8: Change active theme
    if (manager.SetActiveTheme("TestThemeTwo") &&
        manager.GetActiveTheme()->GetName() == "TestThemeTwo")
        std::cout << "PASS: ThemeManager changes the active theme.\n";
    else
        std::cout << "FAIL: ThemeManager could not change the active theme.\n";

    // Test 9: Reject nonexistent active theme
    if (!manager.SetActiveTheme("MissingTheme"))
        std::cout << "PASS: ThemeManager rejects nonexistent active themes.\n";
    else
        std::cout << "FAIL: ThemeManager accepted a nonexistent active theme.\n";

    // Test 10: Invalid selection does not replace active theme
    if (manager.GetActiveTheme()->GetName() == "TestThemeTwo")
        std::cout << "PASS: Invalid theme selection preserves the active theme.\n";
    else
        std::cout << "FAIL: Invalid theme selection changed the active theme.\n";
}


// --------------------------------------------------
// COLORS TESTS
// --------------------------------------------------

void RunColorsTests()
{
    // Test 1: Black
    if (Colors::FromString("Black") == Colors::Color::Black)
        std::cout << "PASS: FromString converts Black.\n";
    else
        std::cout << "FAIL: FromString failed to convert Black.\n";

    // Test 2: White
    if (Colors::FromString("White") == Colors::Color::White)
        std::cout << "PASS: FromString converts White.\n";
    else
        std::cout << "FAIL: FromString failed to convert White.\n";

    // Test 3: Blue
    if (Colors::FromString("Blue") == Colors::Color::Blue)
        std::cout << "PASS: FromString converts Blue.\n";
    else
        std::cout << "FAIL: FromString failed to convert Blue.\n";

    // Test 4: DarkBlue
    if (Colors::FromString("DarkBlue") == Colors::Color::DarkBlue)
        std::cout << "PASS: FromString converts DarkBlue.\n";
    else
        std::cout << "FAIL: FromString failed to convert DarkBlue.\n";

    // Test 5: Green
    if (Colors::FromString("Green") == Colors::Color::Green)
        std::cout << "PASS: FromString converts Green.\n";
    else
        std::cout << "FAIL: FromString failed to convert Green.\n";

    // Test 6: DarkGreen
    if (Colors::FromString("DarkGreen") == Colors::Color::DarkGreen)
        std::cout << "PASS: FromString converts DarkGreen.\n";
    else
        std::cout << "FAIL: FromString failed to convert DarkGreen.\n";

    // Test 7: Cyan
    if (Colors::FromString("Cyan") == Colors::Color::Cyan)
        std::cout << "PASS: FromString converts Cyan.\n";
    else
        std::cout << "FAIL: FromString failed to convert Cyan.\n";

    // Test 8: DarkCyan
    if (Colors::FromString("DarkCyan") == Colors::Color::DarkCyan)
        std::cout << "PASS: FromString converts DarkCyan.\n";
    else
        std::cout << "FAIL: FromString failed to convert DarkCyan.\n";

    // Test 9: Red
    if (Colors::FromString("Red") == Colors::Color::Red)
        std::cout << "PASS: FromString converts Red.\n";
    else
        std::cout << "FAIL: FromString failed to convert Red.\n";

    // Test 10: DarkRed
    if (Colors::FromString("DarkRed") == Colors::Color::DarkRed)
        std::cout << "PASS: FromString converts DarkRed.\n";
    else
        std::cout << "FAIL: FromString failed to convert DarkRed.\n";

    // Test 11: Purple
    if (Colors::FromString("Purple") == Colors::Color::Purple)
        std::cout << "PASS: FromString converts Purple.\n";
    else
        std::cout << "FAIL: FromString failed to convert Purple.\n";

    // Test 12: DarkPurple
    if (Colors::FromString("DarkPurple") == Colors::Color::DarkPurple)
        std::cout << "PASS: FromString converts DarkPurple.\n";
    else
        std::cout << "FAIL: FromString failed to convert DarkPurple.\n";

    // Test 13: Yellow
    if (Colors::FromString("Yellow") == Colors::Color::Yellow)
        std::cout << "PASS: FromString converts Yellow.\n";
    else
        std::cout << "FAIL: FromString failed to convert Yellow.\n";

    // Test 14: DarkYellow
    if (Colors::FromString("DarkYellow") == Colors::Color::DarkYellow)
        std::cout << "PASS: FromString converts DarkYellow.\n";
    else
        std::cout << "FAIL: FromString failed to convert DarkYellow.\n";

    // Test 15: Gray
    if (Colors::FromString("Gray") == Colors::Color::Gray)
        std::cout << "PASS: FromString converts Gray.\n";
    else
        std::cout << "FAIL: FromString failed to convert Gray.\n";

    // Test 16: DarkGray
    if (Colors::FromString("DarkGray") == Colors::Color::DarkGray)
        std::cout << "PASS: FromString converts DarkGray.\n";
    else
        std::cout << "FAIL: FromString failed to convert DarkGray.\n";

    // Test 17: Tan
    if (Colors::FromString("Tan") == Colors::Color::Tan)
        std::cout << "PASS: FromString converts Tan.\n";
    else
        std::cout << "FAIL: FromString failed to convert Tan.\n";

    // Test 18: Brown
    if (Colors::FromString("Brown") == Colors::Color::Brown)
        std::cout << "PASS: FromString converts Brown.\n";
    else
        std::cout << "FAIL: FromString failed to convert Brown.\n";

    // Test 19: Unknown name fallback
    if (Colors::FromString("NotAColor") == Colors::Color::White)
        std::cout << "PASS: FromString defaults unknown colors to White.\n";
    else
        std::cout << "FAIL: FromString returned an incorrect unknown-color fallback.\n";

    // Test 20: Empty name fallback
    if (Colors::FromString("") == Colors::Color::White)
        std::cout << "PASS: FromString handles empty color names.\n";
    else
        std::cout << "FAIL: FromString returned an incorrect empty-name fallback.\n";
}


// --------------------------------------------------
// FILE BOSS TESTS
// --------------------------------------------------

void RunFileBossTests()
{
    FileBoss fileBoss;

    const std::string testFile = "wonkle_fileboss_test.txt";
    const std::string jsonFile = "wonkle_fileboss_test.json";
    const std::string badJsonFile = "wonkle_fileboss_bad.json";

    std::string content;

    // Test 1: Empty path
    if (!fileBoss.FileExists(""))
        std::cout << "PASS: FileExists rejects an empty path.\n";
    else
        std::cout << "FAIL: FileExists accepted an empty path.\n";

    // Test 2: Missing file
    if (!fileBoss.FileExists(testFile))
        std::cout << "PASS: FileExists detects a missing file.\n";
    else
        std::cout << "FAIL: FileExists found a file that should not exist.\n";

    // Test 3: Write file
    if (fileBoss.WriteFile(testFile, "Wonkle test"))
        std::cout << "PASS: WriteFile creates a file.\n";
    else
        std::cout << "FAIL: WriteFile could not create a file.\n";

    // Test 4: Confirm file exists
    if (fileBoss.FileExists(testFile))
        std::cout << "PASS: FileExists detects a created file.\n";
    else
        std::cout << "FAIL: FileExists did not detect the created file.\n";

    // Test 5: Read file
    if (fileBoss.ReadFile(testFile, content) && content == "Wonkle test")
        std::cout << "PASS: ReadFile retrieves file contents.\n";
    else
        std::cout << "FAIL: ReadFile did not retrieve the expected contents.\n";

    // Test 6: Append file
    if (fileBoss.AppendFile(testFile, " appended"))
        std::cout << "PASS: AppendFile adds content to a file.\n";
    else
        std::cout << "FAIL: AppendFile could not append content.\n";

    // Test 7: Verify appended content
    content.clear();

    if (fileBoss.ReadFile(testFile, content) &&
        content == "Wonkle test appended")
        std::cout << "PASS: AppendFile preserves existing content.\n";
    else
        std::cout << "FAIL: AppendFile did not preserve existing content.\n";

    // Test 8: Overwrite file
    if (fileBoss.WriteFile(testFile, "Replacement"))
        std::cout << "PASS: WriteFile overwrites existing content.\n";
    else
        std::cout << "FAIL: WriteFile could not overwrite existing content.\n";

    // Test 9: Verify overwritten content
    content.clear();

    if (fileBoss.ReadFile(testFile, content) && content == "Replacement")
        std::cout << "PASS: WriteFile replaces the previous contents.\n";
    else
        std::cout << "FAIL: WriteFile did not replace the previous contents.\n";

    // Test 10: Read missing file
    if (!fileBoss.ReadFile("wonkle_missing_test_file.txt", content))
        std::cout << "PASS: ReadFile handles missing files.\n";
    else
        std::cout << "FAIL: ReadFile unexpectedly opened a missing file.\n";

    // Test 11: Write empty path
    if (!fileBoss.WriteFile("", "test"))
        std::cout << "PASS: WriteFile rejects an empty path.\n";
    else
        std::cout << "FAIL: WriteFile accepted an empty path.\n";

    // Test 12: Append empty path
    if (!fileBoss.AppendFile("", "test"))
        std::cout << "PASS: AppendFile rejects an empty path.\n";
    else
        std::cout << "FAIL: AppendFile accepted an empty path.\n";

    // Test 13: Read empty path
    if (!fileBoss.ReadFile("", content))
        std::cout << "PASS: ReadFile rejects an empty path.\n";
    else
        std::cout << "FAIL: ReadFile accepted an empty path.\n";

    // Test 14: Write JSON
    if (fileBoss.WriteFile(jsonFile, R"({"score":42,"name":"Wonkle"})"))
        std::cout << "PASS: Test JSON file was created.\n";
    else
        std::cout << "FAIL: Test JSON file could not be created.\n";

    // Test 15: Read JSON
    nlohmann::json data;

    if (fileBoss.ReadJSON(jsonFile, data))
        std::cout << "PASS: ReadJSON parses valid JSON.\n";
    else
        std::cout << "FAIL: ReadJSON could not parse valid JSON.\n";

    // Test 16: Verify JSON values
    if (data.contains("score") && data["score"] == 42 &&
        data.contains("name") && data["name"] == "Wonkle")
        std::cout << "PASS: ReadJSON preserves parsed values.\n";
    else
        std::cout << "FAIL: ReadJSON returned incorrect parsed values.\n";

    // Test 17: Malformed JSON
    fileBoss.WriteFile(badJsonFile, "{ invalid json");

    nlohmann::json badData;

    if (!fileBoss.ReadJSON(badJsonFile, badData))
        std::cout << "PASS: ReadJSON rejects malformed JSON.\n";
    else
        std::cout << "FAIL: ReadJSON accepted malformed JSON.\n";

    // Test 18: Missing JSON file
    nlohmann::json missingData;

    if (!fileBoss.ReadJSON("wonkle_missing_test.json", missingData))
        std::cout << "PASS: ReadJSON handles missing files.\n";
    else
        std::cout << "FAIL: ReadJSON opened a missing file.\n";

    // Test 19: Empty JSON path
    if (!fileBoss.ReadJSON("", missingData))
        std::cout << "PASS: ReadJSON rejects an empty path.\n";
    else
        std::cout << "FAIL: ReadJSON accepted an empty path.\n";

    // Clean up test files
    std::filesystem::remove(testFile);
    std::filesystem::remove(jsonFile);
    std::filesystem::remove(badJsonFile);
}

// --------------------------------------------------
// RENDER TESTS
// --------------------------------------------------

void RunRenderTests()
{
    ThemeManager themes;
    Render render(themes);

    // Test 1: BeginFrame
    render.BeginFrame(20, 10, ThemeRoles::Background);

    std::cout << "PASS: Render begins a frame without errors.\n";

    // Test 2: DrawCharacter inside frame
    render.DrawCharacter(5, 5, 'X', ThemeRoles::Ball);

    std::cout << "PASS: Render accepts character drawing inside a frame.\n";

    // Test 3: DrawText inside frame
    render.DrawText(1, 1, "Wonkle", ThemeRoles::MenuText);

    std::cout << "PASS: Render accepts text drawing inside a frame.\n";

    // Test 4: Horizontal line
    render.DrawHorizontalLine(1, 2, 5, '-', ThemeRoles::Border);

    std::cout << "PASS: Render accepts horizontal line drawing.\n";

    // Test 5: Vertical line
    render.DrawVerticalLine(10, 1, 5, '|', ThemeRoles::Border);

    std::cout << "PASS: Render accepts vertical line drawing.\n";

    // Test 6: Draw objects
    std::vector<RenderObject> objects;
    objects.push_back({ Position(3, 3), 'O', ThemeRoles::Ball });
    objects.push_back({ Position(4, 3), '|', ThemeRoles::Paddle });

    render.DrawObjects(objects);

    std::cout << "PASS: Render accepts collections of render objects.\n";

    // Test 7: Draw outside frame boundaries
    render.DrawCharacter(-1, -1, 'X', ThemeRoles::Ball);
    render.DrawCharacter(20, 10, 'X', ThemeRoles::Ball);

    std::cout << "PASS: Render handles drawing outside frame boundaries.\n";

    // Test 8: Present frame
    render.PresentFrame();

    std::cout << "PASS: Render presents a completed frame.\n";
}


// --------------------------------------------------
// MENU TESTS
// --------------------------------------------------

void RunMenuTests()
{
    // This checks that the Menu object can be constructed.

    Menu menu;

    std::cout << "PASS: Menu object can be constructed.\n";
}


// --------------------------------------------------
// PONG TESTS
// --------------------------------------------------

void RunPONGTests()
{
    ThemeManager themes;

    // Test 1: Construct PONG
    PONG game(themes);

    std::cout << "PASS: PONG object can be constructed with a ThemeManager.\n";
}