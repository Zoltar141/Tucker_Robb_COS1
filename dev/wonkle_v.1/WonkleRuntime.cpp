#include "WonkleRuntime.h"

WonkleRuntime::WonkleRuntime()//blank game state
{
}

void WonkleRuntime::SetStateValue(const std::string& key, int value)//works as before but now pulls from current state
{
    current.SetValue(key, value);
}

bool WonkleRuntime::GetStateValue(const std::string& key, int& value) const//works as before but now pulls from current state
{
    return current.GetValue(key, value);
}

void WonkleRuntime::AddRule(const Rule& rule)//add new rule to game state 
{
    rules.push_back(rule);//its a vector
}


const GameState& WonkleRuntime::GetState() const//read only access
{
    return current;//current runtime state
}



bool WonkleRuntime::EvaluateCondition(const Condition& condition) const
{
    int value = 0;
    int comparisonValue = condition.value.literal;

    // Retrieve the state value being evaluated.
    if (!GetStateValue(condition.key, value))
    {
        return false;
    }

    // Resolve the comparison operand.
    if (condition.value.isStateReference)
    {
        if (!GetStateValue(condition.value.stateKey, comparisonValue))
        {
            return false;
        }
    }

    switch (condition.operation)
    {
    case Comparison::Equal:
        return value == comparisonValue;

    case Comparison::NotEqual:
        return value != comparisonValue;

    case Comparison::Less:
        return value < comparisonValue;

    case Comparison::LessEqual:
        return value <= comparisonValue;

    case Comparison::Greater:
        return value > comparisonValue;

    case Comparison::GreaterEqual:
        return value >= comparisonValue;
    }

    return false;
}


void WonkleRuntime::ApplyTransformation(const Transformation& transformation)
{
    int operandValue = transformation.value.literal;

    // Resolve the operand.
    if (transformation.value.isStateReference)
    {
        if (!GetStateValue(transformation.value.stateKey, operandValue))
        {
            return;
        }
    }

    // Assignment can create a new state value.
    if (transformation.operation == Operation::Assign)
    {
        SetStateValue(transformation.key, operandValue);
        return;
    }

    int value = 0;

    if (!GetStateValue(transformation.key, value))
    {
        return;
    }

    switch (transformation.operation)
    {
    case Operation::Assign:
        break;

    case Operation::Add:
        SetStateValue(transformation.key, value + operandValue);
        break;

    case Operation::Subtract:
        SetStateValue(transformation.key, value - operandValue);
        break;

    case Operation::Multiply:
        SetStateValue(transformation.key, value * operandValue);
        break;

    case Operation::Divide:
        if (operandValue != 0)
        {
            SetStateValue(transformation.key, value / operandValue);
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