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
            String(ImVec2(g.g_GameRect.right / 2.f - (ImGui::CalcTextSize("[ Spectator Found! ]").x), g.g_GameRect.top), ImColor(1.f, 0.f, 0.f, 1.f), "[ Spectator Found! ]");

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
        const float pDistance = ((pLocal->m_localOrigin - pEntity->m_localOrigin).Length() * 0.01905f);

        // 各種チェック
        if (g.g_ESP_MaxDistance < pDistance)
            continue;
        else if (!g.g_ESP_Team && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            continue;

        // 頭とベース位置の座標(3D/2D)を取得 - Bone to Neckを出せばいい
        Vector2 pBase{}, pHead{};
        const Vector3 Head = pEntity->GetEntityBonePosition(8) + Vector3(0.f, 0.f, 12.f);
        if (!WorldToScreen(ViewMatrix, g.g_GameRect, pEntity->m_localOrigin + Vector3(0.f, 0.f, -6.f), pBase) || !WorldToScreen(ViewMatrix, g.g_GameRect, Head, pHead))
            continue;

        // ESP Box等のサイズ算出
        const float Height = pBase.y - pHead.y;
        const float Width = Height / 2.f;
        const float bScale = Width / 1.5f;

        /*
        対象が見えてるかチェックするよ。
        前のループで取得したLastVisibleTimeを保存しておくよりも安定する。
        ただし、Ping値による影響があるかどうかを確認する必要がある。
        */
        bool visible = pEntity->m_lastvisibletime + 0.1f >= pLocal->GetTimeBase();

        // ESPの色を決める
        ImColor color = SetESPColor(visible, pEntity->m_iTeamNum == pLocal->m_iTeamNum ? true : false);

        // Glow
        if (g.g_ESP_Glow)
            pEntity->EnableGlow(GlowColor{ color.Value.x, color.Value.y, color.Value.z }, GlowMode{ 101, 6, 85, 96 });
        else if (!g.g_ESP_Glow && m.Read<int>(pEntity->entity + 0x310) != 0)
            pEntity->DisableGlow();

        // Line
        if (g.g_ESP_Line)
            DrawLine(ImVec2((float)g.g_GameRect.right / 2.f, (float)g.g_GameRect.bottom), ImVec2(pBase.x, pBase.y), color, 1.f);

        // Box
        if (g.g_ESP_Box)
        {
            // BOX FILLED
            if (g.g_ESP_BoxFilled)
                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(pBase.x - bScale, pHead.y), ImVec2(pBase.x + bScale, pBase.y), ESP_Filled);

            switch (g.g_ESP_BoxType)
            {
            case 0:
                DrawLine(ImVec2(pBase.x - bScale, pHead.y), ImVec2(pBase.x + bScale, pHead.y), color, 1.f);
                DrawLine(ImVec2(pBase.x - bScale, pHead.y), ImVec2(pBase.x - bScale, pBase.y), color, 1.f);
                DrawLine(ImVec2(pBase.x + bScale, pHead.y), ImVec2(pBase.x + bScale, pBase.y), color, 1.f);
                DrawLine(ImVec2(pBase.x - bScale, pBase.y), ImVec2(pBase.x + bScale, pBase.y), color, 1.f);
                break;
            case 1:
                DrawLine(ImVec2((pBase.x + bScale), pHead.y), ImVec2((pBase.x + Width / 4.5), pHead.y), color, 1); // �E��
                DrawLine(ImVec2((pBase.x - bScale), pHead.y), ImVec2((pBase.x - Width / 4.5), pHead.y), color, 1); // ����
                DrawLine(ImVec2((pBase.x + bScale), pHead.y), ImVec2((pBase.x + bScale), pHead.y + Height / 4), color, 1); // �E�㉡
                DrawLine(ImVec2((pBase.x - bScale), pHead.y), ImVec2((pBase.x - bScale), pHead.y + Height / 4), color, 1); // ���㉡
                DrawLine(ImVec2((pBase.x + bScale), pBase.y), ImVec2((pBase.x + bScale), pBase.y - (Height / 4)), color, 1);
                DrawLine(ImVec2((pBase.x - bScale), pBase.y), ImVec2((pBase.x - bScale), pBase.y - (Height / 4)), color, 1);
                DrawLine(ImVec2((pBase.x + bScale), pBase.y), ImVec2((pBase.x + Width / 4.5), pBase.y), color, 1);
                DrawLine(ImVec2((pBase.x - bScale), pBase.y), ImVec2((pBase.x - Width / 4.5), pBase.y), color, 1);
                break;
            }
        }

        // Healthbar
        if (g.g_ESP_HealthBar)
        {
            // Health
            HealthBar(((pBase.x - bScale) - 4.f), pBase.y, 2, -Height, pEntity->m_iHealth, pEntity->m_iMaxHealth);

            // Armor
            if (pEntity->m_shieldHealth > 0)
                ArmorBar((pBase.x + bScale) + 3.f, pBase.y, 2, -Height, pEntity->m_shieldHealth, pEntity->m_shieldHealthMax);
        }

        // Distance
        if (g.g_ESP_Distance) {
            const std::string DistStr = std::to_string((int)pDistance) + "m";
            String(ImVec2(pBase.x - (ImGui::CalcTextSize(DistStr.c_str()).x / 2.f), pBase.y + 1.f), ImColor(1.f, 1.f, 1.f, 1.f), DistStr.c_str());
        }

        // Name
        if (g.g_ESP_Name) {
            const std::string pName = pEntity->IsPlayer() ? pEntity->GetName() : "NPC";
            String(ImVec2(pBase.x - (ImGui::CalcTextSize(pName.c_str()).x / 2.f), pHead.y -14.f), ImColor(1.f, 1.f, 1.f, 1.f), pName.c_str());
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
                if (Vec3_Empty(CheckBone) || CheckBone == pEntity->m_localOrigin)
                    continue;

                // 弱者男性なのでこうやるしかない。StudioHDRでHitBox取得してもいいけどもう気力がない
                float B2B = (pEntity->m_localOrigin - CheckBone).Length() * 0.01905f;

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
    if (g.g_AimBot && target.entity != NULL)
        AimBot(target);
}