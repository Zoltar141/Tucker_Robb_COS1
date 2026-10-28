#pragma once
#include <string>
#include <vector>
#include <unordered_map>


enum class Comparison
{
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual
};

enum class Operation
{
    Assign,
    Add,
    Subtract,
    Multiply,
    Divide
};

class WonkleRuntime
{
public:
    
    using State = std::unordered_map<std::string, int>; //so i dont have to type that nonsense out 

    struct Condition          //what must be true about the state
    {
        std::string key;      //which value to inspect
        Comparison operation; //how to compare it 
        int value;              //value to compare it against
    };

    struct Transformation  //how it should change the state
    {
        std::string key;    //which target to change 
        Operation operation;//how to change it 
        int value;            //by how much
    };

    struct Rule  //combines the condition and transformation 
    {
        std::vector<Condition> conditions;
        std::vector<Transformation> transformations;
    };

    WonkleRuntime();//defaut for now

    void SetStateValue(const std::string& key, int value);//example WonkleRuntime.SetStateValue("score", 0);       
    bool GetStateValue(const std::string& key, int& value) const;//a missing key won't automatically create a new state entry.

    void AddRule(const Rule& rule);//Register a rule for the runtime to evaluate during its update cycle.

    void Update();//Evaluates the conditions for registered rules.
                  //Determines which rules apply.
                  //Applies their transformations.

    const State& GetState() const;//read-only access to the entire current state.


private:

    State current; //the current runState
    std::vector<Rule> rules; //rules storage

    bool EvaluateCondition(const Condition& condition) const;//checks condtition for true
    void ApplyTransformation(const Transformation& transformation);//makes changes
};

