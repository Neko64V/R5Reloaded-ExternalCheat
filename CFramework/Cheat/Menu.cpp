#include "FrameCore.h"

// ImGui::Combo/ImGui::List等で使う文字列群
const char* BoxTypeList[] = { "Simple", "Cornered" };
const char* AimTypeList[] = { "FOV", "Distance" };
const char* AimModeList[] = { "Memory", "Mouse" };
const char* AimBoneList[] = { "Head", "Chest" };
const char* AimKeyTypeList[] = { "and", "or" };
const char* CrosshairList[] = { "Cross", "Circle" };
std::vector<std::string> MenuSelectList = { "AimBot", "Visual", "Misc", "System" };

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

        ImGui::Checkbox("AimBot", &g.g_AimBot);
        ImGui::Checkbox("Prediction", &g.g_Aim_Prediction);
        ImGui::Checkbox("Visibility Check", &g.g_Aim_VisCheck);

        ImGui::Spacing();
        ImGui::NewLine();

        ImGui::Text("Aim Options");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Aim at NPC", &g.g_Aim_NPC);
        ImGui::Checkbox("Aim at Team", &g.g_Aim_Team);
        ImGui::Combo("AimBone", &g.g_Aim_Bone, AimBoneList, IM_ARRAYSIZE(AimBoneList));

        ImGui::Spacing();
        ImGui::NewLine();

        ImGui::Text("FOV");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("DrawFOV", &g.g_Aim_DrawFov);
        ImGui::CustomSliderFloat("FOV", "##aim_fov", &g.g_Aim_Fov, 50.f, 1000.f);
        ImGui::ColorEdit4("FOV Color", &FOV_User.Value.x);
        break;
    case 1: // visual
        ImGui::Text("Visual");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("ESP", &g.g_ESP);
        ImGui::Checkbox("Glow", &g.g_ESP_Glow);
        ImGui::Checkbox("NPC ESP", &g.g_ESP_NPC);
        ImGui::Checkbox("Team ESP", &g.g_ESP_Team);

        ImGui::NewLine();
        ImGui::Spacing();

        ImGui::Text("ESP Options");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Box", &g.g_ESP_Box);
        ImGui::Checkbox("BoxFilled", &g.g_ESP_BoxFilled);
        ImGui::Checkbox("Line", &g.g_ESP_Line);
        ImGui::Checkbox("Name", &g.g_ESP_Name);
        ImGui::Checkbox("Distance", &g.g_ESP_Distance);
        ImGui::Checkbox("HealthBar", &g.g_ESP_HealthBar);
        break;
    case 2: // misc
        ImGui::Text("Misc");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("BunnyHop", &g.g_BunnyHop);

        ImGui::NewLine();

        ImGui::Text("NoRecoil");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("NoReocil", &g.g_NoRecoil);
        ImGui::CustomSliderFloat("Recoil percentage", "##recoilptg", &g.g_NoRecoilVal, 0.f, 1.f);
        break;
    case 3: // system
        ImGui::Text("System");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("StreamProof", &g.g_StreamProof);

        ImGui::Spacing();
        ImGui::NewLine();

        ImGui::Text("Crosshair");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Crosshair", &g.g_Crosshair);
        ImGui::CustomSliderInt("CrosshairSize", "##SizeCH", &g.g_CrosshairSize, 1, 10);
        ImGui::ColorEdit4("Color##C", &CrosshairColor.Value.x);
        ImGui::Combo("Type##C", &g.g_CrosshairType, CrosshairList, IM_ARRAYSIZE(CrosshairList));
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
        ImGui::CustomSliderFloat("FOV", "##aim_fov", &g.g_Aim_Fov, 50.f, 1000.f);
        ImGui::CustomSliderFloat("Smooth", "##aim_smt", &g.g_Aim_Smooth, 1.f, 20.f);
        ImGui::CustomSliderFloat("Distance", "##aim_dist", &g.g_Aim_MaxDistance, 50.f, 300.f);
        style.ItemSpacing.y = DefaultSpacing;
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Combo("AimType", &g.g_Aim_Type, AimTypeList, IM_ARRAYSIZE(AimTypeList));
        ImGui::Combo("AimMode", &g.g_Aim_Mode, AimModeList, IM_ARRAYSIZE(AimModeList));

        ImGui::Spacing();
        ImGui::NewLine();

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
        break;
    case 1: // visual
        ImGui::Text("ESP Setting");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::CustomSliderFloat("Distance", "##Dist", &g.g_ESP_MaxDistance, 100.f, 2000.f);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Combo("BoxType", &g.g_ESP_BoxType, BoxTypeList, IM_ARRAYSIZE(BoxTypeList));

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
    case 2: // misc

        break;
    case 3: // system
        ImGui::Text("Exit");
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Exit", ImVec2(ImGui::GetContentRegionAvail().x, 30.f)))
            g.g_Run = false;
        break;
    default:
        break;
    }

    ImGui::EndChild();
    ImGui::EndChild();
    //---------------------------------------------------//

    ImGui::End();
}