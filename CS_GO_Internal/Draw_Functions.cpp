#include "Includes.h"

WNDPROC oWndProc = 0;

// converts an IMGUI VEC4 into a D3DCOLOR
D3DCOLOR FLOAT4TOD3DCOLOR(float Col[])
{
    ImU32 col32_no_alpha = ImGui::ColorConvertFloat4ToU32(ImVec4(Col[0], Col[1], Col[2], Col[3]));
    float a = (col32_no_alpha >> 24) & 255;
    float r = (col32_no_alpha >> 16) & 255;
    float g = (col32_no_alpha >> 8) & 255;
    float b = col32_no_alpha & 255;
    return D3DCOLOR_ARGB((int)a, (int)b, (int)g, (int)r);
}

// function which draws a line from (x1, y1) to (x2, y2). you specifiy the thickness of the line and the color
void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color)
{
    static ID3DXLine* LineL;
    if (!LineL) {
        D3DXCreateLine(pDevice, &LineL);
    }
    
    D3DXVECTOR2 Line[2];
    Line[0] = D3DXVECTOR2(x1, y1);
    Line[1] = D3DXVECTOR2(x2, y2);
    LineL->SetWidth(thickness);
    LineL->Draw(Line, 2, color);
}


// this is the function which is responsible for the ESP (wallhacks)
void ESP() {

    // get the localplayer object.
    DWORD localplayer = *(DWORD*)(client_module + hazedumper::signatures::dwLocalPlayer);

    // loop through the entity list
    for (int i = 0; i < 32; i++) {
        DWORD current_entity = *(DWORD*)((client_module + hazedumper::signatures::dwEntityList) + i * 0x10);
        
        // check if the current entity is valid (alive, not NULL... etc)
        if (!ValidEnt(current_entity, localplayer)) {
            continue;
        }

        // create 2 vec2 that will hold the screen coordinates of the head and base of the entity.
        vec2 headScreenPos;
        vec2 originScreenPos;


        // if entity is on the same team as localplayer execute this code
        if ((*(BYTE*)(current_entity + 0xf4)) == (*(BYTE*)(localplayer + 0xf4))) {
            

            // get the 3d coordinates of entity's head
            vec3 headbone_pos;
            uintptr_t bonematrix = (*(uintptr_t*)(current_entity + hazedumper::netvars::m_dwBoneMatrix));
            headbone_pos.x = *(float*)(bonematrix + 0x30 * 8 + 0xC);
            headbone_pos.y = *(float*)(bonematrix + 0x30 * 8 + 0x1C);
            headbone_pos.z = *(float*)(bonematrix + 0x30 * 8 + 0x2C);


            // get the 3d coordinates of entity's base
            vec3 originpos = *(vec3*)(current_entity + hazedumper::netvars::m_vecOrigin);


           // world to screen to entity base
           bool head = WorldToScreen(headbone_pos, headScreenPos, viewmatrix, windowWidth, windowHeight);
            

           // world to screen to entity head
           bool origin = WorldToScreen(originpos, originScreenPos, viewmatrix, windowWidth, windowHeight);


            // create a ratio for height to width
            double height = sqrt((pow((headScreenPos.y - originScreenPos.y), 2) + pow((headScreenPos.x - originScreenPos.x), 2)));
            float width = (height / 2.2);


            D3DCOLOR color_red = 0xFFFF0000;
            D3DCOLOR color_green = 0xFF00FF00;

            // if worldtoscreen was sucessful (the entity was infront of you). then start drawing them
            if ((head) && (origin)) {

                // if BoxESP is toggled. draw box around entity
                if (toggles::TeamMate_Box_Esp) {

                    // draw rectangle;
                    DrawLine((headScreenPos.x - (width / 2)), headScreenPos.y - (height / 8), (headScreenPos.x + (width / 2)), headScreenPos.y - (height / 8), 2, ESP_Colors::Team_Box_Esp_C);
                    DrawLine((headScreenPos.x + (width / 2)), headScreenPos.y - (height / 8), (headScreenPos.x + (width / 2)), originScreenPos.y, 2, ESP_Colors::Team_Box_Esp_C);
                    DrawLine((headScreenPos.x - (width / 2)), headScreenPos.y - (height / 8), (headScreenPos.x + -(width / 2)), originScreenPos.y, 2, ESP_Colors::Team_Box_Esp_C);
                    DrawLine((headScreenPos.x - (width / 2)), originScreenPos.y, (headScreenPos.x + (width / 2)), originScreenPos.y, 2, ESP_Colors::Team_Box_Esp_C);
                }

                // if HealthESP is toggled. draw health bar on entity
                if (toggles::TeamMate_Health_ESP) {

                    
                    // get entity HP
                    int health = *(int*)(current_entity + 0x100);

                    // if entity HP is 100 (full health) then just draw a green bar.
                    if (health == 100) {
                        // draw healthbar all green
                        DrawLine((headScreenPos.x + (width / 2)) + 10, headScreenPos.y - (height / 8), (headScreenPos.x + (width / 2)) + 10, originScreenPos.y, 5, color_green);

                    }
                    else {

                        // find out what percentage of the bar should be green and what percentage should be red

                        float health_percentage = (1 - ((float)health / 100.0f));
                        float difference = height * health_percentage;

                        //draw lost health
                        DrawLine((headScreenPos.x + (width / 2)) + 10, (headScreenPos.y - (height / 8)), (headScreenPos.x + (width / 2)) + 10, (headScreenPos.y - (height / 8)) + difference, 5, color_red);

                        // draw remaining health
                        DrawLine((headScreenPos.x + (width / 2)) + 10, (headScreenPos.y - (height / 8)) + difference, (headScreenPos.x + (width / 2)) + 10, originScreenPos.y, 5, color_green);

                    }
                }

                // if snapline ESP is toggled. draw a line from the bottom centre of screen to the entity base
                if (toggles::TeamMate_snaplines) {
                    DrawLine(windowWidth / 2, windowHeight, originScreenPos.x, originScreenPos.y, 2, ESP_Colors::Team_snaplines_C);
                }

                if ((!toggles::Team_check) && (toggles::Aimbot_snaplines) && (aimbot_entity == current_entity)) {
                    DrawLine(windowWidth / 2, windowHeight, originScreenPos.x, originScreenPos.y, 2, ESP_Colors::Aimbot_snaplines_C);
                }

            }

            


            
        } // this code gets executed if the entity is not on the same team as localplayer
        else {

            // get the 3d coord of entity's head
            vec3 headbone_pos;
            uint32_t bonematrix = (*(uint32_t*)(current_entity + hazedumper::netvars::m_dwBoneMatrix));
            headbone_pos.x = *(float*)(bonematrix + 0x30 * 8 + 0xC);
            headbone_pos.y = *(float*)(bonematrix + 0x30 * 8 + 0x1C);
            headbone_pos.z = *(float*)(bonematrix + 0x30 * 8 + 0x2C);


            // get 3d coord of entity's body
            vec3 originpos = *(vec3*)(current_entity + hazedumper::netvars::m_vecOrigin);
            

            // world to screen to entity base
            bool head = WorldToScreen(headbone_pos, headScreenPos, viewmatrix, windowWidth, windowHeight);

            // world to screen to entity head
            bool origin = WorldToScreen(originpos, originScreenPos, viewmatrix, windowWidth, windowHeight);
            
            // create a ratio for height to width
            double height = sqrt((pow((headScreenPos.y - originScreenPos.y), 2) + pow((headScreenPos.x - originScreenPos.x), 2)));
            float width = height / 2.2;
            
            D3DCOLOR color_red = 0xFFFF0000;
            D3DCOLOR color_green = 0xFF00FF00;

            // check if entity is on the screen (in front of palyer)
            if ((head) && (origin)) {

                // if BOXESP is toggled then draw box around entity
                if (toggles::Enemy_Box_Esp) {
                    DrawLine((headScreenPos.x - (width / 2)), headScreenPos.y - (height / 8), (headScreenPos.x + (width / 2)), headScreenPos.y - (height / 8), 2, ESP_Colors::Enemy_Box_Esp_C);
                    DrawLine((headScreenPos.x + (width / 2)), headScreenPos.y - (height / 8), (headScreenPos.x + (width / 2)), originScreenPos.y, 2, ESP_Colors::Enemy_Box_Esp_C);
                    DrawLine((headScreenPos.x - (width / 2)), headScreenPos.y - (height / 8), (headScreenPos.x + -(width / 2)), originScreenPos.y, 2, ESP_Colors::Enemy_Box_Esp_C);
                    DrawLine((headScreenPos.x - (width / 2)), originScreenPos.y, (headScreenPos.x + (width / 2)), originScreenPos.y, 2, ESP_Colors::Enemy_Box_Esp_C);
                }

                // if Health ESP is toggled Draw health on entity
                if (toggles::Enemy_Health_ESP) {

                    // get entity HP
                    int health = *(int*)(current_entity + 0x100);

                    // if entity is 100 HP then just draw a green bar
                    if (health == 100) {
                        // draw healthbar all green
                        DrawLine((headScreenPos.x + (width / 2)) + 10, headScreenPos.y - (height / 8), (headScreenPos.x + (width / 2)) + 10, originScreenPos.y, 5, color_green);

                    }
                    else {

                        // else find out what percentage of the bar should be red

                        float health_percentage = (1 - ((float)health / 100.0f));
                        float difference = height * health_percentage;

                        //draw lost health
                        DrawLine((headScreenPos.x + (width / 2)) + 10, (headScreenPos.y - (height / 8)), (headScreenPos.x + (width / 2)) + 10, (headScreenPos.y - (height / 8)) + difference, 5, color_red);

                        // draw remaining health
                        DrawLine((headScreenPos.x + (width / 2)) + 10, (headScreenPos.y - (height / 8)) + difference, (headScreenPos.x + (width / 2)) + 10, originScreenPos.y, 5, color_green);

                    }
                }

                

                // check if ESP snapline is toggled. if so draw snapline to entity
                if (toggles::Enemy_snaplines) {
                    DrawLine(windowWidth / 2, windowHeight, originScreenPos.x, originScreenPos.y, 2, ESP_Colors::Enemy_snaplines_C);
                }

                // check if aimbot_entity is available and check if aimbot snaplines are toggled. if so draw aimbot_snapline
                if ((aimbot_entity == current_entity) && toggles::Aimbot_snaplines) {
                        DrawLine(windowWidth / 2, windowHeight, originScreenPos.x, originScreenPos.y, 2, ESP_Colors::Aimbot_snaplines_C);
                }
            }
        }
    }
}


// Initilizes ImGui and also stores the WNDPROC of the Game into OWndPROC. 
void InitImGui(IDirect3DDevice9* pDevice) {

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    D3DDEVICE_CREATION_PARAMETERS d3dcp{ 0 };
    pDevice->GetCreationParameters(&d3dcp);

    oWndProc = (WNDPROC)SetWindowLongPtr(d3dcp.hFocusWindow, GWL_WNDPROC, (LONG_PTR)WndProc);

    //io.Fonts->AddFontDefault();
    ImGui_ImplWin32_Init(d3dcp.hFocusWindow);
    ImGui_ImplDX9_Init(pDevice);
    init = true;
}

void menuSpacing() {
    for (int i = 0; i < 5; i++) {
        ImGui::Spacing();
    }
}


// this is the code responsible for drawing the ingame menu
void DrawMenu(LPDIRECT3DDEVICE9 o_pDevice) {


    //if ImGui is Not initilized. then initilize it
    if (!init) {
        InitImGui(pDevice);
    }
    else {

        // setup the ImGui and DX9 frame

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // setup the style of the menu
        ImGui::SetNextWindowSize({ 1200, 640 });
        ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5, 0.5);
        ImGui::GetStyle().WindowRounding = 0.0f;
        ImGui::GetStyle().ButtonTextAlign;
        ImGui::PushStyleColor(3, { 1.0f, 0.0f, 0.0f, 1.0f });



        // if showmenu is true. then start rendering the menu
        if (toggles::show_menu)
        {
            ImGui::Begin("Rep's Internal");

            ImGui::BeginChildFrame(1, ImVec2(400.0f, 0.0f));
            {

                ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "ESP");

                menuSpacing();

                ImGui::Checkbox("ESP", &toggles::ESP_toggle);

                menuSpacing();

                ImGui::Checkbox("Enemy Box ESP  ", &toggles::Enemy_Box_Esp);

                ImGui::SameLine();


                if (ImGui::ColorEdit3("", (float*)&E_Box_ESP)) {
                    ESP_Colors::Enemy_Box_Esp_C = FLOAT4TOD3DCOLOR((float*)&E_Box_ESP);
                };

                menuSpacing();

                ImGui::Checkbox("Team Box ESP   ", &toggles::TeamMate_Box_Esp);

                ImGui::SameLine();               

                if (ImGui::ColorEdit3("1", (float*)&T_Box_ESP)) {
                    ESP_Colors::Team_Box_Esp_C = FLOAT4TOD3DCOLOR((float*)&T_Box_ESP);
                };

                menuSpacing();

                ImGui::Checkbox("Enemy SnapLines", &toggles::Enemy_snaplines);

                ImGui::SameLine();

                if (ImGui::ColorEdit3("2", (float*)&E_snap_ESP)) {
                    ESP_Colors::Enemy_snaplines_C = FLOAT4TOD3DCOLOR((float*)&E_snap_ESP);
                };

                menuSpacing();

                ImGui::Checkbox("Team SnapLines ", &toggles::TeamMate_snaplines);

                ImGui::SameLine();


                if (ImGui::ColorEdit3("3", (float*)&T_snap_ESP)) {
                    ESP_Colors::Team_snaplines_C = FLOAT4TOD3DCOLOR((float*)&T_snap_ESP);
                };

                menuSpacing();

                ImGui::Checkbox("Enemy Health ESP", &toggles::Enemy_Health_ESP);

                menuSpacing();

                ImGui::Checkbox("Team Health ESP", &toggles::TeamMate_Health_ESP);

                menuSpacing();

                ImGui::Checkbox("Aimbot SnapLine", &toggles::Aimbot_snaplines);

                ImGui::SameLine();

                if (ImGui::ColorEdit3("4", (float*)&A_Snap_ESP)) {
                    ESP_Colors::Aimbot_snaplines_C = FLOAT4TOD3DCOLOR((float*)&A_Snap_ESP);
                };

            }
            ImGui::EndChildFrame();
            ImGui::SameLine();
            ImGui::BeginChildFrame(2, ImVec2(400.0f, 0.0f));
            {

                ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "AIMBOT");

                menuSpacing();

                ImGui::Checkbox("Aimbot", &toggles::aimbot_toggle);

                menuSpacing();



                ImGui::Checkbox("Team Check", &toggles::Team_check);

                menuSpacing();

                ImGui::SliderFloat("Pixel FOV", &aimbot_fov, 0.0, 300.0, "%f");

                menuSpacing();


                if (ImGui::Combo("Aimbot Key", &aimbot_list_key, aimbot_items, 3)) {

                    if (aimbot_list_key == 0) {
                        aimbot_key = VK_LSHIFT;
                    }
                    else if (aimbot_list_key == 1) {
                        aimbot_key = VK_RBUTTON;
                    }
                    else {
                        aimbot_key = VK_CAPITAL;
                    }

                }

            }
            ImGui::EndChildFrame();
            ImGui::SameLine();
            ImGui::BeginChildFrame(3, ImVec2(400.0f, 0.0f));
            {

                ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "MISC");


                menuSpacing();


                ImGui::Checkbox("BHOP", &toggles::Bhop_toggle);


                menuSpacing();


                ImGui::Checkbox("NoFlash", &toggles::noflash_toggle);


                menuSpacing();


                ImGui::Checkbox("Trigger", &toggles::triggerbot_toggle);

                menuSpacing();

                ImGui::Checkbox("Radar ESP", &toggles::Radar_toggle);
            }

            ImGui::EndChildFrame();






            ImGui::End();
        }


        // once the menu has been drawn. end the frame and then render it on the screen
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }


}




