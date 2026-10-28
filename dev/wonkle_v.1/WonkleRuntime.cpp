#include "WonkleRuntime.h"

WonkleRuntime::WonkleRuntime()//blank game state
{
}

void WonkleRuntime::SetStateValue(const std::string& key, int value)//sets value
{
    current[key] = value;//creates a new key if one doesnt exist
}

bool WonkleRuntime::GetStateValue(const std::string& key, int& value) const //gets state value
{
    auto it = current.find(key);  //iterator points to key

    if (it == current.end())      //if key doesnt exist 
    {
        return false;             //couldnt find key
    }

    value = it->second;           //otherwise get value from map 
    return true;
}

void WonkleRuntime::AddRule(const Rule& rule)//add new rule to game state 
{
    rules.push_back(rule);//its a vector
}

const WonkleRuntime::State& WonkleRuntime::GetState() const//read only access
{
    return current;//current runtime state
}


bool WonkleRuntime::EvaluateCondition(const Condition& condition) const//checks whether a condition is true
{
    int value;

    
    if (!GetStateValue(condition.key, value))//if key missing
    {
        return false;//stop
    }

   
    switch (condition.operation)//compare state to condition
    {
    case Comparison::Equal:
        return value == condition.value;

    case Comparison::NotEqual:
        return value != condition.value;

    case Comparison::Less:
        return value < condition.value;

    case Comparison::LessEqual:
        return value <= condition.value;

    case Comparison::Greater:
        return value > condition.value;

    case Comparison::GreaterEqual:
        return value >= condition.value;
    }

    return false;
}


void WonkleRuntime::ApplyTransformation(const Transformation& transformation) //applies transformation to state 
{
    int value;

    
    if (!GetStateValue(transformation.key, value))//if no key skip transformation
    {
        return;
    }

    
    switch (transformation.operation)//apply transformation
    {
    case Operation::Assign:
        SetStateValue(transformation.key, transformation.value);
        break;

    case Operation::Add:
        SetStateValue(transformation.key, value + transformation.value);
        break;

    case Operation::Subtract:
        SetStateValue(transformation.key, value - transformation.value);
        break;

    case Operation::Multiply:
        SetStateValue(transformation.key, value * transformation.value);
        break;

    case Operation::Divide:
        if (transformation.value != 0)
        {
            SetStateValue(transformation.key, value / transformation.value);
        }
        break;
    }
}


void WonkleRuntime::Update()//combine conditions transforms and state
{
    for (const Rule& rule : rules)//for each rule
    {
        bool conditionsMet = true;

        
        for (const Condition& condition : rule.conditions)//every condition must be true.
        {
            if (!EvaluateCondition(condition))//if not true
            {
                conditionsMet = false;//return false and stop
                break;
            }
        }

        
        if (conditionsMet)//if all pass
        {
            for (const Transformation& transformation : rule.transformations)//apply each transformation 
            {
                ApplyTransformation(transformation);
            }
        }
    }
}