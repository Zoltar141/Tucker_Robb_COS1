#pragma once
#include <string>

class InputHelper
{
public:
   
    static int GetInt(const std::string& prompt, int min, int max); //menu and num input
    
    static bool IsKeyPressed();//realtime keyboard input
    
    static char GetKey();// Retrieve input
};
