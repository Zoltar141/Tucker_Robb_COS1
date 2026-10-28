// wonkle_v.1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "WonkleRuntime.h"

int main()
{
    //RunTimeTest
    WonkleRuntime runtime;

    runtime.SetStateValue("score", 0);//set up initial state

    WonkleRuntime::Rule rule;//create a rule; if score equals 0, add 10

    rule.conditions.push_back({//if score 0
        "score",
        Comparison::Equal,
        0
        });

    rule.transformations.push_back({//add ten
        "score",
        Operation::Add,
        10
        });

    runtime.AddRule(rule);//add the rule
    
    runtime.Update();//run the engine
    
    int score;//check

    if (runtime.GetStateValue("score", score))
    {
        std::cout << "Score: " << score << '\n';//should output 10
    }
    else
    {
        std::cout << "ERROR: Score not found!\n";
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
