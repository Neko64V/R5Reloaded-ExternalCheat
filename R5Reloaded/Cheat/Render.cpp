#include "Cheat.h"
#include "..\Overlay\Overlay.h"
#include "../ImGui/Custom.h"
#include "../ImGui/Font/RobotoRegular.h"
#include "SDK/Game/GameSDK.h"
#pragma comment(lib, "freetype.lib")

// ImGui::Combo/ImGui::List等で使う文字列群
const char* BoxTypeList[] = { "Simple", "Cornered" };
const char* AimTypeList[] = { "FOV", "Distance" };
const char* AimModeList[] = { "Memory", "Mouse" };
const char* AimBoneList[] = { "Head", "Chest" };
const char* AimKeyTypeList[] = { "and", "or" };
const char* CrosshairList[] = { "Cross", "Circle" };
std::vector<std::string> MenuSelectList = { "AimBot", "Visual", "System" };

// 1回だけ実行される。他にやりたいことがあればどうぞ。
bool Cheat::Init()
{
    // ImGuiで使用するフォントを読み込む
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromMemoryCompressedTTF(RobotoRegular_compressed_data, RobotoRegular_compressed_size, 14.f, nullptr);
    io.Fonts->Build();

    return true;
}

// 画面上に常に表示しておきたいものたち。
void Cheat::RenderInfo()
{
    // ウォーターマーク的な？
    std::string text = "R5Reloaded | " + std::to_string((int)ImGui::GetIO().Framerate) + "FPS";
    StringEx(ImVec2(8.f, 8.f), ImColor(1.f, 1.f, 1.f, 1.f), 14.f, text.c_str());

    // FovCircle
    if (g.AimBot && g.Aim_DrawFov) {
        if (g.Aim_FovFilled)
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(g.GameRect.right / 2.f, g.GameRect.bottom / 2.f), g.Aim_Fov - 1.f, ImColor(FOV_User.Value.x, FOV_User.Value.y, FOV_User.Value.z, 0.09f));
        ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(g.GameRect.right / 2.f, g.GameRect.bottom / 2.f), g.Aim_Fov, FOV_User);
    }

    // 観戦者がいたら
    if (SpectatorList.size() > 0)
    {
        // 画面上中央に警告を表示
        String(ImVec2(g.GameRect.right / 2.f - (ImGui::CalcTextSize("[ Spectator Found! ]").x / 2.f), g.GameRect.top + 2.f), ImColor(1.f, 0.f, 0.f, 1.f), "[ Spectator Found! ]");
        
        // 左側に観戦者の名前を表示
        ImGui::SetNextWindowPos(ImVec2(50.f, 375.f));
        ImGui::Begin("SpectatorList", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
        
        ImGui::Text("[+] SpectatorList");

        for (auto& spec : SpectatorList)
            ImGui::Text(spec.GetName().c_str());

        ImGui::End();

        SpectatorList.clear();
    }

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
        default:
            break;
        }
    }
}

// チートのメニュー
void Cheat::RenderMenu()
{
    // Setup
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImGuiStyle& style = ImGui::GetStyle();
    static float DefaultSpacing = style.ItemSpacing.y;
    static int Index = 0;
    static int BindingID = 0;

    ImGui::SetNextWindowBgAlpha(0.975f);
    ImGui::SetNextWindowSize(ImVec2(725.f, 450.f));
    ImGui::Begin("R5Reloaded [ EXTERNAL ]", &g.ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    //---// Clild 0 //-----------------------------------//
    ImGui::BeginChild("##SelectChild", ImVec2(150.f, ImGui::GetContentRegionAvail().y), false);

    ImGui::NewLine();

    for (int i = 0; i < MenuSelectList.size(); i++)
    {
        if (ImGui::CustomButton(MenuSelectList[i].c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 35.f), Index == i ? true : false))
            Index = i;
    }

    ImGui::EndChild();
    //---// Clild 0 End //-------------------------------//

    ImGui::SameLine();

    //---// Clild 1 //-----------------------------------//
    ImGui::BeginChild("##ContextChild", ImVec2(ImGui::GetContentRegionAvail()), false);

    //---// Left //--------------------------------------//
    ImGui::BeginChild("##LeftChild", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 16.f, ImGui::GetContentRegionAvail().y), false);

    ImGui::Spacing();

    switch (Index)
    {
    case 0: // aim
        ImGui::Text("Aimbot");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("AimBot", &g.AimBot);
        ImGui::Checkbox("Prediction", &g.Aim_Prediction);
        ImGui::Checkbox("Visibility Check", &g.Aim_VisCheck);

        ImGui::Spacing();
        ImGui::NewLine();

        ImGui::Text("Aim Options");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Aim at NPC", &g.Aim_NPC);
        ImGui::Checkbox("Aim at Team", &g.Aim_Team);
        ImGui::Combo("AimBone", &g.Aim_Bone, AimBoneList, IM_ARRAYSIZE(AimBoneList));

        ImGui::Spacing();
        ImGui::NewLine();

        ImGui::Text("FOV");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("DrawFOV", &g.Aim_DrawFov);
        ImGui::Checkbox("FOV Filled", &g.Aim_FovFilled);
        ImGui::Checkbox("RainbowFOV", &g.Aim_RainbowFov);
        ImGui::CustomSliderFloat("FOV", "##aim_fov", &g.Aim_Fov, 50.f, 1000.f);
        ImGui::ColorEdit4("FOV Color", &FOV_User.Value.x);
        break;
    case 1: // visual
        ImGui::Text("Visual");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("ESP", &g.ESP);
        ImGui::Checkbox("Glow", &g.ESP_Glow);
        ImGui::Checkbox("NPC ESP", &g.ESP_NPC);
        ImGui::Checkbox("Team ESP", &g.ESP_Team);

        ImGui::NewLine();
        ImGui::Spacing();

        ImGui::Text("ESP Options");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Box", &g.ESP_Box);
        ImGui::Checkbox("BoxFilled", &g.ESP_BoxFilled);
        ImGui::Checkbox("Line", &g.ESP_Line);
        ImGui::Checkbox("Name", &g.ESP_Name);
        ImGui::Checkbox("Distance", &g.ESP_Distance);
        ImGui::Checkbox("HealthBar", &g.ESP_HealthBar);
        break;
    case 2: // system
        ImGui::Text("System");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("StreamProof", &g.StreamProof);

        ImGui::Spacing();
        ImGui::NewLine();

        ImGui::Text("Crosshair");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Crosshair", &g.Crosshair);
        ImGui::CustomSliderInt("CrosshairSize", "##SizeCH", &g.CrosshairSize, 1, 10);
        ImGui::ColorEdit4("Color##C", &CrosshairColor.Value.x);
        ImGui::Combo("Type##C", &g.CrosshairType, CrosshairList, IM_ARRAYSIZE(CrosshairList));
        break;
    default:
        break;
    }

    ImGui::EndChild();
    //---------------------------------------------------//

    ImGui::SameLine();

    //---// Right //--------------------------------------//
    ImGui::BeginChild("##RightChild", ImVec2(ImGui::GetContentRegionAvail()), false);

    ImGui::Spacing();

    switch (Index)
    {
    case 0: // aim
        ImGui::Text("Setting");
        ImGui::Separator();
        ImGui::Spacing();
        style.ItemSpacing.y = 3.f;
        ImGui::CustomSliderFloat("FOV", "##aim_fov", &g.Aim_Fov, 50.f, 1000.f);
        ImGui::CustomSliderFloat("Smooth", "##aim_smt", &g.Aim_Smooth, 1.f, 20.f);
        ImGui::CustomSliderFloat("Distance", "##aim_dist", &g.Aim_MaxDistance, 50.f, 300.f);
        style.ItemSpacing.y = DefaultSpacing;
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Combo("AimType", &g.Aim_Type, AimTypeList, IM_ARRAYSIZE(AimTypeList));
        ImGui::Combo("AimMode", &g.Aim_Mode, AimModeList, IM_ARRAYSIZE(AimModeList));

        ImGui::Spacing();
        ImGui::NewLine();

        ImGui::Text("KeyBind");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("1st Key:");
        if (ImGui::Button(BindingID == 1 ? "< Press Any Key >" : KeyNames[g.AimKey0], ImVec2(215.f, 22.5f))) {
            BindingID = 1;
            std::thread([&]() {KeyBinder(g.AimKey0, BindingID); }).detach();
        }

        ImGui::PushItemWidth(215.f);
        ImGui::Combo("##KeyMode", &g.AimKeyType, AimKeyTypeList, IM_ARRAYSIZE(AimKeyTypeList));
        ImGui::PopItemWidth();
        ImGui::Spacing();

        ImGui::Text("2nd Key:");
        if (ImGui::Button(BindingID == 2 ? "< Press Any Key >" : KeyNames[g.AimKey1], ImVec2(215.f, 22.5f))) {
            BindingID = 2;
            std::thread([&]() {KeyBinder(g.AimKey1, BindingID); }).detach();
        }
        break;
    case 1: // visual
        ImGui::Text("ESP Setting");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::CustomSliderFloat("Distance", "##Dist", &g.ESP_MaxDistance, 100.f, 2000.f);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Combo("BoxType", &g.ESP_BoxType, BoxTypeList, IM_ARRAYSIZE(BoxTypeList));

        ImGui::NewLine();
        ImGui::Spacing();

        ImGui::Text("ESP Colors");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::BulletText("Main Color");
        ImGui::ColorEdit4("Default", &ESP_Default.Value.x);
        ImGui::ColorEdit4("Visible", &ESP_Visible.Value.x);
        ImGui::ColorEdit4("Team", &ESP_Team.Value.x);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::BulletText("Sub Color");
        ImGui::ColorEdit4("BoxFilled", &ESP_Filled.Value.x);
        break;
    case 2: // system
        ImGui::Text("Exit");
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Exit", ImVec2(ImGui::GetContentRegionAvail().x, 30.f)))
            g.Run = false;
        break;
    default:
        break;
    }

    ImGui::EndChild();
    ImGui::EndChild();
    //---------------------------------------------------//

    ImGui::End();
}

void Cheat::RenderESP()
{
    CPlayer* pLocal = &local;

    // Localの更新に失敗したらこの関数を終了
    if (!pLocal->Update())
        return;

    // AimBot関連
    float FOV = 0.f;
    float MinFov = 999.f;
    float MinDistance = 9999.f;
    CPlayer target = CPlayer();
    Vector2 ScreenMiddle = { (float)g.GameRect.right / 2.f, (float)g.GameRect.bottom / 2.f };

    // ViewMatrixとかいろいろ
    ImGui::GetStyle().AntiAliasedLines = false;
    float BaseTime = m.Read<float>(pLocal->entity + 0x1d18);
    uintptr_t ViewRenderer = m.Read<uintptr_t>(m.g_BaseAddress + sdk.ViewRender);
    Matrix ViewMatrix = m.Read<Matrix>(m.Read<uintptr_t>(ViewRenderer + sdk.ViewMatrix));

    // るーぷするよ
    for (auto& entity : NewEntityList) 
    {
        CPlayer* pEntity = &entity;

        if (pEntity->IsSpectator()) {
            SpectatorList.push_back(entity);
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

        Vector2 pBase{}, pHead{};

        // 頭とベース位置の座標(3D/2D)を取得
        Vector3 Head = pEntity->GetEntityBonePosition(8) + Vector3(0.f, 0.f, 12.f);
        if (!WorldToScreen(ViewMatrix, g.GameRect, pEntity->m_localOrigin + Vector3(0.f, 0.f, -6.f), pBase) || !WorldToScreen(ViewMatrix, g.GameRect, Head, pHead))
            continue;

        // ESP Box等のサイズ算出
        float Height = abs(abs(pHead.y) - abs(pBase.y));
        float Width = Height / 2.f;
        float bScale = Width / 1.5f;

        // 対象が見えてるかチェックするよ。
        bool visible = pEntity->m_lastvisibletime + 0.35f >= BaseTime;

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

    // AimBotのターゲットがいたらAimBotする。
    if (g.AimBot && target.entity != NULL)
        AimBot(target);

    ImGui::GetStyle().AntiAliasedLines = true;
}