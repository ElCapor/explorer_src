#ifndef UTIL_H
#define UTIL_H

//blabla includes
#include <Windows.h>
#include <vector>
#include <TlHelp32.h> // -> for making snapshots
#include <Psapi.h> // -> manipulation processes
#include "globals.h"
#include <algorithm>
#include <string>
std::vector<DWORD> get_valid_processes() 
{
    // roblox has 2 processes (1 normal and 1 anticheat), we make sure to have the correct one by checking it's size
    std::vector<DWORD> return_value{};

    HANDLE h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (h_snapshot == INVALID_HANDLE_VALUE)
    {
        std::printf("Failed to create snapshot!\n");
        return return_value;
    }

    PROCESSENTRY32 procentry32{};
    procentry32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(h_snapshot, &procentry32))
    {
        std::printf("Failed to run Process32First!\n");
        CloseHandle(h_snapshot);
        return return_value;
    }

    do
    {
        if (!strcmp(procentry32.szExeFile, "RobloxPlayerBeta.exe"))
        {
            HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, false, procentry32.th32ProcessID);

            PROCESS_MEMORY_COUNTERS PMC{};
            GetProcessMemoryInfo(h, &PMC, sizeof(PROCESS_MEMORY_COUNTERS));

            CloseHandle(h);

            if (PMC.PeakWorkingSetSize >= 100000000) // correct process
            {
                return_value.push_back(procentry32.th32ProcessID);
            }
        }
    } while (Process32Next(h_snapshot, &procentry32));

    CloseHandle(h_snapshot);

    return return_value;
}

std::uintptr_t get_base_address(HANDLE handle) // this is how to get the base alloc address
// the code is pretty similar to the one above and with some refactoring we could combine both
{
    HANDLE h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(handle));
    if (!h_snapshot)
    {
        std::printf("Failed to create snapshot!\n");
        return 0;
    }

    MODULEENTRY32 current_module{};
    current_module.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(h_snapshot, &current_module))
    {
        CloseHandle(h_snapshot);
        std::printf("Failed to get module!\n");
        return 0;
    }

    do
    {
        if (!strcmp(current_module.szModule, "RobloxPlayerBeta.exe"))
        {
            CloseHandle(h_snapshot);
            return reinterpret_cast<std::uintptr_t>(current_module.modBaseAddr);
        }
    } while (Module32Next(h_snapshot, &current_module));

    std::printf("failed to find module!\n");
    CloseHandle(h_snapshot);
    return 0;
}

template <typename T>
T Read(DWORD addr)
{
    T buf;
    ReadProcessMemory(globals::h, reinterpret_cast<void*>(addr),&buf, sizeof(T), NULL);
    return buf;
}

template <typename T>
T Write(DWORD addr, T buf)
{
    WriteProcessMemory(globals::h, reinterpret_cast<void*>(addr), &buf, sizeof(buf), NULL);
    return buf;
}


std::string ReadNormalString(DWORD addr);

std::string ReadStringOfUnknownLength(std::uintptr_t address) {
    std::string string;
    char character = 0;
    int charSize = sizeof(character);
    int offset = 0;
    bool isvalid;
    string.reserve(52);

    while (offset < 200) { // 200 / sizeof(char) == 50. This means 50 charactes can be read until it will stop reading.
        character = Read<char>(address + offset);
        if (character == 0) break;
        offset += charSize;
        string.push_back(character);
    }
    return string;
}

std::string ReadNormalString(DWORD addr)
{
    int string_size = Read<int>(addr + 0x10);
    if (string_size < 16u)
    {
        return ReadStringOfUnknownLength(addr);
    }
    else {
        return ReadStringOfUnknownLength(Read<DWORD>(addr));
    }
}



std::string readClassName(DWORD Instance)
{
    DWORD classDescriptor = Read<DWORD>(Instance + instance::class_descriptor);
    return ReadStringOfUnknownLength(Read<DWORD>(classDescriptor + instance::class_name));
}
std::vector<DWORD> GetInstanceTable(DWORD InstanceAddr) { // made in 10 minutes
    std::vector<DWORD> instanceTable;

    if (!InstanceAddr || !Read<DWORD>(InstanceAddr))
        return instanceTable;

    DWORD next_addr = Read<DWORD>(InstanceAddr + 4);
    DWORD addr = Read<DWORD>(InstanceAddr);  // basically we know we are at the end we have 2 empty adresses

    while (addr != next_addr) { // means we have 2 0's in a row
        instanceTable.push_back(Read<DWORD>(addr));
        addr += 8; //igonre ref stuff
    }

    return instanceTable;
}
#endif // !UTIL_H
