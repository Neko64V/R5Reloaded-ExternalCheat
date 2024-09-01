#pragma once
#include <vector>
#include <string>
#include "Config/Config.h"

extern bool IsKeyDown(int VK);
extern void KeyBinder(int& target_key, int& BindID);
extern const char* KeyNames[];