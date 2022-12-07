#pragma once
#include <d3d9.h>

namespace toggles {

	extern bool ESP_toggle;
	extern bool Enemy_Box_Esp;
	extern bool TeamMate_Box_Esp;
	extern bool Enemy_snaplines;
	extern bool TeamMate_snaplines;
	extern bool Enemy_Health_ESP;
	extern bool TeamMate_Health_ESP;
	extern bool Aimbot_snaplines;

	
	// aimbot toggles and info
	extern bool aimbot_toggle;
	extern bool Team_check;

	


	// misc features
	extern bool Radar_toggle;
	extern bool triggerbot_toggle;
	extern bool Bhop_toggle;
	extern bool noflash_toggle;

	extern bool show_menu;
};

namespace ESP_Colors {

	extern D3DCOLOR Enemy_Box_Esp_C;
	extern D3DCOLOR Team_Box_Esp_C;
	extern D3DCOLOR Enemy_snaplines_C;
	extern D3DCOLOR Team_snaplines_C;
	extern D3DCOLOR Aimbot_snaplines_C;
};

extern uintptr_t Game_WNDPROC;
extern void* d3d9Device[119];
extern BYTE EndSceneBytes[];
extern tEndScene oEndScene;
extern const char* aimbot_items[];
extern float aimbot_fov;
extern int aimbot_key;
extern int aimbot_list_key;
extern int windowHeight;
extern int windowWidth;
extern float* viewmatrix;
extern bool init;
extern ImVec4 E_Box_ESP;
extern ImVec4 T_Box_ESP;
extern ImVec4 E_snap_ESP;
extern ImVec4 T_snap_ESP;
extern ImVec4 A_Snap_ESP;
extern DWORD aimbot_entity;
extern DWORD client_module;
extern DWORD engine_module;

struct vec4
{
	float x, y, z, w;
};

struct vec2 {
	float x, y;
};



struct vec3 {
	float x, y, z;

	vec3() {
		x = 0;
		y = 0;
		z = 0;
	}

	vec3(float a, float b, float c) {
		x = a;
		y = b;
		z = c;
	}

	vec3 add2vectors(vec3 vec) {
		this->x += vec.x;
		this->y += vec.y;
		this->z += vec.z;

		return *this;
	}
};

void radar(DWORD entity);
void bhop(DWORD entity);
void noflash(DWORD entity);
void triggerbot(DWORD localplayer);
void aimbot(DWORD entity, DWORD localplayer);
vec3 Subtract(vec3 src, vec3 dst);
bool WorldToScreen(vec3 pos, vec2& screen, float matrix[16], int wWidth, int wHeight);
DWORD BestTarget2(DWORD localplayer);
float Magnitude(vec3 vec);
float Distance(float src, float dst);
vec3 CalcAngle(vec3 src, vec3 dst);
bool ValidEnt(DWORD Entity, DWORD localplayer);
void fillposvecs(DWORD localplayer, DWORD ent, vec3* src, vec3* dst);