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

Stay tuned, this stand up is coming soon...

### Week 4

My final stand up...
