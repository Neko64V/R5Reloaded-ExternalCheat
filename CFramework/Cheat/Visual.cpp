#include "FrameCore.h"

void CFramework::RenderInfo()
{
    // ウォーターマーク
    std::string text = "R5Reloaded";
    StringEx(ImVec2(8.f, 8.f), ImColor(1.f, 1.f, 1.f, 1.f), ImGui::GetFontSize(), text.c_str());

    ImGui::GetStyle().AntiAliasedLines = false;

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

    ImGui::GetStyle().AntiAliasedLines = true;
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

        if (pEntity->IsSpectator()) {
            SpectatorList.push_back(pEntity->GetName());
            continue;
        }
        else if (!g.g_ESP_NPC && !pEntity->IsPlayer()) {
            continue;
        }
        
        if (!pEntity->Update())
            continue;

        // 距離を取得
        float pDistance = ((pLocal->m_localOrigin - pEntity->m_localOrigin).Length() * 0.01905f);

        if (g.g_ESP_MaxDistance < pDistance)
            continue;
        else if (!g.g_ESP_Team && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            continue;

        // 頭とベース位置の座標(3D/2D)を取得
        Vector2 pBase{}, pHead{};
        Vector3 Head = pEntity->GetEntityBonePosition(8) + Vector3(0.f, 0.f, 12.f);
        if (!WorldToScreen(ViewMatrix, g.g_GameRect, pEntity->m_localOrigin + Vector3(0.f, 0.f, -6.f), pBase) || !WorldToScreen(ViewMatrix, g.g_GameRect, Head, pHead))
            continue;

        // ESP Box等のサイズ算出
        float Height = abs(abs(pHead.y) - abs(pBase.y));
        float Width = Height / 2.f;
        float bScale = Width / 1.5f;
    
        /* 対象が見えてるかチェックするよ。
        前のループで取得したLastVisibleTimeを保存しておくよりも安定する。
        ただし、Ping値による影響があるかどうかを確認する必要がある。 */
        bool visible = pEntity->m_lastvisibletime + 0.1f >= pLocal->GetTimeBase();

        // ESPの色を決める
        ImColor color = visible ? ESP_Visible : ESP_Default;

        if (g.g_ESP_Team && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            color = ESP_Team;

        // LINE
        if (g.g_ESP_Line)
            DrawLine(ImVec2((float)g.g_GameRect.right / 2.f, (float)g.g_GameRect.bottom), ImVec2(pBase.x, pBase.y), color, 1.f);

        // BOX
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
            HealthBar(((pBase.x - bScale) - 4.f), pBase.y, 2, -Height, pEntity->m_iHealth, pEntity->m_iMaxHealth);

            if (pEntity->m_shieldHealth > 0)
                ArmorBar((pBase.x + bScale) + 3.f, pBase.y, 2, -Height, pEntity->m_shieldHealth, pEntity->m_shieldHealthMax);
        }

        // Distance
        if (g.g_ESP_Distance) {
            std::string DistStr = std::to_string((int)pDistance) + "m";
            String(ImVec2(pBase.x - (ImGui::CalcTextSize(DistStr.c_str()).x / 2.f), pBase.y + 1.f), ImColor(1.f, 1.f, 1.f, 1.f), DistStr.c_str());
        }

        // Name
        if (g.g_ESP_Name) {
            std::string pName = pEntity->IsPlayer() ? pEntity->GetName() : "NPC"; // プレイヤーかダミーかの判定
            String(ImVec2(pBase.x - (ImGui::CalcTextSize(pName.c_str()).x / 2.f), pHead.y -14.f), ImColor(1.f, 1.f, 1.f, 1.f), pName.c_str());
        }

        // Glow
        if (g.g_ESP_Glow) {
            pEntity->SetGlow(GlowColor{ color.Value.x, color.Value.y, color.Value.z }, GlowMode{ 101, 6, 85, 96 });
        }
        else if (!g.g_ESP_Glow && m.Read<int>(pEntity->entity + 0x310) != 0) {
            m.Write<int>(pEntity->entity + 0x310, 0);
            m.Write<int>(pEntity->entity + 0x320, 0);
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

                // 弱者男性なのでこうやるしかない。StudioHDRでHitBox取得してもいいけど気力がない
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

                    break;
                }
            }
        }
    }

    // AimBotのターゲットがいたらAimBotする
    if (g.g_AimBot && target.entity != NULL)
        AimBot(target);
}
