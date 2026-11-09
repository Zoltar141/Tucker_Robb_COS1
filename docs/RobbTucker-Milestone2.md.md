> Use this worksheet to plan the next phase of your project **before you begin coding**
> Be clear, specific, and intentional—this will guide your development this week.

---
## 📌 Project Overview

**Project Name:**
→   w.o.n.k.l.e.
  
**What does your program currently do? (1–3 sentences)**   
→The program currently provides a menu-driven console environment for running games and managing settings. It includes configurable themes, user navigation, and reusable systems designed to handle different aspects of gameplay. The goal is to create a foundation that can support multiple console games without having to rebuild common functionality for each one.

---
## 🔍 Current Progress Check  
  
**What is working right now?**   
→   The main menu and navigation are working, and the original Pong game is playable. The game has working rendering, paddle movement, ball movement, and basic gameplay. The existing ConsoleColor system also provides the foundation for controlling console colors. The core program is functional, but there are still areas that need refinement.
  
**What is NOT working or incomplete?**   
→   The Settings menu is still incomplete, and theme customization has not been fully implemented. Game colors are still largely hardcoded rather than controlled through a unified theme system. There are also some gameplay issues, including the AI being overly difficult and some delay in player paddle movement when holding input. Console rendering and cleanup when exiting the game could also use improvement.
  
**What feels confusing or messy in your code?**   
→   Some of the responsibilities between the Menu, Pong, ThemeManager, and rendering systems are starting to overlap. I'm still working through how these systems should communicate without making individual classes responsible for too much. Theme management is a particular area that needs attention, especially how the active theme gets accessed by the game and rendering systems. I also want to make the code easier to maintain as more functionality and settings are added

---
## 🚀 Feature Planning  
  
List the features you plan to add or improve this week.  
  
### Feature 1  
**Name:**   
→   Expanded Theme Customization
  
**What does this feature do?**   
→   Expand the existing theme system to allow users to customize the appearance of the console, including background colors, text colors, menu colors, and game element colors. Users will be able to select from different visual themes.
  
**Why is this feature important?**   
→   It gives users control over the appearance of the program while creating a consistent visual experience across different games and menus.
  
---
### Feature 2  
**Name:**   
→    Improved Paddle Input and Movement
  
**What does this feature do?**   
→   Improve how player input is processed and translated into paddle movement. This includes addressing input delays, improving movement responsiveness, and making paddle controls feel more consistent during gameplay.
  
**Why is this feature important?**   
→   Responsive controls are essential to a game like Pong, where timing and positioning directly affect gameplay. Improving input handling will make the game feel smoother and give players more reliable control over their paddles.
  
---
  
### Feature 3 (optional)  
**Name:**   
→   Modular Pong Architecture
  
**What does this feature do?**   
→   Break the existing Pong implementation into smaller, reusable components by separating gameplay logic, rendering, input handling, and other responsibilities into their own classes and source files.
  
**Why is this feature important?**   
→   The current Pong implementation handles several responsibilities within a single source file, making it harder to maintain and reuse functionality. Separating these systems will make the code easier to manage, allow individual components to be reused in other games, and establish a stronger foundation for expanding Wonkle.
  
---
## 🧩 System Design Updates  
  
**Will you need to create any new classes? If so, which ones?**   
→   Collision.h Position.h FileBoss.h ThemeManager.h Theme.h bunch more depending on what needs to be seperated.
  
**Will you modify any existing classes? How?**   
→   Yes. I plan to modify the existing Pong implementation by separating responsibilities that are currently handled together. This will involve changing how the Pong class handles rendering, collision detection, and other gameplay operations so that those responsibilities can eventually be handled by reusable components. The goal is to preserve the existing gameplay while making individual systems easier to maintain and reuse.
  
**What data structures will you use (vectors, 2D vectors, etc.)?**   
→   I plan to use vectors to manage collections of game objects and other reusable elements. I will also use structs to represent shared properties such as position, boundaries, and visual information. These structures should make it easier to pass information between classes without tightly coupling them to the Pong implementation.
  
---
## 🔄 Program Flow  
  
**Describe how a user interacts with your program:**  
  
1. Program starts →    Initializes the console, loads available settings, and displays the main menu.
2. User chooses →   Selects an option such as starting Pong, opening Settings, or exiting.
3. Program responds →   Launches the selected game, displays the settings menu, or exits the application.
4. Loop/next step →   Returns to the main menu when the user finishes a game or leaves Settings, allowing them to continue navigating until they choose to exit.
  
---
## 🎯 Usability Improvements  
  
How will you make your program easier to use this week?  
  
- Clearer prompts:   
→   Improve menu descriptions and on-screen instructions so users understand what each option does and how to interact with the program.
  
- Better error handling:   
→   Improve input validation and handling of unexpected user input to prevent invalid selections from interrupting the program or causing unexpected behavior. 
  
- Improved menu/navigation:   
→   Refine menu transitions and navigation so users can move between the main menu, game, and settings more easily, with consistent behavior when returning to previous menus.
  
---
## ⚠️ Potential Challenges  
  
**What do you think will be the hardest part this week?**   
→   The hardest part will likely be separating the existing Pong functionality into individual components without breaking how the game currently works. Some systems depend on information and functionality from other parts of the program, so changing how they communicate could introduce unexpected issues. Making those components reusable without tying them specifically to Pong will also require careful planning.
  
**What is your plan if you get stuck?**   
→   I'll work through the changes one component at a time, testing the game after each major change to make sure its existing functionality is preserved. If something stops working, I'll isolate the affected component and review how it communicates with the rest of the program. I'll also use Git to revert changes when necessary rather than continuing to build on top of broken code.
  
---
  
## 📈 Level Up Goal  
  
**What skill are you focusing on improving this week?**   
→   I'm focusing on improving my understanding of modular programming and how to design classes that can function independently while still communicating with other parts of a program. I want to get better at recognizing which responsibilities should be separated and how to make those components reusable.
  
**What will you do to improve it?**   
(e.g., tutorial, practice, debugging, office hours)   
→   I'll practice by breaking down the existing Pong implementation into smaller components, separating responsibilities such as rendering, collision detection, and gameplay logic. I'll test the game as I make changes to ensure everything continues working as expected. I'll also review how classes, objects, and their relationships are structured so I can make better architectural decisions as the project grows.
  
---
## 🗓️ Task Breakdown (GitHub Issues Planning)  
  
List the tasks you plan to create as GitHub Issues:  
  
- [ ]  Add theme selection functionality to the application Settings system. 
- [ ]   Create a ThemeManager class responsible for managing available themes and maintaining the currently active theme.
- [ ]   Refactor the rendering system to separate visual presentation from individual game logic.
- [ ]   Create a reusable Position class to manage coordinate information independently of individual game objects.
  
---
  
## 🔥 Final Check  
  
Before you start coding, ask yourself:  
  
- [x ] Do I know what I’m building this week?   
- [ x] Do I know where to start?   
- [ x] Did I break my work into small tasks?   
  
If yes → start coding 🚀   
If no → refine your plan first   
  
---
## 😈 Final Thought  
  
> Plan it now… or debug it later.