# 🚀 Welcome to Project & Portfolio!
# Project W.O.N.K.L.E. PONG

Robb Tucker

Hello my name is [Robb Tucker]. I am a student from [Emmett, Idaho]. The purpose of this repository is to practice development using version control. This work will help me begin to build a portfolio of skills and accomplishment that can be shared in the future.

<br>

## 📢 &nbsp; Weekly Stand Up

Each week I will summarize my milestone activity and progress by writing a stand-up. A stand-up is meant to be a succinct update on how things are going. Use these prompts as a guide on what to write about:

⚙️ Overview - What I worked on this past week
<br>
🌵 Challenges - What problems did I have & how I'm addressing them
<br>
🏆 Accomplishments - What is something I "leveled up" on this week
<br>
🔮 Next Steps - What I plan to prioritize and do next

<br>

### Week 1

This week, I focused on building the foundation of Wonkle Pong. I created the WonkleRuntime and GameState classes, started implementing the runtime's state management and rule evaluation, and built a menu system with an InputHelper to handle user input. I also created tests to verify that the runtime can evaluate conditions and apply transformations correctly.

One challenge has been figuring out how to connect the runtime and GameState without making the system unnecessarily complicated. I've also had to work through some input-handling issues, particularly when users enter invalid data. I'm addressing these by testing individual components as I develop them.

I've made progress understanding how the different parts of the runtime work together. Getting my first rule-based transformation to execute successfully and return the expected result was a big milestone. I've also established my GitHub repository, development branches, project board, and issues to keep development organized.

My next priority is continuing development of the WonkleRuntime, expanding its rule evaluation and transformation capabilities, and connecting those systems to the actual Pong gameplay. I'll also continue testing each component before moving on to the next milestone.

### Week 2

 Overview

This week I spent most of my time working on the internal structure of Wonkle. A lot of the work involved cleaning up existing systems and figuring out how to make them communicate without creating unnecessary dependencies. I continued working on the theme system, rendering, FileBoss, and the runtime itself.

 Challenges

One of the biggest challenges was getting the different systems wired together properly. As I started separating responsibilities, I ran into some issues with how components accessed information from one another. The theme system was a good example of this, particularly getting the active theme's colors to the rendering system. I also spent some time investigating the paddle input delay in Pong and tracking down where the problem was coming from.

 Accomplishments

I made progress toward moving theme definitions into JSON, continued working on the connection between ThemeManager and rendering, and expanded the functionality and testing of WonkleRuntime. I added tests for more complex state references and transformations, including arithmetic operations, comparisons, and chained rules. I also continued developing FileBoss as the central place for file operations rather than scattering file-handling responsibilities throughout the application.

One thing I've learned this week is that separating code into reusable components is only part of the job. Getting those components to communicate properly without becoming dependent on one another is where a lot of the architectural work happens.

Next Steps

My next priority is to finish testing the changes I've made and make sure the different components are working together as intended. I also want to start putting the framework through its paces outside of Pong. My plan is to use Brick Breaker as a test application to see how well the existing components can support a completely different game. This should help identify any remaining dependencies on Pong-specific functionality and show me where the framework still needs work. I'd also like to use that process to identify areas where the components could be made more flexible without adding unnecessary complexity..

### Week 3

 Overview

This week, I focused on expanding Wonkle from a Pong-focused project into a reusable console game framework. I added Brick Breaker and Snake as additional games and worked on building shared systems that allow all three games to operate within the same framework.

A major part of this work was separating game-specific functionality from the systems responsible for running and supporting each game.

 Improvements

- **Separated Pong configuration:** Moved Pong's game settings, rules, controls, rendering roles, and other configuration data into its own `pong.json` file. This reduces hardcoded game-specific information inside the implementation.
    
- **Created GameController:** Introduced a shared `GameController` class to manage common game functionality and provide a consistent structure for individual games.
    
- **Added Brick Breaker:** Implemented Brick Breaker using the shared game architecture, including its own configuration, rules, collision behavior, and brick durability.
    
- **Added Snake:** Implemented Snake using the shared framework, including movement, body tracking, food spawning, scoring, and collision behavior.
    
- **Separated rendering:** Developed a shared rendering system with frame buffering to reduce console flickering and avoid unnecessary redraws.
    
- **Expanded theme support:** Added JSON-driven themes and semantic color roles so games can share visual customization without hardcoding individual colors.
    
- **Organized shared systems:** Separated responsibilities into dedicated components for collision detection, boundaries, positioning, file handling, input, and runtime state.
    

 Challenges

One of the biggest challenges was integrating multiple games without allowing the framework to become dependent on the mechanics of any particular game.

Pong, Brick Breaker, and Snake all handle movement, collisions, and game states differently. I had to determine which responsibilities belonged in shared systems and which needed to remain specific to each game.

Another challenge was moving configuration data out of individual implementations while maintaining compatibility with the existing runtime and rendering systems. I addressed this by using separate JSON configurations for each game and keeping shared functionality in reusable classes.

I also worked through rendering issues involving console flickering and unnecessary screen updates, which led to implementing a frame buffer.

 Accomplishments

My biggest accomplishment this week was successfully expanding Wonkle to support three different games through a shared architecture.

I gained a better understanding of separating class responsibilities, designing reusable systems, and using configuration files to make software more flexible.

I also improved my ability to recognize the difference between code that is reusable and code that is simply duplicated across multiple implementations.

The addition of `GameController` and `WonkleRuntime` represents an important step toward making Wonkle a framework rather than a collection of individual games.

Next Steps

Before Week 4, I plan to focus on:

- Completing and refining the integration of Brick Breaker and Snake.
    
- Expanding and validating the shared runtime and configuration systems.
    
- Improving consistency across the three game implementations.
    
- Testing shared systems to identify issues that may affect multiple games.
    
- Continuing work on persistence, including saving and loading game state.
    
- Preparing the framework for future games without requiring major changes to its underlying architecture.
    

The overall goal is to make adding new games increasingly straightforward by allowing them to reuse Wonkle's existing systems rather than rebuilding functionality for every project.
### Week 4

My final stand up...
