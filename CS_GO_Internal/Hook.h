#pragma once


bool Hook(BYTE* src, BYTE* dst, int len);

BYTE* TrampHook(BYTE* src, BYTE* dst, int len, bool b);


void Patch(BYTE* src, BYTE* dst, int len );