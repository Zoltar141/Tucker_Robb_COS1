#pragma once
#include <string>

class InputHelper
{
public:
   
    static int GetInt(const std::string& prompt, int min, int max); //menu and num input
    
    static bool IsKeyPressed();//realtime keyboard input
    static bool IsKeyDown(int key); // Checks whether a key is currently held.
    
    static char GetKey();// Retrieve input
};
