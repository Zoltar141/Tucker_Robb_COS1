# Instructions

Update this document where indicated [look for the brackets!]. Replace text inside the brackets with your own information. For example: Course Name should be the name of this course, and not the generic words "Course Name".

<br>

## [ Project & Portfolio 1 ]

- **[ Robb Tucker ]**
- **[ Nov 1, 2026 ]**

This paper addresses some of the topic matter covered in research and activity this week. Be sure to include reference links below to the research and information you used to complete this assignment.

## Topic: Terminal

Professional developers use Terminal daily. It's essential to understand some fundamental commands to use the application.

Update the information below to demonstrate your knowledge on this topic.

**1. Using Terminal, there are essential commands to know.**

List the correct Terminal commands to do the actions listed below. Replace **CMD** with the correct command sequence. You can keep or enhance the brief description.

**The last bullet provides an example**.

- [ cls ]: Clear the Screen
- [ cd ]: Print the "Working Directory"
- [ dir ]: List files and folders
- [ dir /a ]: List files and folders, including invisible files
- [ dir /a | more ]: List all files and folders, in human readable form
- [ cd foldername ]: Change directory
- [ cd \ ]: Change directory, go to root directory
- [ cd %USERPROFILE% ]: Change directory and go to user home directory
- [ cd ..]: Change directory, go up one folder level
- [ cd ../.. ]: Change directory, go up two folder levels
- [ cd %USERPROFILE%/Desktop ]: Change directory to my desktop!

**2. Using Terminal...**

**Folder Drop:** Try typing "cd" followed by a space, and then drag a folder into terminal and press return. Test this out and describe your results below.

[ After dragging the folder into Command Prompt and pressing Enter, the working directory changed to the selected folder. The command prompt now displays the folder's full path, showing that CMD is operating from that location instead of the original directory. ]

## Topic: Version Control & Git

Version control, also known as revision control, records changes to a file or set of files over time so that you can recall specific versions later. In this class, we are learning Git. Update the information below where indicated.

**1. There are three types of version control.**

[ 1. **Local Version Control:** Tracks changes to files on a single computer. It allows users to save different versions and restore previous versions.

2. **Centralized Version Control:** Stores project files and revision history on a central server. Multiple developers can access and collaborate on the same project.

3. **Distributed Version Control:** Gives each developer a complete copy of the project's files and version history. Developers can work independently and share changes with others. Git is a popular example.]

**2. Using Terminal, there are also essential Git commands to know.**

List the correct Git commands to do the actions listed below in Terminal. Replace CMD with the correct command and keep or enhance the brief description.

- [ git clone REPOSITORY_URL ]: Clone a repository
- [ git config --global user.name "Name" ]: Set-up a global user name
- [ git config --global user.email "name@email.com" ]: Set-up a global email address (to match my GitHub account email)
- [ git status ]: Shows the current state of your directory and staging area
- [ git add . ]: Add modified files to the next commit
- [ git commit -m "Message" ]: Make a commit with a new message
- [ git log ]: Show my commit history
- [ git help ]: Show Git's help screen

**3. Connecting to GitHub using Terminal.**
HTTPS is the the correct way to connect to GitHub in this course. Describe how you connect to GitHub from Terminal using this protocol. What steps do you take?

[ To connect to GitHub using HTTPS, I first create a repository on GitHub and copy its HTTPS URL. In Terminal, I navigate to the folder where I want the project stored and use `git clone` followed by the repository URL. If I am connecting an existing local project, I use `git init` if needed, then `git remote add origin` followed by the HTTPS URL. When prompted, I authenticate using my GitHub credentials or a Personal Access Token. Once connected, I can use Git commands to push and pull changes between my local project and GitHub. ]

**4. Using .gitignore and Why it's Important**  
Most repositories contain a .gitignore file.

- What is the purpose of this file?
  <br>
  [A .gitignore file tells Git which files and folders to ignore so they aren't accidentally added to the repository. This helps keep unnecessary files, temporary files, and private information out of the project.]

- What is the "**.DS_Store**" file and why would you want to ignore it?
  <br>
  [.DS_Store is a hidden file automatically created by macOS to store folder display settings, such as icon positions and window layout. It isn't needed for the project to function, so ignoring it prevents unnecessary files from being added to the repository.]

- What other file or folder would you want to add to a .gitignore file and why?
  <br>
  [I would add the build and output folders to .gitignore because they contain compiled files and generated data that can be recreated from the source code. This keeps the repository cleaner and prevents unnecessary files from being tracked.]

<br>

# Reference Links

Replace the example references below with your own links and recommended resources. It is acceptable to provide multiple links for a single topic and to use material provided to you in this class. You are encouraged to link to your own independent research as well.

[ The GitHub documentation was the most helpful resource this week because it provided practical examples of Git commands and explained how Git connects a local project to a remote repository. Being able to see the commands and understand what each one does made it easier to apply them to my own projects. I also found the information about version control useful because it helped explain how developers track changes, maintain project history, and collaborate without losing previous work. ]

**Terminal Commands**  
[Microsoft Windows Commands](https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/windows-commands)

**Three Types of Version Control**  
[Atlassian - Types of Version Control](https://support.atlassian.com/bitbucket-cloud/docs/types-of-version-control/)

**Git Commands**  
[GitHub - About Git](https://docs.github.com/en/get-started/using-git/about-git)

**Connecting to GitHub using Terminal**  
[GitHub - About Authentication to GitHub](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/about-authentication)

**Using .gitignore and Why it's Important**  
[GitHub - Ignoring Files](https://docs.github.com/en/get-started/git-basics/ignoring-files?utm_source=chatgpt.com)
