#include "FrameCore.h"

void CFramework::RenderInfo()
{
    // FovCircle
    if (g.g_AimBot && g.g_Aim_DrawFov)
        ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(g.g_GameRect.right / 2.f, g.g_GameRect.bottom / 2.f), g.g_Aim_Fov, FOV_User);

    // Crosshair
    if (g.g_Crosshair)
    {
        switch (g.g_CrosshairType)
        {
        case 0: {
            ImVec2 Center = ImVec2(g.g_GameRect.right / 2, g.g_GameRect.bottom / 2);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Center.x - g.g_CrosshairSize, Center.y), ImVec2((Center.x + g.g_CrosshairSize) + 1, Center.y), CrosshairColor, 1);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Center.x, Center.y - g.g_CrosshairSize), ImVec2(Center.x, (Center.y + g.g_CrosshairSize) + 1), CrosshairColor, 1);
        }   break;
        case 1:
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)g.g_GameRect.right / 2.f, (float)g.g_GameRect.bottom / 2.f), g.g_CrosshairSize + 1, ImColor(0.f, 0.f, 0.f, 1.f), NULL);
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)g.g_GameRect.right / 2.f, (float)g.g_GameRect.bottom / 2.f), g.g_CrosshairSize, CrosshairColor, NULL);
            break;
        }
    }

    // SpectatorList
    if (g.g_SpectatorList)
    {
        if (SpectatorPlayerName.size() > 0)
            String(Vector2(g.g_GameRect.right / 2 - (ImGui::CalcTextSize("[ Spectator Found! ]").x), g.g_GameRect.top), ImColor(1.f, 0.f, 0.f, 1.f), "[ Spectator Found! ]");

        ImGui::SetNextWindowBgAlpha(SpectatorPlayerName.size() > 0 ? 0.9f : 0.35f);
        ImGui::SetNextWindowPos(ImVec2(12.f, 16.f));
        ImGui::SetNextWindowSize(ImVec2(250.f, 125.f));
        std::string title = "Spectator [" + std::to_string(SpectatorPlayerName.size()) + "]";
        ImGui::Begin(title.c_str(), &g.g_ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        std::vector<std::string> spec_list = SpectatorPlayerName;

        for (auto& name : spec_list)
            ImGui::Text(name.c_str());

        ImGui::End();
    }
}

void CFramework::RenderESP()
{
    CEntity* pLocal = &local;

    // Localの更新に失敗したらこの関数を終了
    if (!pLocal->Update())
        return;

    // AimBot関連
    float FOV = 0.f;
    float MinFov = 999.f;
    float MinDistance = 9999.f;
    CEntity target = CEntity();
    Vector2 ScreenMiddle = { (float)g.g_GameRect.right / 2.f, (float)g.g_GameRect.bottom / 2.f };

    // ViewMatrixとかいろいろ
    uintptr_t ViewRenderer = m.Read<uintptr_t>(m.m_gBaseAddress + offset::ViewRender);
    Matrix ViewMatrix = m.Read<Matrix>(m.Read<uintptr_t>(ViewRenderer + offset::ViewMatrix));

    // るーぷするよ
    for (auto& entity : EntityList)
    {
        CEntity* pEntity = &entity;

        if (!pEntity->Update())
            continue;

        // 距離を取得
        const float pDistance = ((pLocal->m_vecAbsOrigin - pEntity->m_vecAbsOrigin).Length() * 0.01905f);

        // 各種チェック
        if (g.g_ESP_MaxDistance < pDistance)
            continue;
        else if (!g.g_ESP_Team && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            continue;

        /* // 方法1 - 頭とベース座標の位置をベースにする
        Vector2 pBase{}, pHead{};
        const Vector3 Head = pEntity->GetEntityBonePosition(8) + Vector3(0.f, 0.f, 12.f);
        if (!WorldToScreen(ViewMatrix, g.g_GameRect, pEntity->m_vecAbsOrigin + Vector3(0.f, 0.f, -6.f), pBase) || !WorldToScreen(ViewMatrix, g.g_GameRect, Head, pHead))
            continue;
        */

        // 方法2 - SourceEngineのみではあるが m_Collision を使用する方法
        // Counter Strike: Sourceを触ってる時にこれApexにも応用できそうだなと思ってこうなりました。オフセット以外なんもいじってない
        Vector3 min = pEntity->vecMin();
        Vector3 max = pEntity->vecMax();

        int left, top, right, bottom;
        Vector2 flb, brt, blb, frt, frb, brb, blt, flt;

        Vector3 points[8] = { Vector3(min.x, min.y, min.z), Vector3(min.x, max.y, min.z), Vector3(max.x, max.y, min.z),
                    Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z),
                    Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z) };

        Vector3 transformedPoints[8]{};

        // FS_1v1等のモードだと正常に動かないので従来の方法を使うのが望ましいかもね
        for (int i = 0; i < 8; i++) {
            transformedPoints[i] = Vector3::TransformNormal(points[i], pEntity->GetRgflCoordinateFrame());
        }

        if (!WorldToScreen(ViewMatrix, g.g_GameRect, transformedPoints[3], flb) || !WorldToScreen(ViewMatrix, g.g_GameRect, transformedPoints[5], brt) ||
            !WorldToScreen(ViewMatrix, g.g_GameRect, transformedPoints[0], blb) || !WorldToScreen(ViewMatrix, g.g_GameRect, transformedPoints[4], frt) ||
            !WorldToScreen(ViewMatrix, g.g_GameRect, transformedPoints[2], frb) || !WorldToScreen(ViewMatrix, g.g_GameRect, transformedPoints[1], brb) ||
            !WorldToScreen(ViewMatrix, g.g_GameRect, transformedPoints[6], blt) || !WorldToScreen(ViewMatrix, g.g_GameRect, transformedPoints[7], flt))
            continue;

        Vector2 vec2_array[] = { flb, brt, blb, frt, frb, brb, blt, flt };
        left = flb.x;
        top = flb.y;
        right = flb.x;
        bottom = flb.y;

        for (auto j = 1; j < 8; ++j)
        {
            if (left > vec2_array[j].x)
                left = vec2_array[j].x;
            if (bottom < vec2_array[j].y)
                bottom = vec2_array[j].y;
            if (right < vec2_array[j].x)
                right = vec2_array[j].x;
            if (top > vec2_array[j].y)
                top = vec2_array[j].y;
        }

        // サイズ算出
        const int Height = bottom - top;
        const int Width = right - left;
        const int Center = (right - left) / 2.f;
        const int bScale = (right - left) / 3.f;

        // 対象が見えてるかチェックする。
        bool visible = pEntity->m_lastvisibletime + 0.125f >= pLocal->GetTimeBase();

        // 色を決める
        ImColor color = visible ? ESP_Visible : (pLocal->m_iTeamNum == pEntity->m_iTeamNum ? ESP_Team : ESP_Default);

        // Glow
        if (g.g_ESP_Glow)
            pEntity->EnableGlow(GlowColor{ color.Value.x, color.Value.y, color.Value.z }, GlowMode{ 101, 6, 85, 96 });
        else if (!g.g_ESP_Glow && m.Read<int>(pEntity->address + 0x310) != 0)
            pEntity->DisableGlow();

        // Line
        if (g.g_ESP_Line)
            DrawLine(Vector2(g.g_GameRect.right / 2.f, g.g_GameRect.bottom), Vector2(right - (Width / 2), bottom), color, 1.f);

        // Box
        if (g.g_ESP_Box)
        {
            // BoxFilled
            if (g.g_ESP_BoxFilled)
                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ESP_Filled);

            switch (g.g_ESP_BoxType)
            {
            case 0:
                // Shadow
                DrawLine(Vector2(left - 1, top - 1), Vector2(right + 2, top - 1), ImColor(0.f, 0.f, 0.f, 1.f), 1.f);
                DrawLine(Vector2(left - 1, top), Vector2(left - 1, bottom + 2), ImColor(0.f, 0.f, 0.f, 1.f), 1.f);
                DrawLine(Vector2(right + 1, top), Vector2(right + 1, bottom + 2), ImColor(0.f, 0.f, 0.f, 1.f), 1.f);
                DrawLine(Vector2(left - 1, bottom + 1), Vector2(right + 1, bottom + 1), ImColor(0.f, 0.f, 0.f, 1.f), 1.f);

                // Main
                DrawLine(Vector2(left, top), Vector2(right, top), color, 1.f);
                DrawLine(Vector2(left, top), Vector2(left, bottom), color, 1.f);
                DrawLine(Vector2(right, top), Vector2(right, bottom), color, 1.f);
                DrawLine(Vector2(left, bottom), Vector2(right + 1, bottom), color, 1.f);
                break;
            case 1:
                DrawLine(Vector2(left, top), Vector2(left + bScale, top), color, 1.f); // Top
                DrawLine(Vector2(right, top), Vector2(right - bScale, top), color, 1.f);
                DrawLine(Vector2(left, top), Vector2(left, top + bScale), color, 1.f); // Left
                DrawLine(Vector2(left, bottom), Vector2(left, bottom - bScale), color, 1.f);
                DrawLine(Vector2(right, top), Vector2(right, top + bScale), color, 1.f); // Right
                DrawLine(Vector2(right, bottom), Vector2(right, bottom - bScale), color, 1.f);
                DrawLine(Vector2(left, bottom), Vector2(left + bScale, bottom), color, 1.f); // Bottom
                DrawLine(Vector2(right, bottom), Vector2(right - bScale, bottom), color, 1.f);
                break;
            }
        }

        // Healthbar
        if (g.g_ESP_HealthBar)
        {
            HealthBar(left - 4.f, bottom, 2, -Height, pEntity->m_iHealth, pEntity->m_iMaxHealth); // Health

            // Armor
            if (pEntity->m_shieldHealth > 0)
                ArmorBar(right + 4.f, bottom, 2, -Height, pEntity->m_shieldHealth, pEntity->m_shieldHealthMax);
        }

        // Distance
        if (g.g_ESP_Distance) {
            const std::string DistStr = std::to_string((int)pDistance) + "m";
            StringEx(Vector2(right - Center - (ImGui::CalcTextSize(DistStr.c_str()).x / 2.f), bottom + 1), ImColor(1.f, 1.f, 1.f, 1.f), ImGui::GetFontSize(), DistStr.c_str());
        }

        // Name
        if (g.g_ESP_Name) {
            const std::string pName = pEntity->IsPlayer() ? pEntity->pName : "NPC";
            StringEx(Vector2(right - Center - (ImGui::CalcTextSize(pName.c_str()).x / 2.f), top - ImGui::GetFontSize() - 1), ImColor(1.f, 1.f, 1.f, 1.f), ImGui::GetFontSize(), pName.c_str());
        }

        // AimBot
        if (g.g_AimBot)
        {
            // Check
            if (g.g_Aim_VisCheck && !visible)
                continue;
            else if (!g.g_Aim_Team && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
                continue;
            else if (!g.g_Aim_NPC && !pEntity->IsPlayer())
                continue;

            // FOV Check
            for (int i = 0; i < 90; i++)
            {
                Vector2 pBone{};
                Vector3 CheckBone = pEntity->GetEntityBonePosition(i);
                if (Vec3_Empty(CheckBone) || CheckBone == pEntity->m_vecAbsOrigin)
                    continue;

                // 弱者男性なのでこうやるしかない。StudioHDRでHitBox取得してもいいけどもう気力がない
                float B2B = (pEntity->m_vecAbsOrigin - CheckBone).Length() * 0.01905f;

                if (B2B > 2.f)
                    continue;
                else if (!WorldToScreen(ViewMatrix, g.g_GameRect, CheckBone, pBone))
                    continue;

                FOV = abs((ScreenMiddle - pBone).Length());

                if (FOV < g.g_Aim_Fov)
                {
                    switch (g.g_Aim_Type)
                    {
                        // Crosshair
                    case 0:
                        if (MinFov > FOV) {
                            target = entity;
                            MinFov = FOV;
                        }
                        break;
                        // Game Distance
                    case 1:
                        if (MinDistance > pDistance) {
                            target = entity;
                            MinDistance = pDistance;
                        }
                        break;
                    }
                }
            }
        }
    }

    // AimBotのターゲットがいたらAimBotする
    if (g.g_AimBot && target.address != NULL)
        AimBot(target);
}