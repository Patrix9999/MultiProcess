#pragma once

#include <windows.h> // WinAPI functions
#include <cstring> // strlen

inline const BYTE* FindPattern(const BYTE* base, const size_t size,
                               const BYTE* pattern, const char* mask)
{
    for (size_t i = 0, patternLen = strlen(mask); i <= size - patternLen; ++i)
    {
        bool found = true;
        for (size_t j = 0; j < patternLen; ++j)
        {
            if (mask[j] == 'x' && pattern[j] != base[i + j])
            {
                found = false;
                break;
            }
        }
        if (found)
            return base + i;
    }
    return nullptr;
}

inline const BYTE* FindPatternInModule(const HMODULE hModule, const char* pattern, const char* mask)
{
    if (!hModule)
        return nullptr;

    // DOS header validation
    const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) // "MZ"
        return nullptr;

    // NT header validation
    const auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
        reinterpret_cast<BYTE*>(hModule) + dosHeader->e_lfanew
    );
    if (ntHeader->Signature != IMAGE_NT_SIGNATURE) // "PE\0\0"
        return nullptr;

    const BYTE* base = reinterpret_cast<BYTE*>(hModule) + ntHeader->OptionalHeader.BaseOfCode;
    const size_t size = ntHeader->OptionalHeader.SizeOfCode;

    return FindPattern(base, size, reinterpret_cast<const BYTE*>(pattern), mask);
}