#pragma once
#include <Windows.h>

struct Globals
{
    // System
    bool g_Run = true;
    bool g_ShowMenu = false;

    // GameData
    HWND g_GameHwnd;
    RECT g_GameRect{};
    POINT g_GamePoint{};

    // AimBot
    bool g_AimBot   = true;
    bool g_Aim_NPC  = false;
    bool g_Aim_Team = false;
    bool g_Aim_VisCheck   = true;
    bool g_Aim_Prediction = true;
    bool g_Aim_NoSway     = true;
    bool g_Aim_DrawFov    = true;
    float g_Aim_Fov = 150.f;
    float g_Aim_Smooth = 1.f;
    float g_Aim_MaxDistance = 150.f;
    int g_Aim_Mode = 0;   // Memory/Mouse
    int g_Aim_Bone = 1;   // Bone
    int g_Aim_Type = 0;   // FOV/Dist
    int g_AimKeyType = 1; // and/or

    // Visual
    bool g_ESP      = true;
    bool g_ESP_Glow = false;
    bool g_ESP_NPC  = false;
    bool g_ESP_Team = false;
    bool g_ESP_Box  = true;
    bool g_ESP_BoxFilled = false;
    bool g_ESP_Line      = false;
    bool g_ESP_Name      = true;
    bool g_ESP_Distance  = true;
    bool g_ESP_HealthBar = true;
    int g_ESP_BoxType    = 1;
    int g_ESP_RenderMode = 1;
    int g_ESP_MaxDistance = 500;
   
    // Misc
    bool g_BunnyHop = false;
    bool g_NoRecoil = false;
    float g_NoRecoilVal = 1.f;

    // System
    bool g_SpectatorList = true;
    bool g_StreamProof = false;
    bool g_Crosshair = false;
    int g_CrosshairSize = 3;
    int g_CrosshairType = 0;
    int g_MaxFramerate = 165;

    // Key
    int g_MenuKey = VK_INSERT;
    int g_AimKey0 = VK_RBUTTON;
    int g_AimKey1 = VK_LBUTTON;
};

extern Globals g;
