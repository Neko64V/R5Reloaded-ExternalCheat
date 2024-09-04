#pragma once
#include <Windows.h>

struct Globals
{
    // System
    bool Run = false;
    bool ShowMenu = false;

    // GameData
    HWND GameHwnd;
    RECT GameRect{};
    POINT GamePoint{};

    // AimBot
    bool AimBot   = true;
    bool Aim_NPC  = false;
    bool Aim_Team = false;
    bool Aim_VisCheck   = true;
    bool Aim_Prediction = true;
    bool Aim_NoSway     = true;
    bool Aim_DrawFov    = true;
    float Aim_Fov = 150.f;
    float Aim_Smooth = 1.f;
    float Aim_MaxDistance = 150.f;
    int Aim_Mode = 0; // Memory/Mouse
    int Aim_Bone = 0; // Bone
    int Aim_Type = 0; // FOV/Dist
    int AimKeyType = 1; // and/or

    // Visual
    bool ESP      = true;
    bool ESP_Glow = false;
    bool ESP_NPC  = false;
    bool ESP_Team = false;
    bool ESP_Box = true;
    bool ESP_BoxFilled = false;
    bool ESP_Line      = false;
    bool ESP_Name      = true;
    bool ESP_Distance  = true;
    bool ESP_HealthBar = true;
    int ESP_BoxType = 1;
    float ESP_MaxDistance = 500.f;
   
    // System
    bool StreamProof = false;
    bool Crosshair = false;
    int CrosshairSize = 3;
    int CrosshairType = 0;

    // Key
    int MenuKey = VK_END;
    int AimKey0 = VK_RBUTTON;
    int AimKey1 = VK_LBUTTON;
};

extern Globals g;