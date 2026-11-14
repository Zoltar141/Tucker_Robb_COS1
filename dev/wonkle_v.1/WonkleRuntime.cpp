#include "WonkleRuntime.h"

static Comparison GetComparison(const std::string& name)
{
    static const std::unordered_map<std::string, Comparison> comparisons =
    {
        {"Equal", Comparison::Equal},
        {"NotEqual", Comparison::NotEqual},
        {"Less", Comparison::Less},
        {"LessEqual", Comparison::LessEqual},
        {"Greater", Comparison::Greater},
        {"GreaterEqual", Comparison::GreaterEqual}
    };

    return comparisons.at(name);
}

static Operation GetOperation(const std::string& name)
{
    static const std::unordered_map<std::string, Operation> operations =
    {
        {"Assign", Operation::Assign},
        {"Add", Operation::Add},
        {"Subtract", Operation::Subtract},
        {"Multiply", Operation::Multiply},
        {"Divide", Operation::Divide}
    };

    return operations.at(name);
};

WonkleRuntime::WonkleRuntime()//blank game state
{
}

void WonkleRuntime::SetStateValue(const std::string& key, int value)//works as before but now pulls from current state
{
    currentState.SetValue(key, value);
}

bool WonkleRuntime::GetStateValue(const std::string& key, int& value) const//works as before but now pulls from current state
{
    return currentState.GetValue(key, value);
}

void WonkleRuntime::LoadRules(const nlohmann::json& ruleData)
{
    rules.clear();

    for (const auto& ruleDataEntry : ruleData)
    {
        Rule rule;

        for (const auto& conditionData : ruleDataEntry.at("conditions"))
        {
            rule.conditions.push_back({
                conditionData.at("key").get<std::string>(),
                GetComparison(conditionData.at("comparison").get<std::string>()),
                Operand(conditionData.at("value").get<int>())
                });
        }

        for (const auto& transformationData : ruleDataEntry.at("transformations"))
        {
            rule.transformations.push_back({
                transformationData.at("key").get<std::string>(),
                GetOperation(transformationData.at("operation").get<std::string>()),
                Operand(transformationData.at("value").get<int>())
                });
        }

        AddRule(rule);
    }
}

void WonkleRuntime::AddRule(const Rule& rule)//add new rule to game state 
{
    rules.push_back(rule);//its a vector
}


const GameState& WonkleRuntime::GetState() const//read only access
{
    return currentState;//current runtime state
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