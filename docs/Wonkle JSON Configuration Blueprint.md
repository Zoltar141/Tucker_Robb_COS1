**Purpose:** the standard structure for a Wonkle game file, like `pong.json` or `maze.json`. It matches what `GameController`, `GameObjects` and `Menu` actually read. If a key isn't listed here, the engine ignores it.

How a file gets used: the Choose Program menu starts a game with `GameObjects game(themeManager, "yourgame.json")`. The file name without `.json` becomes the game's save slot and high score slot, so `yourgame.json` saves under `"yourgame"`.

There are three kinds of pieces:

- **REQUIRED**: every game needs it. If it's missing, the game stops with an error or crashes when it starts or when it draws.
- **FEATURE**: a section that switches something on (ball, paddles, bricks, snake, text, sounds). Leave it out and that part of the engine is skipped. Put it in and everything listed for it becomes required.
- **OPTIONAL**: has a default or only adds something extra.

---

## 1. Required core (every game)

Copy this, fill in the `FILL_IN` parts and keep the standard values unless you have a reason to change them. Everything sits inside `"game"`.

```json
{
  "game": {
    "menu": {
      "art": [
        "FILL_IN_ART_LINE",
        "FILL_IN_ART_LINE"
      ],
      "description": "FILL_IN_DESCRIPTION",
      "controls": "FILL_IN_CONTROLS_SCREEN_TEXT"
    },

    "field": {
      "width": 80,
      "height": 25,
      "left_edge": 0,
      "right_edge": 79,
      "top": 0,
      "bottom": 24
    },

    "timing": {
      "frame_delay_ms": 50
    },

    "controls": {
      "exit": 27,
      "restart": 82,
      "display_text": "FILL_IN_BOTTOM_LINE_TEXT"
    },

    "initial_state": {
      "game_won": 0,
      "game_lost": 0,
      "FILL_IN_STATE_NAME": 0
    },

    "rules": [
      {
        "name": "FILL_IN_RULE_NAME",
        "conditions": [
          { "key": "FILL_IN_STATE_NAME", "comparison": "FILL_IN_COMPARISON", "value": 0 }
        ],
        "transformations": [
          { "key": "FILL_IN_STATE_NAME", "operation": "FILL_IN_OPERATION", "value": 0 }
        ]
      }
    ],

    "rendering": {
      "extra_height": 4,
      "world_vertical_offset": 2,
      "scoreboard_column": 0,
      "scoreboard_row": 0,
      "controls_column": 0,
      "controls_row_offset": 3,

      "roles": {
        "background": "Background",
        "border": "Border",
        "ui": "UI"
      },

      "ascii": {
        "top_border": "_",
        "bottom_border": "-",
        "vertical_border": "|"
      },

      "scoreboard": {
        "separator": "   ",
        "entries": [
          { "label": "FILL_IN_LABEL:", "value": "FILL_IN_STATE_NAME" }
        ]
      },

      "messages": {
        "player_wins": "FILL_IN_WIN_TEXT",
        "player_loses": "FILL_IN_LOSE_TEXT",
        "restart_prompt": "    R: Restart    ESC: Menu"
      }
    }
  }
}
```

### What each required piece does

| Key | Type | What it's for |
|---|---|---|
| `menu.art` | list of text lines | Printed at the top of the game's menu screen. |
| `menu.description` | text | Printed under the art. |
| `menu.controls` | text | Shown when the player picks "2. Controls". |
| `field.width`, `field.height` | numbers | The size of the play area. |
| `field.left_edge`, `right_edge`, `top`, `bottom` | numbers | The walls. These are what the ball bounces off and what the border is drawn on. |
| `timing.frame_delay_ms` | number | The pause between frames. Lower is faster. |
| `controls.exit` | key code | Pauses during play, and leaves at the game over prompt. Keep it at **27** (ESC). |
| `controls.restart` | key code | Restarts at the game over prompt. It has to be an **uppercase letter code** (`82` is R). |
| `controls.display_text` | text | The line under the field during play. |
| `initial_state` | name: number | **Every value the game keeps**, and what it starts at. Only names listed here get saved, loaded and reset on restart. Decimals are allowed. |
| `initial_state.game_won` / `game_lost` | 0 | Your rules set one of these to anything other than 0 to end the game. Without them the game can never end. |
| `rules` | list | The game's behavior, see section 2. Use `[]` if there are none. |
| `rendering.extra_height` | number | Extra rows under the field for the controls line. |
| `rendering.world_vertical_offset` | number | How far down the field is pushed, which leaves room for the scoreboard. |
| `rendering.scoreboard_column` / `scoreboard_row` | numbers | Where the scoreboard line starts. |
| `rendering.controls_column` / `controls_row_offset` | numbers | Where the controls line goes. Its row is `field.height + controls_row_offset`. |
| `rendering.roles.background` / `border` / `ui` | theme role | The colors for the background, the border, and all text. |
| `rendering.ascii.top_border` / `bottom_border` / `vertical_border` | **text** | The border characters. Only the first character is used. |
| `rendering.scoreboard.entries` | list | Each entry shows `label` followed by the current value of the state named in `value`. Decimals show up to 2 places. |
| `rendering.scoreboard.separator` | text | Goes between entries. |
| `rendering.messages.player_wins` / `player_loses` | text | Shown in the middle of the field when the game ends. |
| `rendering.messages.restart_prompt` | text | Added to the controls line when the game ends. Only R and ESC work at that prompt, so say so. |

### Optional extras in the core

| Key | What it does |
|---|---|
| `initial_state.score` | If the state has a value named `score`, the game keeps a **high score** for it. |
| `rendering.messages.final_score` | Text shown before the score on the game over line, like `"  Final Score: "`. |
| `name` (inside `game`) | Just a label for you. Nothing reads it. |

---

## 2. Rules

Every update the rules run top to bottom. A rule runs when **all** of its conditions are true, and then each of its transformations happens in order. A later rule sees what an earlier one changed in the same update.

```json
{
  "name": "FILL_IN_RULE_NAME",
  "conditions": [
    { "key": "ball_position_x", "comparison": "GreaterEqual", "value": { "state": "goal_x" } }
  ],
  "transformations": [
    { "key": "score", "operation": "Add", "value": 1 },
    { "key": "ball_position_x", "operation": "Assign", "value": 40 }
  ]
}
```

| Piece | Allowed values |
|---|---|
| `conditions` / `transformations` | **Both are required**, use `[]` when empty. An empty `conditions` list means the rule runs every update. |
| `key` | Any state name. |
| `comparison` | `Equal`, `NotEqual`, `Less`, `LessEqual`, `Greater`, `GreaterEqual`. Spelling matters. A typo stops the game from starting, and the menu shows the error. |
| `operation` | `Assign`, `Add`, `Subtract`, `Multiply`, `Divide`. |
| `value` | A number (`5`, `0.5`), or `{ "state": "other_name" }` to use another state value. |
| `name` | A label for you. Nothing reads it. |

Behavior to know:
- A condition on a value that doesn't exist is false, so the rule doesn't run.
- `Assign` can create a brand new value. `Add`, `Subtract`, `Multiply` and `Divide` do nothing to a value that doesn't exist.
- `Divide` by 0 leaves the value alone.
- Using `{ "state": "missing" }` in a transformation does nothing.

---

## 3. Feature sections

Add one of these only when the game uses it. Each lists the state values and rendering entries it needs.

### Sounds (OPTIONAL)

When a listed state value changes, its wav plays. Only one sound plays at a time, and the first one in the list wins, so put win/lose first. A missing wav file is skipped without any error.

```json
"sounds": [
  { "key": "game_won", "file": "winner.wav" },
  { "key": "game_lost", "file": "loser.wav" },
  { "key": "FILL_IN_STATE_NAME", "file": "FILL_IN_FILE.wav" }
]
```

### Movement interval (OPTIONAL, inside `timing`)

```json
"timing": { "frame_delay_ms": 20, "movement_interval_ms": 120 }
```
The game only moves once this many milliseconds have passed, while input is still read every frame. Snake uses it.

### Ball (FEATURE)

```json
"ball": {
  "bounces_off": [ "top", "bottom" ],
  "bounce_multiplier": -1,
  "keep_inside": [ "top", "bottom" ]
}
```
- `bounces_off`: the walls the ball bounces off. Any of `left`, `right`, `top`, `bottom`.
- `bounce_multiplier`: what the velocity gets multiplied by on a bounce. `-1` is a normal bounce.
- `keep_inside` (optional): walls the ball can never go past.
- **Needs in `initial_state`:** `ball_position_x`, `ball_position_y`, `ball_velocity_x`, `ball_velocity_y`.
- **Needs in `rendering`:** `ascii.ball` (text) and `roles.ball`.

### Paddles (FEATURE)

A list. Each paddle is a block that moves along one axis. Anything that moves around on the field (a player in a maze, a cursor, a marker) is a paddle too.

```json
"paddles": [
  {
    "position": "FILL_IN_STATE_NAME",
    "moves_along": "y",
    "fixed_at": 2,
    "width": 1,
    "height": 5,
    "limits": [ 0, 20 ],

    "step": 1,
    "keys": { "back": 87, "forward": 83 },

    "bounce_velocity": 1,
    "bounce_angles": [ -1, -1, null, 1, 1 ],

    "draw_as": "paddle",
    "draw_width": 1
  }
]
```

| Key | Required? | What it does |
|---|---|---|
| `position` | REQUIRED | The state value that holds where the paddle is. It also has to be in `initial_state`. |
| `moves_along` | REQUIRED | `"x"` or `"y"`. |
| `width`, `height` | REQUIRED | The size of the space the ball can hit, and the size it's drawn at. |
| `limits` | REQUIRED | `[min, max]` for `position`. It's enforced every update. |
| `fixed_at` | REQUIRED unless it has `other_axis` | Where it sits on the axis it doesn't move along. |
| `step` + `keys.back` / `keys.forward` | OPTIONAL | Player control. Leave `keys` out and only the rules move it, like the Pong opponent. |
| `bounce_velocity` | REQUIRED **if the game has a ball** | The ball's speed away from the paddle after a hit. |
| `bounce_angles` | REQUIRED **if the game has a ball** | One entry per space along the paddle, giving the ball's new speed along the paddle. `null` keeps the speed it had. |
| `draw_as` | OPTIONAL (defaults to `"paddle"`) | Which `ascii` and `roles` entry to draw it with. |
| `draw_width` | OPTIONAL (defaults to `width`) | Draws it narrower than its hit area. |
| `other_axis` | OPTIONAL | Makes it move on both axes, for example `{ "position": "player_y", "step": 1, "limits": [0, 20], "keys": { "back": 87, "forward": 83 } }`. |

- **Needs in `rendering`:** `ascii.<draw_as>` (text) and `roles.<draw_as>` for every paddle, which is `ascii.paddle` and `roles.paddle` by default.

### Bricks (FEATURE)

```json
"bricks": {
  "origin_x": 1,
  "origin_y": 3,
  "width": 78,
  "layout": [
    "5500005500",
    "0011110000"
  ],
  "hit_by": { "x": "player_x", "y": "player_y" }
}
```
- `layout`: one line of text per row. Each character is how many hits that brick has left: `0` is empty, `1` to `9` is a brick. Short rows get filled out to `width` with `0`.
- `hit_by` (optional): which state values do the hitting. Without it the ball does. Maze uses it for its walls.
- Each update the engine puts the hits left on the brick being touched into `brick_hits`, and your rules decide what happens. Breaker's rules subtract 1 and add to the score, and Maze's rules push the player back.
- The engine sets `bricks_remaining` itself when the game starts.
- **Needs in `initial_state`:** `brick_hits`, `bricks_remaining`.
- **Needs in `rendering`:** `ascii.brick` as a **number** (a character code, e.g. `219` is a solid block), plus `roles.brick_1`, `roles.brick_2` and so on for every hit count used in the layout.

### Snake (FEATURE)

```json
"snake": {
  "columns": 39,
  "rows": 20,
  "cell_width": 2,
  "origin_x": 1,
  "keys": { "up": 87, "down": 83, "left": 65, "right": 68 }
}
```
- A grid of `columns` x `rows`. Each grid space is drawn `cell_width` characters wide so it looks square.
- **Needs in `initial_state`:** `head_x`, `head_y`, `direction_x`, `direction_y`, `requested_direction_x`, `requested_direction_y`, `snake_length`, `food_x`, `food_y` (start both at `-1`, which means "place food"), and `body_under_head`.
- **Needs in `rendering`:** `ascii.snake_head`, `ascii.snake_body` and `ascii.food` as **numbers** (character codes), plus `roles.snake_head`, `roles.snake_body` and `roles.food`.

### Text on the field (FEATURE)

```json
"text": [
  { "x": 1, "y": 14, "text": "+", "role": "FILL_IN_ROLE_NAME" },
  { "x": { "state": "a_dial" }, "y": { "state": "b_dial" }, "value": "answer", "role": "FILL_IN_ROLE_NAME" },
  { "x": 1, "y": 14, "text": "!", "when": "error", "role": "FILL_IN_ROLE_NAME" }
]
```
- `x`, `y`: a number, or `{ "state": "name" }` to follow a state value.
- `text` shows fixed text, and `value` shows a state value instead.
- `when` (optional): only shown while that state value isn't 0.
- `role`: a key **in `rendering.roles`**, not a theme role directly.
- Text is drawn last, on top of everything else.

---

## 4. Characters: text or number

Different parts of the engine read `rendering.ascii` in different ways. Using the wrong type stops the game with a json error the first time that thing is drawn.

| Write it as **text** (`"O"`) | Write it as a **number** (`219`) |
|---|---|
| `top_border`, `bottom_border`, `vertical_border` | `brick` |
| `ball` | `snake_head`, `snake_body`, `food` |
| `paddle` and every other `draw_as` name | |

---

## 5. Key codes

| Key | Code | | Key | Code |
|---|---|---|---|---|
| W | 87 | | R | 82 |
| A | 65 | | J | 74 |
| S | 83 | | K | 75 |
| D | 68 | | ESC | 27 |

- Movement keys (`paddles.keys`, `snake.keys`) are checked while held down. Any letter's uppercase code works, and so do the arrow keys: Left 37, Up 38, Right 39, Down 40.
- `controls.restart` and `controls.exit` are read as typed keys, so stick to uppercase letter codes and 27 for ESC. Arrow keys **won't** work there.

---

## 6. Theme roles

Every value in `rendering.roles` has to be a role name from `themes.json`:

`Background`, `Foreground`, `MenuText`, `MenuHighlight`, `Border`, `GameBoundary`, `Paddle`, `Ball`, `SnakeBody`, `SnakeFood`, `UI`, `Brick1`, `Brick2`, `Brick3`, `Brick4`, `Brick5`

A name that isn't in the theme shows up white on black.

---

## 7. Placeholder reference

| Placeholder | Replace with |
|---|---|
| `FILL_IN_ART_LINE` | One line of the menu's title art |
| `FILL_IN_DESCRIPTION` | A line or two about the game |
| `FILL_IN_CONTROLS_SCREEN_TEXT` | The full controls text for the Controls screen |
| `FILL_IN_BOTTOM_LINE_TEXT` | The short controls line under the field, like `"W/S: Move    ESC: Exit"` |
| `FILL_IN_STATE_NAME` | A state value name, lowercase with underscores (`player_score`) |
| `FILL_IN_RULE_NAME` | A label for the rule (`player_scores`) |
| `FILL_IN_COMPARISON` | One of the six comparisons in section 2 |
| `FILL_IN_OPERATION` | One of the five operations in section 2 |
| `FILL_IN_LABEL:` | The scoreboard label (`SCORE:`) |
| `FILL_IN_WIN_TEXT` / `FILL_IN_LOSE_TEXT` | The game over messages |
| `FILL_IN_ROLE_NAME` | A key you added to `rendering.roles` |
| `FILL_IN_FILE.wav` | A wav file in the project folder |

---

## 8. Guidelines

1. Name state values in lowercase with underscores, and use the exact names the engine expects for features (section 3).
2. Every value the game keeps goes in `initial_state`, even ones the rules or the engine set later. Anything left out won't be saved, loaded or reset on restart.
3. Put behavior in `rules` wherever the runtime can do it. Feature sections handle movement and collision, and rules decide what those mean.
4. `rendering.roles` maps your names to theme roles. Never put a color name there.
5. Check section 4 before writing any `ascii` entry.
6. Every `draw_as`, text `role`, `brick_N`, `ball` and snake part needs its `ascii` and `roles` entry.
7. Leave out feature sections the game doesn't use. Don't add empty ones.
8. Don't add keys nothing reads. A key the engine doesn't know does nothing, which makes it easy to think you've configured something when you haven't.
9. Make sure the file is valid json. A broken file stops the game from loading, and the menu shows the error.
10. A new key only means something once the C++ reads it. Add the code first, then document the key here.

---

## 9. Adding a new game

1. Copy the required core into `newgame.json` in the project folder.
2. Add the feature sections it needs, along with their state values and rendering entries.
3. Add it to the Choose Program list in `Menu.cpp` (`HandleSelection`, case 1).
4. Add `"newgame.json"` to `GAME_FILES` at the top of `Tests.cpp` and run `RunAllTests()`. That checks the json loads, the rules load, the menu text is there, every wav exists and the game builds.
5. If it should show up in Load Game, add it to the list in `RunLoadMenu`.
