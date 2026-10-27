# Wonkle Pong — Mini-Wonkle Runtime Architecture

## 1. Project Overview

Wonkle Pong is a C++20 console-based Pong game powered by a small, reusable Wonkle runtime. The purpose of the runtime is to demonstrate how game behavior can be defined through state, conditions, rules, transformations, and simulation rather than being hardcoded into a game-specific engine.

Pong is the first application of the runtime. The architecture should allow other simple games to use the same core runtime without requiring Pong-specific code to be added to it.

## 2. Runtime State Representation

The runtime maintains the current state of the simulation. State represents the data that rules can inspect and transformations can modify.

For Wonkle Pong, the state will need to represent information such as:

- Ball position and movement.
- Paddle positions.
- Player scores.
- Playfield dimensions.
- Current game status.

These are examples of game data, not special properties that should be built directly into the generic runtime.

The runtime should provide a consistent way to access and update state values. Game-specific data should be supplied by the game configuration rather than requiring the runtime to understand what a ball, paddle, or score means.

## 3. Conditions and Rule Evaluation

Conditions determine whether a rule should execute based on the current state.

A rule consists conceptually of:

- Conditions that determine when the rule applies.
- Transformations that describe the changes to make when the conditions are satisfied.

The runtime evaluates rules against the current state and determines which transformations should be applied.

For example, Pong may define a rule that detects when the ball reaches a playfield boundary. The runtime evaluates the condition without needing to understand the meaning of a ball or a wall.

Rule evaluation should be predictable and independent of the specific game using the runtime.

## 4. State Transformations

Transformations describe changes to state when rules execute.

Examples of transformations include:

- Updating a numeric state value.
- Changing a movement direction.
- Incrementing a score.
- Changing the current game status.

These examples describe possible operations, not requirements for separate runtime functions for every Pong mechanic.

The generic runtime should execute supported transformation operations without embedding game-specific decisions into the transformation processor.

Game-specific behavior belongs in the rules and data supplied by the game.

## 5. Simulation Flow

The simulation represents the process of advancing the game from one state to the next.

The proposed simulation cycle is:

1. Receive the current state and any relevant input.
2. Evaluate the applicable rules against the state.
3. Determine which transformations should execute.
4. Apply the transformations according to a defined execution order.
5. Produce the updated state.
6. Allow the game to render the resulting state.

The runtime should define consistent behavior when multiple rules apply during the same cycle. The exact execution order and handling of conflicting transformations must be established during implementation.

Console rendering and keyboard input should remain separate from the generic rule-evaluation system.

## 6. Persistence Responsibilities

Persistence is responsible for saving and restoring state when required by the project.

Its responsibilities include:

- Converting supported state data into a persistent representation.
- Writing saved data to the selected storage format.
- Reading saved data and reconstructing the state.
- Handling missing, malformed, or incompatible saved data appropriately.

Persistence should not contain Pong-specific rules. The game determines which state needs to be preserved, while the persistence mechanism handles the storage and recovery process.

The final serialization format and exact persistence requirements will be determined during implementation.

## 7. Component Responsibilities

The runtime should be organized around clearly separated responsibilities.

| Component | Responsibility |
|---|---|
| State representation | Stores and provides access to simulation data. |
| Condition evaluation | Determines whether rule conditions are satisfied. |
| Rule processing | Selects and coordinates applicable rules. |
| Transformation processing | Applies supported state changes. |
| Simulation control | Coordinates the runtime update cycle. |
| Persistence | Saves and restores supported state data. |
| Pong game configuration | Defines the state, rules, and transformations needed for Pong. |
| Input handling | Converts player input into supported game actions. |
| Console renderer | Displays the current game state without modifying it. |

These are logical responsibilities. They do not necessarily require a separate class or source file for every row.

## 8. Separation Between Wonkle and Pong

The runtime must remain independent of Pong-specific concepts.

The runtime should not contain hardcoded ball physics, paddle controls, scoring rules, or Pong-specific collision logic. Instead, the Pong implementation supplies the state and rules that describe those behaviors.

This separation allows the same runtime to support another game by supplying different data and rules rather than rewriting the runtime.

## 9. Architecture Acceptance Criteria

- [ ] State representation and access responsibilities are documented.
- [ ] Condition evaluation and rule processing are documented.
- [ ] Transformation processing is documented.
- [ ] The simulation update cycle is documented.
- [ ] Persistence responsibilities are documented.
- [ ] Responsibilities are separated between the generic runtime and Pong.
- [ ] The architecture supports Pong without embedding Pong-specific behavior in the runtime.
- [ ] The proposed design is checked against the existing project structure before implementation.

## 10. Implementation Notes

This document describes the intended architecture, not proof that the runtime has already been implemented.

Before creating or changing source files, review the existing repository and course requirements. Preserve any established interfaces, build configuration, testing conventions, and milestone constraints.

Implementation decisions should be documented and updated as the design becomes concrete.
