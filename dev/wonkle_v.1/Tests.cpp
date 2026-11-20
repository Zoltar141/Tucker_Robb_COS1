#include "Tests.h"
#include "GameState.h"
#include "WonkleRuntime.h"
#include "Collision.h"
#include "Boundary.h"
#include "Position.h"
#include "Theme.h"
#include "ThemeManager.h"
#include "Colors.h"
#include "FileBoss.h"
#include "GameController.h"
#include "GameObjects.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
// every game the Choose Program menu can start
const std::vector<std::string> GAME_FILES =
{
    "pong.json", "brickbreaker.json", "snake.json", "maze.json", "calculator.json"
};

static int passed = 0;
static int failed = 0;

// prints one result and counts it
static void Check(bool ok, const std::string& what)
{
    if (ok)
    {
        Colors::Print("PASS: ", Colors::Color::Green);
        passed++;
    }
    else
    {
        Colors::Print("FAIL: ", Colors::Color::Red);
        failed++;
    }

    std::cout << what << "\n";
}

static void Section(const std::string& name)
{
    std::cout << "\n--- " << name << " ---\n";
}

void RunAllTests()
{
    passed = 0;
    failed = 0;

    RunGameStateTests();
    RunWonkleRuntimeTests();
    RunRuleLoadingTests();
    RunCollisionTests();
    RunFileBossTests();
    RunGameFileTests();
    RunSaveTests();
    RunThemeTests();

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
}


// --------------------------------------------------
// GAME STATE
// --------------------------------------------------

void RunGameStateTests()
{
    Section("GameState");

    GameState state;
    int whole = 0;
    double decimal = 0.0;

    state.SetValue("score", 10);
    Check(state.GetValue("score", whole) && whole == 10, "GetValue reads back a stored value");

    state.SetValue("score", 20);
    Check(state.GetValue("score", whole) && whole == 20, "SetValue replaces an existing value");

    Check(!state.GetValue("missing", whole) && !state.HasValue("missing"), "a key that was never set is not found");

    state.SetValue("speed", 2.75);
    Check(state.GetValue("speed", decimal) && decimal == 2.75, "the double GetValue keeps decimals");
    Check(state.GetValue("speed", whole) && whole == 2, "the int GetValue drops the decimals");
}


// --------------------------------------------------
// WONKLE RUNTIME
// --------------------------------------------------

// one rule with one condition, true when the rule ran
static bool RuleFires(double value, Comparison comparison, const Operand& against)
{
    WonkleRuntime runtime;
    runtime.SetStateValue("value", value);
    runtime.SetStateValue("ten", 10);

    WonkleRuntime::Rule rule;
    rule.conditions.push_back({ "value", comparison, against });
    rule.transformations.push_back({ "fired", Operation::Assign, 1 });

    runtime.AddRule(rule);
    runtime.Update();

    return runtime.GetState().HasValue("fired");
}

// one rule with one transformation, the value it leaves behind
static double ApplyOnce(double start, Operation operation, const Operand& operand)
{
    WonkleRuntime runtime;
    runtime.SetStateValue("value", start);
    runtime.SetStateValue("four", 4);

    WonkleRuntime::Rule rule;
    rule.transformations.push_back({ "value", operation, operand });

    runtime.AddRule(rule);
    runtime.Update();

    double result = 0.0;
    runtime.GetState().GetValue("value", result);
    return result;
}

void RunWonkleRuntimeTests()
{
    Section("WonkleRuntime");

    // Each comparison fires when it should and stays quiet when it shouldn't.
    Check(RuleFires(10, Comparison::Equal, 10) && !RuleFires(9, Comparison::Equal, 10), "Equal");
    Check(RuleFires(9, Comparison::NotEqual, 10) && !RuleFires(10, Comparison::NotEqual, 10), "NotEqual");
    Check(RuleFires(9, Comparison::Less, 10) && !RuleFires(10, Comparison::Less, 10), "Less");
    Check(RuleFires(10, Comparison::LessEqual, 10) && !RuleFires(11, Comparison::LessEqual, 10), "LessEqual");
    Check(RuleFires(11, Comparison::Greater, 10) && !RuleFires(10, Comparison::Greater, 10), "Greater");
    Check(RuleFires(10, Comparison::GreaterEqual, 10) && !RuleFires(9, Comparison::GreaterEqual, 10), "GreaterEqual");

    // Each operation.
    Check(ApplyOnce(10, Operation::Assign, 25) == 25, "Assign");
    Check(ApplyOnce(10, Operation::Add, 5) == 15, "Add");
    Check(ApplyOnce(10, Operation::Subtract, 5) == 5, "Subtract");
    Check(ApplyOnce(10, Operation::Multiply, 5) == 50, "Multiply");
    Check(ApplyOnce(20, Operation::Divide, 5) == 4, "Divide");
    Check(ApplyOnce(20, Operation::Divide, 0) == 20, "dividing by zero leaves the value alone");

    // A value from the state instead of a number.
    Check(RuleFires(10, Comparison::Equal, Operand::State("ten")), "a condition can compare against another state value");
    Check(!RuleFires(10, Comparison::Equal, Operand::State("missing")), "comparing against a missing state value stops the rule");
    Check(ApplyOnce(10, Operation::Add, Operand::State("four")) == 14, "a transformation can use another state value");
    Check(ApplyOnce(10, Operation::Add, Operand::State("missing")) == 10, "using a missing state value leaves the value alone");

    // Every condition has to pass.
    {
        WonkleRuntime runtime;
        runtime.SetStateValue("x", 5);
        runtime.SetStateValue("y", 5);

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({ "x", Comparison::Equal, 5 });
        rule.conditions.push_back({ "y", Comparison::Equal, 6 });
        rule.transformations.push_back({ "fired", Operation::Assign, 1 });

        runtime.AddRule(rule);
        runtime.Update();

        Check(!runtime.GetState().HasValue("fired"), "one false condition stops the whole rule");
    }

    // A condition on a key that doesn't exist.
    {
        WonkleRuntime runtime;

        WonkleRuntime::Rule rule;
        rule.conditions.push_back({ "missing", Comparison::Equal, 0 });
        rule.transformations.push_back({ "fired", Operation::Assign, 1 });

        runtime.AddRule(rule);
        runtime.Update();

        Check(!runtime.GetState().HasValue("fired"), "a condition on a missing key stops the rule");
    }

    // Assign can make a new value, the other operations can't.
    {
        WonkleRuntime runtime;

        WonkleRuntime::Rule rule;
        rule.transformations.push_back({ "made", Operation::Assign, 1 });
        rule.transformations.push_back({ "not_made", Operation::Add, 1 });

        runtime.AddRule(rule);
        runtime.Update();

        Check(runtime.GetState().HasValue("made") && !runtime.GetState().HasValue("not_made"),
            "Assign creates a missing value, Add does not");
    }

    // Rules run in the order they were added and see each other's changes.
    {
        WonkleRuntime runtime;
        runtime.SetStateValue("score", 10);

        WonkleRuntime::Rule first;
        first.transformations.push_back({ "score", Operation::Add, 5 });

        WonkleRuntime::Rule second;
        second.conditions.push_back({ "score", Comparison::Equal, 15 });
        second.transformations.push_back({ "fired", Operation::Assign, 1 });

        runtime.AddRule(first);
        runtime.AddRule(second);
        runtime.Update();

        Check(runtime.GetState().HasValue("fired"), "a later rule sees what an earlier rule changed");
    }

    // Decimals. These fail while WonkleRuntime's double GetStateValue reads through the int version.
    {
        WonkleRuntime runtime;
        runtime.SetStateValue("total", 2.5);

        double total = 0.0;
        Check(runtime.GetStateValue("total", total) && total == 2.5, "GetStateValue keeps decimals");
    }

    Check(ApplyOnce(3.5, Operation::Add, 1) == 4.5, "a rule keeps the decimals of the value it changes");
    Check(RuleFires(0.5, Comparison::Greater, 0), "a condition sees decimals (0.5 is Greater than 0)");
}


// --------------------------------------------------
// RULES FROM JSON
// --------------------------------------------------

void RunRuleLoadingTests()
{
    Section("Rules from json");

    // the same layout the game jsons use
    nlohmann::json rules = nlohmann::json::parse(R"([
        {
            "name": "player_scores",
            "conditions": [ { "key": "ball_x", "comparison": "GreaterEqual", "value": { "state": "goal_x" } } ],
            "transformations": [
                { "key": "score", "operation": "Add", "value": 1 },
                { "key": "ball_x", "operation": "Assign", "value": 40 }
            ]
        }
    ])");

    WonkleRuntime runtime;
    runtime.SetStateValue("ball_x", 79);
    runtime.SetStateValue("goal_x", 79);
    runtime.SetStateValue("score", 0);

    bool loaded = true;

    try
    {
        runtime.LoadRules(rules);
    }
    catch (const std::exception&)
    {
        loaded = false;
    }

    Check(loaded, "LoadRules reads a rule with a number and a state value");

    runtime.Update();

    int score = 0;
    int ballX = 0;
    Check(runtime.GetStateValue("score", score) && score == 1 &&
        runtime.GetStateValue("ball_x", ballX) && ballX == 40,
        "a rule loaded from json runs every transformation");

    // A typo in a rule has to be caught, not quietly ignored.
    nlohmann::json typo = nlohmann::json::parse(R"([
        { "conditions": [ { "key": "x", "comparison": "Equals", "value": 1 } ], "transformations": [] }
    ])");

    bool threw = false;

    try
    {
        WonkleRuntime other;
        other.LoadRules(typo);
    }
    catch (const std::exception&)
    {
        threw = true;
    }

    Check(threw, "LoadRules throws on a comparison name that doesn't exist");
}


// --------------------------------------------------
// COLLISION
// --------------------------------------------------

void RunCollisionTests()
{
    Section("Collision");

    Check(Collision::CheckPosition(Position(5, 5), Position(5, 5)), "CheckPosition matches the same spot");
    Check(!Collision::CheckPosition(Position(5, 5), Position(6, 5)), "CheckPosition misses a different spot");

    // a 5 by 5 rectangle starting at (2, 2) covers 2 to 6 on both sides
    Position rectangle(2, 2);

    Check(Collision::CheckRectangle(Position(4, 4), rectangle, 5, 5), "CheckRectangle hits inside");
    Check(!Collision::CheckRectangle(Position(10, 10), rectangle, 5, 5), "CheckRectangle misses outside");
    Check(Collision::CheckRectangle(Position(2, 2), rectangle, 5, 5), "CheckRectangle includes the left and top edges");
    Check(!Collision::CheckRectangle(Position(7, 4), rectangle, 5, 5) &&
        !Collision::CheckRectangle(Position(4, 7), rectangle, 5, 5), "CheckRectangle stops before the right and bottom edges");

    Boundary boundary(80, 25, 0, 1, 79, 23);

    Check(Collision::CheckBoundary(Position(0, 10), boundary) == BoundarySide::Left, "CheckBoundary finds the left wall");
    Check(Collision::CheckBoundary(Position(79, 10), boundary) == BoundarySide::Right, "CheckBoundary finds the right wall");
    Check(Collision::CheckBoundary(Position(10, 1), boundary) == BoundarySide::Top, "CheckBoundary finds the top wall");
    Check(Collision::CheckBoundary(Position(10, 23), boundary) == BoundarySide::Bottom, "CheckBoundary finds the bottom wall");
    Check(Collision::CheckBoundary(Position(10, 10), boundary) == BoundarySide::None, "CheckBoundary finds nothing in the middle");
}


// --------------------------------------------------
// FILE BOSS
// --------------------------------------------------

void RunFileBossTests()
{
    Section("FileBoss");

    FileBoss fileBoss;
    const std::string textFile = "wonkle_test.txt";
    const std::string jsonFile = "wonkle_test.json";
    std::string content;

    Check(fileBoss.WriteFile(textFile, "Wonkle") && fileBoss.ReadFile(textFile, content) && content == "Wonkle",
        "WriteFile then ReadFile gives the same text back");

    fileBoss.WriteFile(textFile, "Replaced");
    Check(fileBoss.ReadFile(textFile, content) && content == "Replaced", "WriteFile replaces what was there");

    fileBoss.AppendFile(textFile, " and more");
    Check(fileBoss.ReadFile(textFile, content) && content == "Replaced and more", "AppendFile adds to the end");

    Check(!fileBoss.ReadFile("wonkle_no_such_file.txt", content), "ReadFile fails on a missing file");

    Check(!fileBoss.FileExists("") && !fileBoss.ReadFile("", content) &&
        !fileBoss.WriteFile("", "x") && !fileBoss.AppendFile("", "x"), "an empty path is turned down everywhere");

    nlohmann::json data;

    fileBoss.WriteFile(jsonFile, R"({ "score": 42, "name": "Wonkle" })");
    Check(fileBoss.ReadJSON(jsonFile, data) && data["score"] == 42 && data["name"] == "Wonkle", "ReadJSON reads valid json");

    fileBoss.WriteFile(jsonFile, "{ broken json");
    Check(!fileBoss.ReadJSON(jsonFile, data), "ReadJSON fails on broken json");

    Check(!fileBoss.ReadJSON("wonkle_no_such_file.json", data), "ReadJSON fails on a missing file");

    std::filesystem::remove(textFile);
    std::filesystem::remove(jsonFile);
}


// --------------------------------------------------
// GAME FILES
// --------------------------------------------------

void RunGameFileTests()
{
    Section("Game files");

    FileBoss fileBoss;

    for (const std::string& file : GAME_FILES)
    {
        nlohmann::json config;

        if (!fileBoss.ReadJSON(file, config) || !config.contains("game"))
        {
            Check(false, file + " loads");
            continue;
        }

        const nlohmann::json& game = config["game"];

        Check(game.contains("initial_state") && game.contains("rules"), file + " has initial_state and rules");

        // the game menu prints these, a missing one crashes it
        Check(game.contains("menu") && game["menu"].contains("art") &&
            game["menu"].contains("description") && game["menu"].contains("controls"),
            file + " has menu art, description and controls");

        // loaded the same way GameController::SetupRules loads them
        std::string ruleError;

        try
        {
            WonkleRuntime runtime;
            runtime.LoadRules(game.at("rules"));
        }
        catch (const std::exception& error)
        {
            ruleError = std::string(" (") + error.what() + ")";
        }

        Check(ruleError.empty(), file + " rules all load" + ruleError);

        // a missing wav is skipped without a word during play, so check for it here
        std::string missingSounds;

        if (game.contains("sounds"))
        {
            for (const auto& sound : game["sounds"])
            {
                std::string wav = sound.value("file", "");

                if (!fileBoss.FileExists(wav))
                {
                    missingSounds += " " + wav;
                }
            }
        }

        Check(missingSounds.empty(), file + " has every sound file it lists" +
            (missingSounds.empty() ? "" : " (missing:" + missingSounds + ")"));

        // the menu builds every game this way
        bool built = true;

        try
        {
            ThemeManager themes;
            GameObjects build(themes, file);
        }
        catch (const std::exception&)
        {
            built = false;
        }

        Check(built, file + " builds a game");
    }
}


// --------------------------------------------------
// SAVES
// --------------------------------------------------

// opens up the save functions GameController keeps protected, only the tests use this
class SaveTestGame : public GameObjects
{
public:
    using GameObjects::GameObjects;
    using GameController::SaveGame;
    using GameController::LoadGame;
    using GameController::runtime;
};

void RunSaveTests()
{
    Section("Saves");

    // these tests wipe wonkle_save.json, so keep the real one and put it back at the end
    FileBoss fileBoss;
    std::string realSaveFile;
    bool hadSaveFile = fileBoss.ReadFile(GameController::SAVE_FILE, realSaveFile);

    {
        ThemeManager themes;
        SaveTestGame game(themes, "snake.json");

        GameController::ResetAllSaveData();
        Check(!game.HasSave() && !game.LoadGame(), "with no save, HasSave is false and LoadGame fails");

        game.runtime.SetStateValue("score", 40);
        Check(game.SaveGame() && game.HasSave(), "SaveGame writes a save that HasSave finds");

        game.runtime.SetStateValue("score", 0);

        int score = 0;
        Check(game.LoadGame() && game.runtime.GetStateValue("score", score) && score == 40,
            "LoadGame puts the saved values back");

        SaveTestGame other(themes, "pong.json");
        Check(!other.HasSave(), "a save for one game doesn't show up for another");

        Check(GameController::ResetAllSaveData() && !game.HasSave(), "ResetAllSaveData wipes the saves");
    }

    if (hadSaveFile)
    {
        fileBoss.WriteFile(GameController::SAVE_FILE, realSaveFile);
    }
    else
    {
        std::filesystem::remove(GameController::SAVE_FILE);
    }
}


// --------------------------------------------------
// THEMES
// --------------------------------------------------

void RunThemeTests()
{
    Section("Themes");

    Theme theme("Test");
    theme.SetColor(ThemeRoles::Ball, Colors::Color::Red, Colors::Color::Blue);

    ThemeColor ball = theme.GetColor(ThemeRoles::Ball);
    Check(ball.foreground == Colors::Color::Red && ball.background == Colors::Color::Blue, "a theme gives back the colors it was set to");

    ThemeColor missing = theme.GetColor("NoSuchRole");
    Check(missing.foreground == Colors::Color::White && missing.background == Colors::Color::Black,
        "a role the theme doesn't have falls back to white on black");

    // every color name Colors knows
    const std::vector<std::pair<std::string, Colors::Color>> colorNames =
    {
        { "Black", Colors::Color::Black }, { "White", Colors::Color::White },
        { "Blue", Colors::Color::Blue }, { "DarkBlue", Colors::Color::DarkBlue },
        { "Green", Colors::Color::Green }, { "DarkGreen", Colors::Color::DarkGreen },
        { "Cyan", Colors::Color::Cyan }, { "DarkCyan", Colors::Color::DarkCyan },
        { "Red", Colors::Color::Red }, { "DarkRed", Colors::Color::DarkRed },
        { "Purple", Colors::Color::Purple }, { "DarkPurple", Colors::Color::DarkPurple },
        { "Yellow", Colors::Color::Yellow }, { "DarkYellow", Colors::Color::DarkYellow },
        { "Gray", Colors::Color::Gray }, { "DarkGray", Colors::Color::DarkGray },
        { "Tan", Colors::Color::Tan }, { "Brown", Colors::Color::Brown }
    };

    std::string wrongNames;

    for (const auto& color : colorNames)
    {
        if (Colors::FromString(color.first) != color.second)
        {
            wrongNames += " " + color.first;
        }
    }

    Check(wrongNames.empty(), "FromString knows all 18 color names" + wrongNames);
    Check(Colors::FromString("NotAColor") == Colors::Color::White, "FromString turns an unknown name into White");

    // themes.json, a misspelled color quietly turns White, so look for any
    FileBoss fileBoss;
    nlohmann::json themeFile;

    if (!fileBoss.ReadJSON("themes.json", themeFile) || !themeFile.contains("themes"))
    {
        Check(false, "themes.json loads");
        return;
    }

    std::string badColors;

    for (const auto& themeData : themeFile["themes"])
    {
        if (!themeData.contains("colors"))
        {
            continue;
        }

        for (const auto& role : themeData.at("colors").items())
        {
            if (!role.value().is_object())
            {
                continue;
            }

            for (const char* side : { "foreground", "background" })
            {
                std::string name = role.value().value(side, "");

                if (Colors::FromString(name) == Colors::Color::White && name != "White")
                {
                    badColors += " " + themeData.value("name", "?") + "/" + role.key() + "/" + side + "=" + name;
                }
            }
        }
    }

    Check(badColors.empty(), "every color in themes.json is a real color name" + badColors);

    ThemeManager manager;
    Check(manager.GetThemes().size() == themeFile["themes"].size(), "ThemeManager loads every theme in themes.json");

    Check(!manager.RegisterTheme(Theme(manager.GetThemes()[0].GetName())), "RegisterTheme turns down a name that's already used");

    std::string current = manager.GetActiveTheme()->GetName();
    Check(!manager.SetActiveTheme("NoSuchTheme") && manager.GetActiveTheme()->GetName() == current,
        "picking a theme that doesn't exist keeps the current one");

    std::string last = manager.GetThemes().back().GetName();
    Check(manager.SetActiveTheme(last) && manager.GetActiveTheme()->GetName() == last, "SetActiveTheme switches themes");
}
