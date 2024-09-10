#pragma once
#include "../Framework/ImGui/imgui.h"
#include "../Framework/ImGui/imgui_impl_win32.h"
#include "../Framework/ImGui/imgui_impl_dx11.h"
#include "../Framework/ImGui/Custom.h"
#include "../Framework/ImGui/Font/RobotoRegular.h"
#include "SDK/CEntity/CEntity.h"
#pragma comment(lib, "freetype.lib")

class CFramework
{
public:
    bool Init();
    void UpdateList();
    bool AimBot(CEntity& target);

	void RenderInfo();
	void RenderMenu();
	void RenderESP();
private:
    CEntity local;
    std::vector<CEntity> EntityList;
    std::vector<std::string> SpectatorList;

    // Colors
    ImColor ESP_Default  = { 1.f, 1.f, 1.f, 1.f };
    ImColor ESP_Visible  = { 1.f, 0.f, 0.f, 1.f };
    ImColor ESP_Team     = { 0.f, 1.f, 1.f, 1.f };
    ImColor ESP_Filled  = { 0.f, 0.f, 0.f, 0.2f };
    ImColor FOV_User = { 1.f, 1.f, 1.f, 1.f };
    ImColor CrosshairColor = { 0.f, 1.f, 0.f, 1.f };

    void DrawLine(ImVec2 a, ImVec2 b, ImColor color, float width)
    {
        ImGui::GetBackgroundDrawList()->AddLine(a, b, color, width);
    }
    void RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags)
    {
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
    }
    void HealthBar(float x, float y, float w, float h, int value, int v_max)
    {
        RectFilled(x, y, x + w, y + h, ImColor(0.f, 0.f, 0.f, 0.725f), 0.f, 0);
        RectFilled(x, y, x + w, y + ((h / float(v_max)) * (float)value), ImColor(min(510 * (v_max - value) / 100, 255), min(510 * value / 100, 255), 25, 255), 0.0f, 0);
    }
    void ArmorBar(float x, float y, float w, float h, int value, int v_max)
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
    void Circle(ImVec2 pos, float fov_size, ImColor color)
    {
        ImGui::GetBackgroundDrawList()->AddCircle(pos, fov_size, color, 100, 0);
    }
    void String(ImVec2 pos, ImColor color, const char* text)
    {
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), pos, color, text, text + strlen(text), 128, 0);
    }
    void StringEx(ImVec2 pos, ImColor color, float font_size, const char* text)
    {
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), font_size, ImVec2(pos.x + 1.f, pos.y + 1.f), ImColor(0.f, 0.f, 0.f, 1.f), text, text + strlen(text), 128, 0);
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), font_size, pos, color, text, text + strlen(text), 128, 0);
    }
};