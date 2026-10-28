#include <iostream>
#include "Tests.h"
#include "GameState.h"
#include "WonkleRuntime.h"

//RunTimeTest
void RunWonkleRuntimeTest()
{
    WonkleRuntime runtime;

    int score = 0;

    runtime.SetStateValue("score", 0); //set initial state

    WonkleRuntime::Rule rule;//new rule if 0 add 10

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

    
    runtime.AddRule(rule);//add rule

    runtime.Update();    //run engine

    if (runtime.GetStateValue("score", score) && score == 10)//if updated correctly  
    {
        std::cout << "PASS: WonkleRuntime evaluates rules and updates state.\n";
    }
    else
    {
        std::cout << "FAIL: WonkleRuntime did not update the score correctly.\n";
    }
}


void RunGameStateTests()
{
    GameState state;
    int value = 0;

    //test 1:store and get value
    state.SetValue("ball_x", 10);

    if (state.GetValue("ball_x", value) && value == 10)
    {
        std::cout << "PASS: GetValue retrieves stored values.\n";
    }
    else
    {
        std::cout << "FAIL: GetValue could not retrieve the value.\n";
    }

    //test 2:change value
    state.SetValue("ball_x", 20);

    if (state.GetValue("ball_x", value) && value == 20)
    {
        std::cout << "PASS: SetValue updates existing values.\n";
    }
    else
    {
        std::cout << "FAIL: SetValue did not update the value.\n";
    }

    //test 3: check for keys
    if (state.HasValue("ball_x"))
    {
        std::cout << "PASS: HasValue detects existing keys.\n";
    }
    else
    {
        std::cout << "FAIL: HasValue missed an existing key.\n";
    }

    if (!state.HasValue("missing_key"))
    {
        std::cout << "PASS: HasValue detects missing keys.\n";
    }
    else
    {
        std::cout << "FAIL: HasValue incorrectly found a missing key.\n";
    }

    //test 4:try to get a missing key
    if (!state.GetValue("missing_key", value))
    {
        std::cout << "PASS: GetValue handles missing keys.\n";
    }
    else
    {
        std::cout << "FAIL: GetValue incorrectly found a missing key.\n";
    }
}