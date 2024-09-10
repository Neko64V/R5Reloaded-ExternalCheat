#include "FrameCore.h"

// ImGui::Combo/ImGui::List等で使う文字列群
const char* BoxTypeList[] = { "Simple", "Cornered" };
const char* AimTypeList[] = { "FOV", "Distance" };
const char* AimModeList[] = { "Memory", "Mouse" };
const char* AimBoneList[] = { "Head", "Chest" };
const char* AimKeyTypeList[] = { "and", "or" };
const char* CrosshairList[] = { "Cross", "Circle" };
std::vector<std::string> MenuSelectList = { "AimBot", "Visual", "System" };

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