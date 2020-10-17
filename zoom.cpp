#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <tchar.h> 
#include <vector>

DWORD dwGetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID)
{
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32))
    {
        do
        {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));

    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

int main()
{
    
    //"UI"
    std::cout << "Numpad + zoom in" << std::endl;
    std::cout << "Numpad - zoom out" << std::endl;
    std::cout << "Numpad 0 reset" << std::endl;
    std::cout << "Numpad 1 restore" << std::endl;

    HWND hGameWindow;
    DWORD pID;
    HANDLE processHandle = NULL;
    char moduleName[] = "UnityPlayer.dll";
    DWORD baseAddress;
    DWORD offsetGameToBaseAddress = 0x012A7B34;
    float zoomValue = 0;
    float zoomValuereset = 0;
    float control = 0;
    float controlres = 1;
    float light =50;

    while (true)
    {
      
        hGameWindow = FindWindow(NULL, "Among Us");
        GetWindowThreadProcessId(hGameWindow, &pID);
        processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
      
        DWORD gameBaseAddress = dwGetModuleBaseAddress(_T(moduleName), pID);

        std::vector<DWORD> pointsOffsets{0xC, 0x34, 0x20, 0x10,  0x18,};
        ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);
        //zoom offset
        DWORD pointsAddress = baseAddress;
        for (int i = 0; i < pointsOffsets.size() - 1; i++)
        ReadProcessMemory(processHandle, (LPVOID)(pointsAddress + pointsOffsets.at(i)), &pointsAddress, sizeof(pointsAddress), NULL);
        pointsAddress += pointsOffsets.at(pointsOffsets.size() - 1);


        //optain cam controll offset
        DWORD pointsAddress2 = baseAddress;
        std::vector<DWORD> pointsOffsets2{0xC, 0x34, 0x20, 0x10,  0x1C,};
        for (int i = 0; i < pointsOffsets2.size() - 1; i++)
        ReadProcessMemory(processHandle, (LPVOID)(pointsAddress2 + pointsOffsets2.at(i)), &pointsAddress2, sizeof(pointsAddress2), NULL);
        pointsAddress2 += pointsOffsets2.at(pointsOffsets2.size() - 1);


        //light offset 
        DWORD pointsAddress3 = baseAddress;
        std::vector<DWORD> pointsOffsets3{0xC, 0x34, 0x20, 0x10, 0x8,};
        for (int i = 0; i < pointsOffsets3.size() - 1; i++)
        ReadProcessMemory(processHandle, (LPVOID)(pointsAddress3 + pointsOffsets3.at(i)), &pointsAddress3, sizeof(pointsAddress3), NULL);
        pointsAddress3 += pointsOffsets3.at(pointsOffsets3.size() - 1);

      
        
        //remove the shadow aka light offset 
        WriteProcessMemory(processHandle, (LPVOID)(pointsAddress3), &light, sizeof(float), 0);
        
        if (GetAsyncKeyState(VK_NUMPAD0)) //restart
        {
            WriteProcessMemory(processHandle, (LPVOID)(pointsAddress2), &controlres, sizeof(float), 0);
            WriteProcessMemory(processHandle, (LPVOID)(pointsAddress), &zoomValuereset, sizeof(float), 0);
        }
        if (GetAsyncKeyState(VK_NUMPAD1)) // restore-
        {
            WriteProcessMemory(processHandle, (LPVOID)(pointsAddress2), &control, sizeof(float), 0); // optain cam controll   
            WriteProcessMemory(processHandle, (LPVOID)(pointsAddress), &zoomValue, sizeof(float), 0);
        }
        if (GetAsyncKeyState(VK_ADD)) //numpad +
        {
            WriteProcessMemory(processHandle, (LPVOID)(pointsAddress2), &control, sizeof(float), 0); // optain cam controll
            ReadProcessMemory(processHandle, (LPCVOID)(pointsAddress), &zoomValue, sizeof (float), NULL);
            zoomValue +=0.01;
            WriteProcessMemory(processHandle, (LPVOID)(pointsAddress), &zoomValue, sizeof(float), 0);
        }
        if (GetAsyncKeyState(VK_SUBTRACT)) // numpad -
        {
            WriteProcessMemory(processHandle, (LPVOID)(pointsAddress2), &control, sizeof(float), 0); // optain cam controll
            ReadProcessMemory(processHandle, (LPCVOID)(pointsAddress), &zoomValue, sizeof (float), NULL);
            zoomValue -=0.01;
            WriteProcessMemory(processHandle, (LPVOID)(pointsAddress), &zoomValue, sizeof(float), 0);
        }
        Sleep(50);

    }

}