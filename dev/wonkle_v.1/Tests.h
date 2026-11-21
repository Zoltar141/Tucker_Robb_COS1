#pragma once

// Runs every test below and prints how many passed and failed.
// Run it from the project folder, the tests read the game jsons, themes.json and the wav files from there.
void RunAllTests();

// Engine
void RunGameStateTests();//storing values, whole numbers and decimals
void RunWonkleRuntimeTests();//comparisons, operations, rule order, state references, decimals
void RunRuleLoadingTests();//rules written as json, the way the games load them
void RunCollisionTests();//what the ball, paddles and bricks use to hit things

// Files
void RunFileBossTests();//reading and writing files and json
void RunGameFileTests();//every game's json loads, its rules load, its sounds exist
void RunSaveTests();//save, load and reset, puts the real save file back when done

// Look
void RunThemeTests();//themes, themes.json and color names
