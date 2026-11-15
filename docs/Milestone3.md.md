
This week, I focused on moving Wonkle beyond a Pong-only project and developing it into a reusable console game framework. The biggest changes were separating Pong's configuration from its implementation, introducing a shared `GameController` class, and integrating Brick Breaker and Snake using the framework's shared systems.

## ⚙️ Features Added

- **Brick Breaker:** Added Brick Breaker as a second playable game. Its configuration is stored in `brickbreaker.json`, including its settings, initial state, rules, rendering information, and collision-related configuration. Brick durability is represented through the brick layout and rule behavior rather than requiring a separate class for every brick.
- **Snake:** Added Snake as a third playable game. Its configuration is stored in `snake.json`. The game uses a body-tick representation to manage the snake's segments, timed movement, direction controls, food placement, scoring, and win/loss conditions.
- **Game selection:** Updated the menu flow so the player can choose between Pong, Brick Breaker, and Snake.
- **Shared rendering:** Added a frame-buffer approach that compares the current frame with the previous frame and redraws changed cells, reducing unnecessary console output and visual flicker.
- **JSON-based themes:** Added `themes.json` so theme colors can be configured as data instead of being individually hardcoded into each game.

## 🧱 Updates to System Design

- **Created `GameController`:** Introduced a common base class for the games. It provides shared game-controller responsibilities and gives the individual games a consistent lifecycle and structure. Pong, Brick Breaker, and Snake now build on this shared controller instead of each having to independently organize the same common game flow.
- **Separated Pong configuration into `pong.json`:** Moved Pong's game configuration out of its C++ implementation and into a dedicated JSON file. This separates configuration data—such as settings, controls, state, rules, and rendering roles—from the code that runs the game.
- **Expanded the Wonkle runtime:** Continued using `WonkleRuntime` and `GameState` as shared systems for storing game state, evaluating conditions, and applying transformations. The runtime is intended to process configured rules without needing to know which specific game is using it.
- **Separated rendering from game logic:** Added the shared `Render` system so games can submit objects for drawing without each implementing its own console-rendering process.
- **Centralized theme management:** Added `Theme` and `ThemeManager` to load themes from JSON and provide semantic visual roles to the renderer. Games request roles such as background, boundary, paddle, ball, or brick instead of selecting console colors directly.
- **Separated common supporting systems:** Organized collision checks, boundary handling, position data, file access, and input validation into their own components (`Collision`, `Boundary`, `Position`, `FileBoss`, and `InputHelper`).

## 🧹 Refactoring Improvements

- **Removed Pong-specific configuration from the Pong implementation:** Pong's settings, controls, initial state, rules, and rendering-role configuration are now defined in `pong.json`. This reduces the amount of configuration embedded in `PONG.cpp` and makes the same general configuration-driven approach available to the other games.
- **Reduced duplicated game-flow responsibilities:** The new `GameController` provides a shared structure for the three games. Each game retains its own mechanics, but common controller responsibilities no longer need to be independently structured in every game.
- **Moved reusable rendering work into `Render`:** Instead of having each game manage its own complete drawing process, the games use the shared renderer. The renderer owns frame dimensions, drawing objects, and the current/previous frame buffers.
- **Kept game-specific mechanics in their respective game classes:** Brick Breaker and Snake use the common framework but retain their own update, input, and gameplay behavior. This keeps the shared systems reusable without forcing unrelated game mechanics into the controller or runtime.

## 💭 What I Changed and Why

Previously, Wonkle was centered around Pong. I wanted to find out whether the systems being developed could support more than one game without duplicating the same infrastructure. I first separated Pong's configuration into `pong.json`, which established a clearer boundary between game data and game code.

I then introduced `GameController` to provide a common structure for running games. Brick Breaker and Snake were added as additional implementations using that shared structure. This was an important step because it gave me a way to test the framework against games with different mechanics: Brick Breaker uses rows of destructible bricks and collision behavior, while Snake uses a growing body, timed movement, and food placement.

I also separated rendering and theme management from the individual games. The shared renderer uses a frame buffer to avoid redrawing unchanged cells, while the theme system resolves semantic roles from JSON-defined themes. Supporting systems such as collision, boundaries, file handling, and input validation were also organized into separate components.

The reason for these changes was to make Wonkle more reusable and easier to extend. Instead of building each game as a completely separate program, I am building shared infrastructure that multiple games can use while keeping their unique mechanics separate. Adding Brick Breaker and Snake gives me a practical way to evaluate that design and identify which parts of Wonkle are genuinely reusable.