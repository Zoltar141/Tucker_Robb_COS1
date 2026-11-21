# W.O.N.K.L.E. Project Progress Report

## Overview

W.O.N.K.L.E. is a console program launcher. Every program in it---Pong,
Brick Breaker, Snake, Maze, and the Wonk-ulator---runs on the same
engine.

-   **GameController** runs the game loop.
-   **GameObjects** handles shared game components such as the ball,
    paddles, bricks, snake, and text.
-   **WonkleRuntime** evaluates each game's rules.
-   Each game's JSON file defines its behavior and configuration.

During the final round of development, I completed the player-facing
features, cleaned up the codebase, and built an automated test suite
that identified and helped fix several real bugs.

## 1. Features Completed or Added

### Save and Load System

-   Pressing `ESC` during a game opens a pause menu with **Resume**,
    **Save**, **Load**, and **Quit to Menu**.
-   A message beneath the field reports the result, such as
    `GAME SAVED`, `NO SAVE FOUND`, or `GAME LOADED`.
-   All games use one save file, `wonkle_save.json`, with a separate
    save slot named after each game's JSON file.
-   Only values listed in a game's `initial_state` are saved, allowing
    the JSON configuration to determine what belongs in a save.
-   High scores are recorded separately for each game that has a `score`
    state value.

### Load Game from the Title Screen

The title screen now has four options:

1.  Choose Program
2.  Load Game
3.  Settings
4.  Exit

Load Game lists only programs that have an existing save and starts the
selected game from that save. If no games have saves, the menu displays
`No saved games.` instead of showing an empty list.

### Individual Program Menus

Choosing a program opens its own menu with:

-   Title art
-   A description
-   Play
-   Controls
-   Back

The title art, description, and controls text come from the game's JSON
file. A new game can therefore have its own menu without requiring
additional C++ menu code.

### Two New Programs: Maze and the Wonk-ulator

**Maze** - Includes a countdown timer. - Uses the brick system to
represent walls. - Includes a finish marker. - Allows the player to move
along both axes.

**Wonk-ulator** - Is a calculator driven entirely by JSON rules. - Uses
two dials and an operator. - Displays decimal answers. - Displays a
W.O.N.K.L.E. message when dividing by zero.

### Sound Effects

-   Each game's JSON file can list sound effects.
-   A WAV file plays when its configured state value changes.
-   Sounds can be assigned to events such as scoring, bouncing, eating,
    winning, and losing.
-   Sounds play asynchronously with `SND_ASYNC`, so playback does not
    pause the game.
-   Missing WAV files are skipped rather than causing a crash.

### Reorganized Settings Menu

-   **Change Theme** opens a submenu containing the six themes in
    `themes.json`: Default, Forest, Ice, Dark, Light, and Classic.
-   **Reset All Saves** clears saves and high scores after a warning and
    confirmation step.
-   **Mute Sound / Unmute Sound** toggles game audio on or off.

### Automated Test Suite

`RunAllTests()` runs 89 checks and prints a colored PASS or FAIL result
for each check, followed by the totals.

### JSON Template Documentation

The game JSON template was rewritten to match the properties the engine
actually reads. It identifies required sections and optional features,
and documents:

-   The rule format
-   Key codes
-   Theme roles
-   Character values that must be written as text versus numbers

## 2. Refactoring and Code Improvements

### Test Suite Rewrite

The previous tests included checks that could not actually fail. The
Render, Menu, and Pong tests printed PASS without verifying behavior,
and there were 18 separate tests for color names.

These were replaced with 89 checks that test actual behavior.

Improvements include:

-   Added helper functions, `RuleFires` and `ApplyOnce`, to make
    rule-engine tests concise.
-   Used tables for comparison operations and color names.
-   Added checks against the actual game data.

For every game JSON file, the test suite checks that:

-   The file loads and its rules load.
-   Its menu text exists.
-   Every WAV file it references exists.
-   The game can be built.

The suite also checks that every color in `themes.json` is a valid color
name. This catches typos that would otherwise silently display as white.

The save tests back up the player's existing save file and restore it
afterward, so running the tests does not erase saved progress.

### Include Cleanup

Each source file was reviewed so it includes what it uses and avoids
unnecessary includes.

-   Removed unused includes, including `<iostream>` from headers and
    `<algorithm>`, `<limits>`, and `<unordered_map>` where they were not
    needed.
-   Moved includes used by only one `.cpp` file out of headers:
    `<fstream>` into `FileBoss.cpp` and `<unordered_map>` into
    `WonkleRuntime.cpp`.
-   Added headers that had only been available indirectly: `<vector>`,
    `<map>`, `<string>`, `<cstdlib>`, and `<stdexcept>`.
-   Kept `<Windows.h>` in `Colors.h` intentionally. Windows renames
    `DrawText`, and the `Render` class has a `DrawText` function, so the
    rename must be consistent across files.

### Smaller Cleanups

-   `WonkleRuntime::GetStateValue(double)` now reads the stored double
    directly. Previously, it went through the integer version and lost
    decimal precision.
-   The integer version of `GameState::GetValue` now uses an explicit
    `static_cast<int>` to make the conversion intentional.
-   Removed the unused `GetGameOverText` declaration.
-   Mute uses one shared flag checked by `PlaySoundFile`, avoiding the
    need for game-specific changes.
-   The Load Game list is generated from one list of game names and JSON
    filenames instead of separate branches for each game.

After these changes, the project builds with no errors and no warnings.

## 3. Bugs Resolved

### Games Shared One Save Slot

Every game was saving under the same empty name because the line
assigning each game's save name was missing from the `GameController`
constructor.

This caused problems such as saving Pong making Snake appear in Load
Game, loading Snake restoring Pong's values, and all games sharing one
high score.

The new save tests exposed the problem. Restoring the missing
constructor assignment fixed it.

### Rules Lost Decimal Values

The double version of `GetStateValue` read values through the integer
version. As a result, rules saw whole numbers instead of decimals. For
example, adding `1` to `3.5` produced `4` instead of `4.5`.

This especially affected the Wonk-ulator. The new decimal tests caught
the issue, and reading the double directly fixed it.

### Reset Option Broke the Settings Menu

The first Reset implementation attempted to create a `GameController`
object, even though `GameController` is abstract. The menu loop also
still exited using the old Back option number. This caused selecting
Reset to leave Settings and selecting Back to loop indefinitely.

Both problems were fixed by calling `ResetAllSaveData()` as a static
function and updating the loop to use the new Back option number.

### Compile Error After the Settings Change

A missing closing brace in `RunSettings` caused the compiler to
interpret `RunGameMenu` as if it were inside `RunSettings`. Adding the
missing brace resolved the compile error.

### Restart Replayed the Win/Lose Sound

Restarting resets state values. The sound system interpreted those
resets as changes and replayed the win or lose sound.

The sound system now takes a new state snapshot immediately after a
restart.

### Load Game Could Crash on a Broken Game File

Creating and running games from Load Game is now wrapped in `try/catch`.
If a game fails to load, the error message identifies the game that
failed, for example:

`W.O.N.K.L.E. Cannot load Snake: ...`

### Compiler and Code Analysis Warnings

The following warnings were cleared:

-   `C4244`: Possible loss of data in `GameState`.
-   `C6031`: Ignored `_getch` return value.
-   `C26451`: Integer arithmetic used as a vector index.
-   Missing-definition warning for the unused declaration.

## 4. Usability Improvements

-   **Less cluttered Settings:** The theme list is behind Change Theme,
    keeping the main Settings menu to four short options.
-   **Safe reset:** Reset All Saves displays a warning, requires a
    second confirmation, and reports whether the reset succeeded.
-   **Clear mute state:** The option changes between Mute Sound and
    Unmute Sound based on the current setting.
-   **Faster access to saved games:** Load Game starts a selected saved
    game directly and lists only games with saves.
-   **Controls screens:** Every program has a controls screen available
    before starting play.
-   **Error messages instead of crashes:** A game that cannot load
    displays a W.O.N.K.L.E. message and returns the player to the menu.
-   **Sound feedback:** Hits, scores, eating, winning, and losing can
    trigger sound effects.
-   **Clearer prompts:** For example, the Settings prompt now says
    `Select an option` instead of `Select a theme`.

## 5. Known Issues and Next Steps

The following items remain identified for future work:

1.  **Decimal values in game state:** Decimals are converted to whole
    numbers after one update. In `SetGameState`, the code writes `0` to
    each value where it should read the value. The fix has been
    identified: change `SetStateValue` to `GetStateValue`. The fix has
    been tested.
2.  **Win/lose message centering:** The message is centered using the
    field's height instead of its width. The fix has been identified as
    a one-word change in `EndFrame`.
3.  **High scores are not displayed:** High scores are saved, and
    `GetHighScore()` exists, but nothing currently calls it to display
    the result.
4.  **Brick Breaker and Snake save data:** Their `SaveExtras` and
    `LoadExtras` hooks exist but are not implemented. Loading Brick
    Breaker resets which bricks have been broken, and loading Snake
    rebuilds the body in a straight line.
5.  **Maze Load Game support:** Maze is not yet included in the Load
    Game list.
6.  **Settings persistence:** Mute state and the selected theme reset
    when the program closes because they are not saved to a file.
7.  **Unused JSON properties:** Some game JSON files still contain keys
    from earlier versions that nothing reads. These have been identified
    for cleanup.

## Conclusion

This round of development brought W.O.N.K.L.E. closer to its goal of
being a reusable, configuration-driven game framework. The shared engine
now supports five programs, with game behavior and much of the
player-facing content defined through JSON. The save system, sound
support, settings, individual game menus, and expanded test suite make
the launcher more complete and easier to extend.

The remaining work is focused on finishing persistence for game-specific
data, displaying high scores, saving settings between sessions, and
cleaning up older JSON properties.
