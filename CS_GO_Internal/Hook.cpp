#include "Includes.h"

// patch a desired memory address (dst) with content from a memory address (src) you specify the size to patch
void Patch(BYTE* dst, BYTE* src, int size) {
    DWORD oProc;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oProc);
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oProc, &oProc);
}


// create a hook in a memory location (tohook) the hook will remove len Bytes at that location and place an unconditional JMP to the memory location (myhook)
bool hook(BYTE* tohook, void* myhook, int len) {
    if (len < 5) {
        return false;
    }

    DWORD curProtection;
    VirtualProtect(tohook, len, PAGE_EXECUTE_READWRITE, &curProtection);

    memset(tohook, 0x90, len);

    *tohook = 0xE9;

    DWORD relitiveaddy = (BYTE*)myhook - tohook - 5;

    *(DWORD*)(tohook + 1) = relitiveaddy;

    VirtualProtect(tohook, len, curProtection, &curProtection);
    return true;
}

// works the same as the above. however you allocate a gateway which stores all the bytes that you overwrote. when you finish with your function call.
// return to the gateway and execute the stolen bytes inorder to not corrupt the stack.

BYTE* TrampHook(BYTE* tohook, BYTE* myhook, int len, bool overwrite_bytes) {

    // create gateway
    BYTE* gateway = (BYTE*)VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    // if you want to execute the overwritten bytes
    if (overwrite_bytes) {
        intptr_t relitiveaddy = (tohook - gateway) - 5;


        memcpy(gateway, tohook, len);

        *(gateway + len) = 0xE9;

        *(intptr_t*)(gateway + len + 1) = relitiveaddy;

    }
    else { // ignore overwritten bytes as user has made sure that they wont corrupt the stack

        intptr_t relitiveaddy = (tohook - gateway);

        *gateway = 0xE9;

        *(intptr_t*)(gateway + 1) = relitiveaddy;

    }

    // call detour
    hook(tohook, myhook, len);


    // return gateway
    return gateway;

}