#pragma once
#include "Config/Config.h"
#include "Memory/Memory.h"
#include "SimpleMath/SimpleMath.h"
#include <vector>
#include <thread>
#include <string>
using namespace DirectX::SimpleMath;

extern const char* KeyNames[];
extern bool IsKeyDown(int VK);
extern void KeyBinder(int& target_key, int& BindID);