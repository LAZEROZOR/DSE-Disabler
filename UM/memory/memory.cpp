#include "memory.h"
#include <psapi.h>
#include <iostream>

LPVOID GetCiDllBaseAddress()
{
    LPVOID drivers[1024];
    DWORD cbNeeded = 0;

    if (!EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded)) return nullptr;

    int count = cbNeeded / sizeof(LPVOID);
    wchar_t name[MAX_PATH];

    for (int i = 0; i < count; ++i) 
    {
        if (GetDeviceDriverBaseNameW(drivers[i], name, _countof(name))) 
        {
            if (_wcsicmp(name, L"CI.dll") == 0) 
            {
				return drivers[i];
            }
        }
    }

	return nullptr;
}