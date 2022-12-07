#pragma once

extern WNDPROC oWndProc;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void InitImGui(IDirect3DDevice9* pDevice);

void DrawMenu(LPDIRECT3DDEVICE9 o_pDevice);

void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color);

void ESP();

void menuSpacing();

D3DCOLOR FLOAT4TOD3DCOLOR(float Col[]);