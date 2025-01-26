#pragma once
#include "../Framework/ImGui/imgui.h"
#include "../Framework/ImGui/imgui_impl_win32.h"
#include "../Framework/ImGui/imgui_impl_dx11.h"
#include "../Framework/ImGui/Custom.h"
#include "SDK/CEntity/CEntity.h"

class CFramework
{
public:
    void UpdateList();
    void MiscAll();
    bool AimBot(CEntity& target);

	void RenderInfo();
	void RenderMenu();
	void RenderESP();
private:
    CEntity local, *pLocal = &local;
    std::vector<CEntity> EntityList;
    std::vector<std::string> SpectatorPlayerName;

    ImColor SetESPColor(bool& is_visible, bool is_team);

    // Colors
    ImColor ESP_Default = { 1.f, 0.f, 0.f, 1.f };
    ImColor ESP_Visible = { 0.f, 1.f, 0.f, 1.f };
    ImColor ESP_Team    = { 0.f, 0.75f, 1.f, 1.f };
    ImColor ESP_Shadow  = { 0.f, 0.f, 0.f, 0.3f };

    ImColor FOV_User = { 1.f, 1.f, 1.f, 0.5f };
    ImColor CrosshairColor = { 0.f, 1.f, 0.f, 1.f };

    void DrawLine(Vector2 a, Vector2 b, ImColor color, float width)
    {
        ImGui::GetBackgroundDrawList()->AddLine(ImVec2((int)a.x, (int)a.y), ImVec2((int)b.x, (int)b.y), color, width);
    }
    void DrawBox(int x, int y, int w, int h, ImColor color, float width)
    {
        
    }
    void RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags)
    {
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
    }
    void HealthBar(int x, int y, int w, int h, int value, int v_max)
    {
        RectFilled(x, y, x + w, y + h, ImColor(0.f, 0.f, 0.f, 0.725f), 0.f, 0);
        RectFilled(x, y, x + w, y + ((h / float(v_max)) * (float)value), ImColor(min(510 * (v_max - value) / 100, 255), min(510 * value / 100, 255), 25, 255), 0.0f, 0);
    }
    void ArmorBar(int x, int y, int w, int h, int value, int v_max)
    {
        ImColor barColor = ImColor(1.f, 1.f, 1.f, 1.f);

        switch (v_max)
        {
        case 50:  barColor = ImColor(1.f, 1.f, 1.f, 1.f); break;
        case 75: barColor = ImColor(0.f, 0.85f, 1.f, 1.f); break;
        case 100: barColor = ImColor(0.7f, 0.f, 1.f, 1.f); break;
        case 125: barColor = ImColor(1.f, 0.01f, 0.f, 1.f); break;
        default: barColor = ImColor(0.f, 0.f, 0.f, 1.f); break;
        }

        RectFilled(x, y, x + w, y + h, ImColor(0.f, 0.f, 0.f, 0.725f), 0.f, 0);
        RectFilled(x, y, x + w, y + ((h / float(v_max)) * (float)value), barColor, 0.f, 0);
    }
    void Circle(Vector2 pos, float fov_size, ImColor color)
    {
        ImGui::GetBackgroundDrawList()->AddCircle(ImVec2((int)pos.x, (int)pos.y), fov_size, color, 100, 0);
    }
    void String(Vector2 pos, ImColor color, const char* text)
    {
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2((int)pos.x, (int)pos.y), color, text, text + strlen(text), 1024, 0);
    }
    void StringEx(Vector2 pos, ImColor color, float font_size, const char* text)
    {
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), font_size, ImVec2((int)pos.x + 1.f, (int)pos.y + 1.f), ESP_Shadow, text, text + strlen(text), 1024, 0);
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), font_size, ImVec2((int)pos.x, (int)pos.y), color, text, text + strlen(text), 1024, 0);
    }
};