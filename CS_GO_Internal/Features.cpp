#pragma once
#include "Includes.h"

DWORD client_modules = (DWORD)GetModuleHandle(L"client.dll");
constexpr float pi = 3.1415926535;

namespace toggles {

    // ESP toggles
    bool Enemy_Box_Esp = 0;
    bool TeamMate_Box_Esp = 0;
    bool Enemy_snaplines = 0;
    bool TeamMate_snaplines = 0;
    bool Enemy_Health_ESP = 0;
    bool TeamMate_Health_ESP = 0;
    bool Aimbot_snaplines = 0;


    //aimbot toggles
    bool Team_check = 0;
    bool aimbot_toggle = 0;


    // misc toggles
    bool Radar_toggle = 0;
    bool Bhop_toggle = 0;
    bool noflash_toggle = 0;
    bool triggerbot_toggle = 0;
    bool ESP_toggle = 0;

    bool show_menu = 0;

};

// array of items for aimbotkey drop down menu
const char* aimbot_items[] = { "Left Shift" , "Right Mouse", "CAPS" };


// ESP colors 
namespace ESP_Colors {

    D3DCOLOR Enemy_Box_Esp_C = 0xFFFF0000;
    D3DCOLOR Team_Box_Esp_C = 0xFF00FF00;
    D3DCOLOR Enemy_snaplines_C = 0xFFFF0000;
    D3DCOLOR Team_snaplines_C = 0xFF00FF00;
    D3DCOLOR Aimbot_snaplines_C = 0xFFFF00FF;
};

// handles to client.dll and engine.dll
DWORD client_module = NULL;
DWORD engine_module = NULL;

// colors which ImGUI uses
ImVec4 E_Box_ESP = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
ImVec4 T_Box_ESP = ImVec4(0.0f, 1.00f, 0.0f, 1.00f);
ImVec4 E_snap_ESP = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
ImVec4 T_snap_ESP = ImVec4(0.0f, 1.00f, 0.0f, 1.00f);
ImVec4 A_Snap_ESP = ImVec4(1.00f, 0.0f, 1.00f, 1.0f);

// WNDPROC and ENDSCENE() hook variables
uintptr_t Game_WNDPROC;
void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
LPDIRECT3DDEVICE9 pDevice = nullptr;

// aimbot values used for IMGUI
float aimbot_fov = 0;
int aimbot_key = VK_SHIFT;
int aimbot_list_key = 0;



bool init = false;
float* viewmatrix = nullptr;
DWORD aimbot_entity = NULL;





// radarESP code
void radar(DWORD entity) {
    bool bdormant = *(bool*)(entity + hazedumper::signatures::m_bDormant);

    if (!bdormant) {
        *(BYTE*)(entity + hazedumper::netvars::m_bSpotted) = 1;
    }

    
}

// bhop code
void bhop(DWORD entity) {
    BYTE flag = *(BYTE*)(entity + hazedumper::netvars::m_fFlags);
    if (GetAsyncKeyState(VK_SPACE) && (flag & (1 << 0))) {
        *(DWORD*)(client_modules + hazedumper::signatures::dwForceJump) = 6;
    }

}

// noflash code
void noflash(DWORD entity) {
    (*(DWORD*)(entity + hazedumper::netvars::m_flFlashDuration)) = 0;
}


// triggerbot code
void triggerbot(DWORD entity) {
    DWORD entity_to_check = *(DWORD*)(entity + hazedumper::netvars::m_iCrosshairId);
    DWORD current_entity = *(DWORD*)((client_modules + hazedumper::signatures::dwEntityList) + (entity_to_check - 1 ) * 0x10);
    if ((entity_to_check > 32) || (entity_to_check <= 0) || (current_entity == 0)) {
        return;
    }
    if ((*(DWORD*)(entity + hazedumper::netvars::m_iTeamNum)) != (*(DWORD*)(current_entity + hazedumper::netvars::m_iTeamNum))) {
        
       (*(DWORD*)(client_modules + hazedumper::signatures::dwForceAttack)) = 2;
    }

    
    
}

// used for aimbot calc
vec3 Subtract(vec3 src, vec3 dst)
{
    vec3 diff;
    diff.x = src.x - dst.x;
    diff.y = src.y - dst.y;
    diff.z = src.z - dst.z;
    return diff;
}

// get magnitude of vec3
float Magnitude(vec3 vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

// get the distance between two floats
float Distance(float src, float dst)
{

    if (dst < src) {
        return abs(dst - src);
    }
    else {
        return abs(src - dst);
    }

    
}

// calculate the required angle's to aim at using the localplayer pos vec3 and entity pos vec3
vec3 CalcAngle(vec3 src, vec3 dst)
{

    vec3 resultant_vector;
    resultant_vector.x = dst.x - src.x;
    resultant_vector.y = dst.y - src.y;
    resultant_vector.z = dst.z - src.z;

    float deltalength = sqrt((resultant_vector.x * resultant_vector.x) + (resultant_vector.y * resultant_vector.y) + (resultant_vector.z * resultant_vector.z));

    float yaw = atan2(resultant_vector.y, resultant_vector.x) * (180 / pi);
    float pitch = -asin(resultant_vector.z / deltalength) * (180 / pi);


    vec3 angle;
    angle.x = pitch;
    angle.y = yaw;
    angle.z = 0.0f;

    return angle;
}


// used to fill out the localplayer position and the entity  3D positions
void fillposvecs(DWORD localplayer, DWORD ent, vec3* src, vec3* dst) {

    uint32_t bonematrix = (*(uint32_t*)(ent + hazedumper::netvars::m_dwBoneMatrix));
    static vec3 headbone_pos;
    static DWORD engineModule = (DWORD)GetModuleHandle(L"engine.dll");
    static vec3* viewAngles = (vec3*)(*(DWORD*)(engineModule + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);
    

    headbone_pos.x = *(float*)(bonematrix + 0x30 * 8 + 0xC);
    headbone_pos.y = *(float*)(bonematrix + 0x30 * 8 + 0x1C);
    headbone_pos.z = *(float*)(bonematrix + 0x30 * 8 + 0x2C);
    

    vec3 entorigin = *(vec3*)((ent)+hazedumper::netvars::m_vecOrigin);

    vec3 localorigin = *(vec3*)((localplayer) + hazedumper::netvars::m_vecOrigin);
    vec3 localviewangle = *(vec3*)((localplayer) + hazedumper::netvars::m_vecViewOffset);
    
    vec3 pos;
    pos.x = localorigin.x + localviewangle.x;
    pos.y = localorigin.y + localviewangle.y;
    pos.z = localorigin.z + localviewangle.z;
    

    src->x = pos.x;
    src->y = pos.y;
    src->z = pos.z;
    
    dst->x = headbone_pos.x;
    dst->y = headbone_pos.y;
    dst->z = headbone_pos.z;

}


// aimbot function
void aimbot(DWORD entity, DWORD localplayer) {

    
    if (!entity) {
        return;
    }

    if ((*(int*)(entity + 0x100) <= 0)) {
        return;
    }

    //while ((* (int*)(entity + 0x100) > 0)) {
        uint32_t bonematrix = (*(uint32_t*)(entity + hazedumper::netvars::m_dwBoneMatrix));
        static vec3 headbone_pos;
        static DWORD engineModule = (DWORD)GetModuleHandle(L"engine.dll");
        static vec3* viewAngles = (vec3*)(*(DWORD*)(engineModule + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);


        headbone_pos.x = *(float*)(bonematrix + 0x30 * 8 + 0xC);
        headbone_pos.y = *(float*)(bonematrix + 0x30 * 8 + 0x1C);
        headbone_pos.z = *(float*)(bonematrix + 0x30 * 8 + 0x2C);


        vec3 localorigin = *(vec3*)(localplayer + hazedumper::netvars::m_vecOrigin);
        vec3 localviewangle = *(vec3*)(localplayer + hazedumper::netvars::m_vecViewOffset);
        vec3 pos;
        pos.x = localorigin.x + localviewangle.x;
        pos.y = localorigin.y + localviewangle.y;
        pos.z = localorigin.z + localviewangle.z;


        vec3 resultant_vector;
        resultant_vector.x = (headbone_pos.x - pos.x);
        resultant_vector.y = (headbone_pos.y - pos.y);
        resultant_vector.z = (headbone_pos.z - pos.z);

        float deltalength = sqrt((resultant_vector.x * resultant_vector.x) + (resultant_vector.y * resultant_vector.y) + (resultant_vector.z * resultant_vector.z));

        // calculate required angles
        float yaw = atan2(resultant_vector.y, resultant_vector.x) * (180 / pi);
        float pitch = -asin(resultant_vector.z / deltalength) * (180 / pi);

        // make sure that the angles are within the games set values
        if ((pitch < 89) && (pitch > -89) && (yaw < 180) && (yaw > -180)) {
            viewAngles->x = pitch;
            viewAngles->y = yaw;
        }


}

// checks if an entity is valid
bool ValidEnt(DWORD Entity, DWORD localplayer) {

    if (Entity == NULL || localplayer == NULL) {
        return false;
    }

    bool bdormant = *(bool*)(Entity + hazedumper::signatures::m_bDormant);
    if (bdormant) {
        return false;
    }

    if (Entity == localplayer) {
        return false;
    }

    if ((*(int*)(Entity + 0x100) <= 0)) {
        return false;
    }

    return true;
}

// take the 3D coordinates of a player and use the localplayers viewmatrix to find the 2D coordinates of that player on the localplayer screen (DID not code this btw its cooked lmao)
bool WorldToScreen(vec3 pos, vec2& screen, float matrix[16], int wWidth, int wHeight)
{

    vec4 clipCoords;
    clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
    clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
    clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
    clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

    if (clipCoords.w < 0.1f)
        return false;


    vec3 NDC;
    NDC.x = clipCoords.x / clipCoords.w;
    NDC.y = clipCoords.y / clipCoords.w;
    NDC.z = clipCoords.z / clipCoords.w;

    screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
    screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
    return true;
}



// find the closest target to the localplayers crosshair
DWORD BestTarget2(DWORD localplayer) {
    float oldDistance = FLT_MAX;
    float newDistance = 0;
    DWORD target = NULL;

    // loop the entity list 
    static vec3 viewAngles = *(vec3*)(*(DWORD*)(engine_module + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);
    for (int i = 0; i < 32; i++) {
        DWORD current_entity = *(DWORD*)((client_module + hazedumper::signatures::dwEntityList) + i * 0x10);


        // check if ent is valid
        if (current_entity == NULL || localplayer == NULL) {
            continue;
        }


        bool bdormant = *(bool*)(current_entity + hazedumper::signatures::m_bDormant);

        if (bdormant) {
            continue;
        }

        // check if Team_check is toggled
        if (toggles::Team_check) {
            if ((*(BYTE*)(current_entity + 0xf4)) == (*(BYTE*)(localplayer + 0xf4))) {
                continue;
            }
        }


        if (current_entity == localplayer) {
            continue;
        }

        if ((*(int*)(current_entity + 0x100) <= 0)) {
            continue;
        }

        // find postitions of both entity and localplayer
        vec3 src;
        vec3 dst;
        vec2 screen;
        fillposvecs(localplayer, current_entity, &src, &dst);

        // Get 2D screenspace coords of entity
        bool b = WorldToScreen(dst, screen, viewmatrix, windowWidth, windowHeight);
        if (!b) {
            continue;
        }

        // get entitys x position on screen and get distance from crosshair
        newDistance = Distance(windowWidth / 2, screen.x);
        if (newDistance < oldDistance)
        {
            oldDistance = newDistance;
            target = current_entity;

        }

    }

    // ensure that best distance is within the aimbots fov that has been set
    if (oldDistance > aimbot_fov) {
        aimbot_entity = NULL;
        return NULL;
    }

    return target;
}