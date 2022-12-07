#pragma once

typedef HRESULT(APIENTRY* tEndScene)(LPDIRECT3DDEVICE9 pDevice);

static HWND window;
extern int windowHeight, windowWidth;

HWND GetProcessWindow();

bool GetD3D9Device(void** pTable, size_t size);

extern LPDIRECT3DDEVICE9 pDevice;