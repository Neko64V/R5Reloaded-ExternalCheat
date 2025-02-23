#include "FrameCore.h"

// ImGui::Combo/ImGui::List等で使う文字列群
const char* ESP_RenderModeList[] = { "Bone", "m_Collision" };
const char* BoxTypeList[] = { "Simple", "Cornered" };
const char* AimTypeList[] = { "FOV", "Distance" };
const char* AimModeList[] = { "Memory", "Mouse" };
const char* AimBoneList[] = { "Head", "Chest" };
const char* AimKeyTypeList[] = { "and", "or" };
const char* CrosshairList[] = { "Cross", "Circle" };
std::vector<const char*> MenuStringList = { "AimBot", "Visual", "Misc", "Setting" };
std::vector<const char*> MenuIconList = { ICON_FA_CROSSHAIRS, ICON_FA_EYE, ICON_FA_BARS, ICON_FA_GEAR };

// チートのメニュー
void CFramework::RenderMenu()
{
    // Setup
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImGuiStyle& style = ImGui::GetStyle();
    static float DefaultSpacing = style.ItemSpacing.y;
    static int Index = 0;
    static int BindingID = 0;

    ImGui::SetNextWindowBgAlpha(0.975f);
    ImGui::SetNextWindowSize(ImVec2(725.f, 450.f));
    ImGui::Begin("R5Reloaded [ EXTERNAL ]", &g.g_ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    //---// Clild 0 //-----------------------------------//
    ImGui::BeginChild("##SelectChild", ImVec2(150.f, ImGui::GetContentRegionAvail().y), false);

    ImGui::NewLine();

    ImGui::PushFont(icon);

    for (int i = 0; i < MenuStringList.size(); i++)
    {
        if (ImGui::CustomButton(MenuIconList[i], MenuStringList[i], ImVec2(ImGui::GetContentRegionAvail().x, 35.f), Index == i ? true : false))
            Index = i;
    }

    ImGui::PopFont();

    ImGui::EndChild();
    //---// Clild 0 End //-------------------------------//

    ImGui::SameLine();

    //---// Clild 1 //-----------------------------------//
    ImGui::BeginChild("##MainChild", ImVec2(ImGui::GetContentRegionAvail()), false);

    //---// Left //--------------------------------------//
    ImGui::BeginChild("##LeftChild", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 16.f, ImGui::GetContentRegionAvail().y), false);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));

    switch (Index)
    {
    case 0: // aim
        ImGui::BeginChild("##C000", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.75f), true);
        ImGui::Text("Aimbot");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("AimBot", &g.g_AimBot);
        ImGui::Checkbox("Prediction", &g.g_Aim_Prediction);
        ImGui::Checkbox("Visibility Check", &g.g_Aim_VisCheck);

        ImGui::EndChild();
        ImGui::BeginChild("##C001", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.f), true);

        ImGui::Text("Aim Options");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Aim at NPC", &g.g_Aim_NPC);
        ImGui::Checkbox("Aim at Team", &g.g_Aim_Team);
        ImGui::Combo("AimBone", &g.g_Aim_Bone, AimBoneList, IM_ARRAYSIZE(AimBoneList));

        ImGui::EndChild();
        ImGui::BeginChild("##C002", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("FOV");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("DrawFOV", &g.g_Aim_DrawFov);
        ImGui::CustomSliderFloat("FOV", "##aim_fov", &g.g_Aim_Fov, 50.f, 1000.f);
        ImGui::ColorEdit4("FOV Color", &FOV_User.Value.x);

        ImGui::EndChild();
        break;
    case 1: // visual
        ImGui::BeginChild("##C010", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.85f), true);
        ImGui::Text("Visual");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("ESP", &g.g_ESP);
        ImGui::Checkbox("Glow", &g.g_ESP_Glow);
        ImGui::Checkbox("NPC ESP", &g.g_ESP_NPC);
        ImGui::Checkbox("Team ESP", &g.g_ESP_Team);

        ImGui::EndChild();
        ImGui::BeginChild("##C011", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("ESP Options");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Box", &g.g_ESP_Box);
        ImGui::Checkbox("BoxFilled", &g.g_ESP_BoxFilled);
        ImGui::Checkbox("Line", &g.g_ESP_Line);
        ImGui::Checkbox("Name", &g.g_ESP_Name);
        ImGui::Checkbox("Distance", &g.g_ESP_Distance);
        ImGui::Checkbox("HealthBar", &g.g_ESP_HealthBar);

        ImGui::EndChild();
        break;
    case 2: { // misc
        ImGui::BeginChild("##C020", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 3.f), true);
        ImGui::Text("Misc");
        ImGui::Separator();
        ImGui::Spacing();
        //ImGui::Checkbox("BunnyHop", &g.g_BunnyHop);
        ImGui::Checkbox("NoReocil", &g.g_NoRecoil);

        ImGui::EndChild();
        ImGui::BeginChild("##C021", ImVec2(ImGui::GetContentRegionAvail()), true);

        // NoRecoil
        ImGui::Text("NoRecoil");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::CustomSliderFloat("Recoil", "##recoilptg", &g.g_NoRecoilVal, 0.f, 1.f, "%.2f");

        ImGui::NewLine();

        // Custom AimAssist Value
        static float new_aa = 0.8f;

        ImGui::Text("AimAssist Value Changer");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::CustomSliderFloat("AimAssist", "##aa", &new_aa, 0.f, 1.f, "%.2f");

        if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
            m.Write<float>(m.m_gProcessBaseAddr + offset::AimAssistVal, new_aa);

        ImGui::EndChild();
    } break;
    case 3: // system
        ImGui::BeginChild("##C030", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 1.75f), true);

        ImGui::Text("System");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("Crosshair", &g.g_Crosshair);
        ImGui::Checkbox("StreamProof", &g.g_StreamProof);

        ImGui::CustomSliderInt("Max Framerate", "##MaxFrame", &g.g_MaxFramerate, 30, 500);

        ImGui::EndChild();
        ImGui::BeginChild("##C031", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("Crosshair");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Crosshair", &g.g_Crosshair);
        ImGui::CustomSliderInt("CrosshairSize", "##SizeCH", &g.g_CrosshairSize, 1, 10);
        ImGui::ColorEdit4("Color##C", &CrosshairColor.Value.x);
        ImGui::Combo("Type##C", &g.g_CrosshairType, CrosshairList, IM_ARRAYSIZE(CrosshairList));

        ImGui::EndChild();
        break;
    default:
        break;
    }

    ImGui::PopStyleColor();

    ImGui::EndChild();
    //---------------------------------------------------//

    ImGui::SameLine();

    //---// Right //--------------------------------------//
    ImGui::BeginChild("##RightChild", ImVec2(ImGui::GetContentRegionAvail()), false);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));

    switch (Index)
    {
    case 0: // aim
        ImGui::BeginChild("##C100", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 1.75f), true);
        ImGui::Text("Setting");
        ImGui::Separator();
        ImGui::Spacing();
        style.ItemSpacing.y = 3.f;
        ImGui::CustomSliderFloat("FOV", "##aim_fov", &g.g_Aim_Fov, 50.f, 1000.f);
        ImGui::CustomSliderFloat("Smooth", "##aim_smt", &g.g_Aim_Smooth, 1.f, 30.f);
        ImGui::CustomSliderFloat("Distance", "##aim_dist", &g.g_Aim_MaxDistance, 50.f, 300.f);
        style.ItemSpacing.y = DefaultSpacing;
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Combo("AimType", &g.g_Aim_Type, AimTypeList, IM_ARRAYSIZE(AimTypeList));
        ImGui::Combo("AimMode", &g.g_Aim_Mode, AimModeList, IM_ARRAYSIZE(AimModeList));

        ImGui::EndChild();
        ImGui::BeginChild("##C101", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("KeyBind");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("1st Key:");
        if (ImGui::Button(BindingID == 1 ? "< Press Any Key >" : KeyNames[g.g_AimKey0], ImVec2(215.f, 22.5f))) {
            BindingID = 1;
            std::thread([&]() { KeyBinder(g.g_AimKey0, BindingID); }).detach();
        }

        ImGui::PushItemWidth(215.f);
        ImGui::Combo("##KeyMode", &g.g_AimKeyType, AimKeyTypeList, IM_ARRAYSIZE(AimKeyTypeList));
        ImGui::PopItemWidth();
        ImGui::Spacing();

        ImGui::Text("2nd Key:");
        if (ImGui::Button(BindingID == 2 ? "< Press Any Key >" : KeyNames[g.g_AimKey1], ImVec2(215.f, 22.5f))) {
            BindingID = 2;
            std::thread([&]() { KeyBinder(g.g_AimKey1, BindingID); }).detach();
        }

        ImGui::EndChild();
        break;
    case 1: // visual
        ImGui::BeginChild("##C110", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.5f), true);

        ImGui::Text("ESP Setting");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::CustomSliderInt("Distance", "##Dist", &g.g_ESP_MaxDistance, 100, 2000);
       
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Combo("BoxType", &g.g_ESP_BoxType, BoxTypeList, IM_ARRAYSIZE(BoxTypeList));
        ImGui::Combo("ESP Mode", &g.g_ESP_RenderMode, ESP_RenderModeList, IM_ARRAYSIZE(ESP_RenderModeList));

        ImGui::EndChild();
        ImGui::BeginChild("##C111", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("ESP Colors");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::ColorEdit4("Default", &ESP_Default.Value.x);
        ImGui::ColorEdit4("Visible", &ESP_Visible.Value.x);
        ImGui::ColorEdit4("Team", &ESP_Team.Value.x);
        ImGui::ColorEdit4("Shadow", &ESP_Shadow.Value.x);

        ImGui::EndChild();
        break;
    case 2: // misc
        ImGui::BeginChild("##120", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::EndChild();
        break;
    case 3: // system
        ImGui::BeginChild("##130", ImVec2(ImGui::GetContentRegionAvail()), true);

        ImGui::Text("Exit");
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Exit", ImVec2(ImGui::GetContentRegionAvail().x, 30.f)))
            g.g_Run = false;

        ImGui::EndChild();
        break;
    default:
        break;
    }

    ImGui::PopStyleColor();

    ImGui::EndChild();
    ImGui::EndChild();
    //---------------------------------------------------//

    ImGui::End();
}