#pragma once
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <Windows.h>

inline void* resolve_relative_addr(void* instruction_start, int instruction_length = 7, bool weird_workaround = false)
{
    void* instruction_end = (void*)((unsigned long long)instruction_start + instruction_length);
    unsigned int* offset = (unsigned int*)((unsigned long long)instruction_start + (instruction_length - 4));

    void* addr = (void*)(((unsigned long long)instruction_start + instruction_length) + *offset);

    //Dunno why this happened on LJ. Was fine on Y6-YK2-YLAD
    //I'm too lazy to make an actual fix :sunglasses:
    if (weird_workaround)
        addr = (void*)((unsigned long long)addr - 0x100000000);

    return addr;
}

inline std::uint8_t* PatternScan(void* module, const char* signature)
{
    static auto pattern_to_byte = [](const char* pattern) {
        auto bytes = std::vector<int>{};
        auto start = const_cast<char*>(pattern);
        auto end = const_cast<char*>(pattern) + strlen(pattern);

        for (auto current = start; current < end; ++current) {
            if (*current == '?') {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else {
                bytes.push_back(strtoul(current, &current, 16));
            }
        }
        return bytes;
        };

    auto dosHeader = (PIMAGE_DOS_HEADER)module;
    auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

    auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto patternBytes = pattern_to_byte(signature);
    auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

    auto s = patternBytes.size();
    auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i) {
        bool found = true;
        for (auto j = 0ul; j < s; ++j) {
            if (scanBytes[i + j] != d[j] && d[j] != -1) {
                found = false;
                break;
            }
        }
        if (found) {
            return &scanBytes[i];
        }
    }
    return nullptr;
}

inline std::uint8_t* PatternScan(const char* signature)
{
    void* module = GetModuleHandle(NULL);
    return PatternScan(module, signature);
}