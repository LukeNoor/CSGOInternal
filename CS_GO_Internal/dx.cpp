#include "Includes.h"

int windowHeight, windowWidth;



BOOL CALLBACK enumWind(HWND handle, LPARAM lp)
{
    DWORD processID;
    GetWindowThreadProcessId(handle, &processID);
    if (GetCurrentProcessId() != processID)
    {
        return TRUE;
    }
    else
    {
        window = handle;
        return FALSE;
    }
}

// find the window that corresponds to csgo.exe and the find width and size using GetWindowRect
HWND GetProcessWindow()
{
    window = NULL;
    EnumWindows(enumWind, NULL);
    RECT size;
    GetWindowRect(window, &size);
    windowWidth = size.right - size.left;
    windowHeight = size.bottom - size.top;

    
    

    return window;
}


bool GetD3D9Device(void** pTable, size_t size)
{
    if (!pTable) { return false; }

    // create D3D interface
    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!pD3D) { return false; }

    // create D3D device object
    IDirect3DDevice9* pDummyDevice = nullptr;

    // create D3D Present Parameters which contains information regarding graphics devicve
    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = false;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = GetProcessWindow();

    // create D3D device and store the result in dummyDeviceCreated;
    HRESULT dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

    // if dummy device failed then reattemp by toggling the Window state. if that fails again then just return false
    if (dummyDeviceCreated != S_OK)
    {
        d3dpp.Windowed = !d3dpp.Windowed;
        HRESULT dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);
        if (dummyDeviceCreated != S_OK)
        {
            pD3D->Release();
            return false;
        }
    }

    // hook the vtable of pDummyDevice so that we can draw our own graphics
    memcpy(pTable, *(void***)(pDummyDevice), size);

    // release the D3D device
    pDummyDevice->Release();
    pD3D->Release();
    return true;
}