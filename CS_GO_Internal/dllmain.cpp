#include "includes.h"

// hook function
void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice) {
    if (!pDevice)
        pDevice = o_pDevice;

    DrawMenu(o_pDevice);
    
    if (toggles::ESP_toggle) {
        ESP();
    }

    // call og function
    oEndScene(pDevice);
}


// This is the hook we placed in WndProc
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    // if the menu is open. then ignore the cs go Input handler and only process the ImGUI hander
    if (toggles::show_menu) {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        return true;
    }
    else { // else if the menu is closed then make sure to return the games input handler else input wont register
        return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
    }

}


// This is the Main_thread where the hack features get executed
DWORD WINAPI Main_thread(HMODULE hModule) {

    
    client_module = (DWORD)GetModuleHandle(L"client.dll");
    engine_module = (DWORD)GetModuleHandle(L"engine.dll");
    DWORD localplayer = *(DWORD*)(client_module + hazedumper::signatures::dwLocalPlayer);
    DWORD clientstate = *(DWORD*)((*(DWORD*)(engine_module + hazedumper::signatures::dwClientState)) + 0x108);
    

    // hook endscene so that we can draw out GUI using the game's dirextx9 engine.
    if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {

        // copy the patched bytes. so that when we uninject we can reallocate the stolen bytes.
        memcpy(EndSceneBytes, (char*)d3d9Device[42], 7);

        // use a trampHook to hook the EndScene() function in DX9
        oEndScene = (tEndScene)TrampHook((BYTE*)d3d9Device[42], (BYTE*)hkEndScene, 7, true);
    }


    // this is the hack loop. panic key is END
    while (!GetAsyncKeyState(VK_END) & 1) {
        
        
        viewmatrix = (float *)(client_module + hazedumper::signatures::dwViewMatrix);
        clientstate = *(DWORD*)((*(DWORD*)(engine_module + hazedumper::signatures::dwClientState)) + 0x108);
        DWORD* entlist = (DWORD*)((client_module + hazedumper::signatures::dwEntityList));

        // check if player is in game. else wait until player is in game.
        while (clientstate != 6) {
            if (GetAsyncKeyState(VK_INSERT) & 1) {
                toggles::show_menu = !toggles::show_menu;
            }

            clientstate = *(DWORD*)((*(DWORD*)(engine_module + hazedumper::signatures::dwClientState)) + 0x108);
            Sleep(1000);
        }

        //once player is in game. find the localplayer object and start the hack

        DWORD localplayer = *(DWORD*)(client_module + hazedumper::signatures::dwLocalPlayer);

        // Press insert to toggle menu
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            toggles::show_menu = !toggles::show_menu;
        }

        // enumerate the entity list
        for (int i = 0; i < 32; i++) {
            DWORD current_entity = *(DWORD*)((client_module + hazedumper::signatures::dwEntityList) + i * 0x10);
            
            // insure both entity's are not invalid
            if (current_entity == NULL || localplayer == NULL) {
                continue;
            }

            // insure that entity and localplayer are on different teams
            if ((*(BYTE*)(current_entity + 0xf4)) == (*(BYTE*)(localplayer + 0xf4))) {
                continue;
            }
            
            // if aimbot is toggled.
            if (toggles::aimbot_toggle) {
                
                // find player closest to localplayers crosshair.
                DWORD bestent = BestTarget2(localplayer);
                if (bestent) {
                    aimbot_entity = bestent;
                }
                else {                  
                    aimbot_entity = NULL;
                }

                // if player is holding the correct aimbot key. then lock on the the closest player.
                if (GetAsyncKeyState(aimbot_key)) {
                    aimbot(bestent, localplayer);
                }
            }

            // if radar is toggled
            if (toggles::Radar_toggle) {
                radar(current_entity);
            }

            // if Bhop is toggled
            if (toggles::Bhop_toggle) {
                bhop(localplayer);
            }

            // if NoFlash is toggled
            if (toggles::noflash_toggle) {
                noflash(localplayer);
            }

            // if triggerbot is toggled
            if (toggles::triggerbot_toggle) {
                triggerbot(localplayer);
            }
        }
    }
    
    // when panic key is pressed make sure to deallocate IMGUI
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    Sleep(1000);

    // Patch the hook on ENDSCENE
    Patch((BYTE*)d3d9Device[42], (BYTE*)EndSceneBytes, 7);

    Sleep(1000);
    // Free The DLL and Exit the hack thread
    FreeLibraryAndExitThread(hModule, 0);
    return 0;


}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        // when the dll is attached create a thread that starts executing Main_thread
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Main_thread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

