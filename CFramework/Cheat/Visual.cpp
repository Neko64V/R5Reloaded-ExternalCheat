#include "FrameCore.h"

void CFramework::RenderInfo()
{
    // ウォーターマーク
    std::string text = "R5Reloaded | " + std::to_string((int)ImGui::GetIO().Framerate) + "FPS ";
    StringEx(ImVec2(8.f, 8.f), ImColor(1.f, 1.f, 1.f, 1.f), 14.f, text.c_str());

    ImGui::GetStyle().AntiAliasedLines = false;

    // FovCircle
    if (g.AimBot && g.Aim_DrawFov)
        ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(g.GameRect.right / 2.f, g.GameRect.bottom / 2.f), g.Aim_Fov, FOV_User);

    // Crosshair
    if (g.Crosshair)
    {
        switch (g.CrosshairType)
        {
        case 0: {
            ImVec2 Center = ImVec2(g.GameRect.right / 2, g.GameRect.bottom / 2);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Center.x - g.CrosshairSize, Center.y), ImVec2((Center.x + g.CrosshairSize) + 1, Center.y), CrosshairColor, 1);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Center.x, Center.y - g.CrosshairSize), ImVec2(Center.x, (Center.y + g.CrosshairSize) + 1), CrosshairColor, 1);
        }   break;
        case 1:
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)g.GameRect.right / 2.f, (float)g.GameRect.bottom / 2.f), g.CrosshairSize + 1, ImColor(0.f, 0.f, 0.f, 1.f), NULL);
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)g.GameRect.right / 2.f, (float)g.GameRect.bottom / 2.f), g.CrosshairSize, CrosshairColor, NULL);
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
    Vector2 ScreenMiddle = { (float)g.GameRect.right / 2.f, (float)g.GameRect.bottom / 2.f };

    // ViewMatrixとかいろいろ
    float TimeBase = pLocal->GetTimeBase();
    uintptr_t ViewRenderer = m.Read<uintptr_t>(m.g_BaseAddress + offset::ViewRender);
    Matrix ViewMatrix = m.Read<Matrix>(m.Read<uintptr_t>(ViewRenderer + offset::ViewMatrix));

    // るーぷするよ
    for (auto& entity : EntityList) 
    {
        CEntity* pEntity = &entity;

        if (pEntity->IsSpectator()) {
            SpectatorList.push_back(pEntity->GetName());
            continue;
        }
        else if (!g.ESP_NPC && !pEntity->IsPlayer()) {
            continue;
        }
        
        if (!pEntity->Update())
            continue;

        // 距離を取得
        float pDistance = ((pLocal->m_localOrigin - pEntity->m_localOrigin).Length() * 0.01905f);

        if (g.ESP_MaxDistance < pDistance)
            continue;
        else if (!g.ESP_Team && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            continue;

        // 頭とベース位置の座標(3D/2D)を取得
        Vector2 pBase{}, pHead{};
        Vector3 Head = pEntity->GetEntityBonePosition(8) + Vector3(0.f, 0.f, 12.f);
        if (!WorldToScreen(ViewMatrix, g.GameRect, pEntity->m_localOrigin + Vector3(0.f, 0.f, -6.f), pBase) || !WorldToScreen(ViewMatrix, g.GameRect, Head, pHead))
            continue;

        // ESP Box等のサイズ算出
        float Height = abs(abs(pHead.y) - abs(pBase.y));
        float Width = Height / 2.f;
        float bScale = Width / 1.5f;

        // 対象が見えてるかチェックするよ。
        bool visible = pEntity->m_lastvisibletime + 0.2f >= TimeBase;

        // ESPの色を決めるよ
        ImColor color = visible ? ESP_Visible : ESP_Default;

        if (g.ESP_Team && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            color = ESP_Team;

        // LINE
        if (g.ESP_Line)
            DrawLine(ImVec2((float)g.GameRect.right / 2.f, (float)g.GameRect.bottom), ImVec2(pBase.x, pBase.y), color, 1.f);

        // BOX
        if (g.ESP_Box)
        {
            // BOX FILLED
            if (g.ESP_BoxFilled)
                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(pBase.x - bScale, pHead.y), ImVec2(pBase.x + bScale, pBase.y), ESP_Filled);

            switch (g.ESP_BoxType)
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
        if (g.ESP_HealthBar)
        {
            HealthBar(((pBase.x - bScale) - 4.f), pBase.y, 2, -Height, pEntity->m_iHealth, pEntity->m_iMaxHealth);

            if (pEntity->m_shieldHealth > 0)
                ArmorBar((pBase.x + bScale) + 3.f, pBase.y, 2, -Height, pEntity->m_shieldHealth, pEntity->m_shieldHealthMax);
        }

        // Distance
        if (g.ESP_Distance) {
            std::string DistStr = std::to_string((int)pDistance) + "m";
            String(ImVec2(pBase.x - (ImGui::CalcTextSize(DistStr.c_str()).x / 2.f), pBase.y + 1.f), ImColor(1.f, 1.f, 1.f, 1.f), DistStr.c_str());
        }

        // Name
        if (g.ESP_Name) {
            std::string pName = pEntity->IsPlayer() ? pEntity->GetName() : "NPC"; // プレイヤーかダミーかの判定
            String(ImVec2(pBase.x - (ImGui::CalcTextSize(pName.c_str()).x / 2.f), pHead.y -14.f), ImColor(1.f, 1.f, 1.f, 1.f), pName.c_str());
        }

        // Glow
        if (g.ESP_Glow) {
            pEntity->SetGlow(GlowColor{ color.Value.x, color.Value.y, color.Value.z }, GlowMode{ 101, 6, 85, 96 });
        }
        else if (!g.ESP_Glow && m.Read<int>(pEntity->entity + 0x310) != 0) {
            m.Write<int>(pEntity->entity + 0x310, 0);
            m.Write<int>(pEntity->entity + 0x320, 0);
        }

        // AimBot
        if (g.AimBot)
        {
            // Check
            if (g.Aim_VisCheck && !visible)
                continue;
            else if (!g.Aim_Team && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
                continue;
            else if (!g.Aim_NPC && !pEntity->IsPlayer())
                continue;

            // FOV Check
            for (int i = 0; i < 90; i++)
            {
                Vector2 pBone{};
                Vector3 CheckBone = pEntity->GetEntityBonePosition(i);
                if (Vec3_Empty(CheckBone) || CheckBone == pEntity->m_localOrigin)
                    continue;

                // 弱者男性(自称)なのでこうやるしかない。ぴえん。
                float B2B = (pEntity->m_localOrigin - CheckBone).Length() * 0.01905f;

                if (B2B > 2.f)
                    continue;
                else if (!WorldToScreen(ViewMatrix, g.GameRect, CheckBone, pBone))
                    continue;

                FOV = abs((ScreenMiddle - pBone).Length());

                if (FOV < g.Aim_Fov)
                {
                    switch (g.Aim_Type)
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
    if (g.AimBot && target.entity != NULL)
        AimBot(target);
}