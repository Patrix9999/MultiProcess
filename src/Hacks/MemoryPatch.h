#pragma once

#include <cstring>
#include <vector>
#include <functional>
#include <windows.h>

#include "UNTYPED.h"

class MemoryPatch
{
protected:
	void Protect(const untyped target, DWORD size, DWORD &protection)
	{
		VirtualProtect(target, size, protection, &protection);
		protection = 0;
	}

	void Unprotect(const untyped target, DWORD size, DWORD &protection)
	{
		VirtualProtect(target, size, PAGE_EXECUTE_READWRITE, &protection);
	}

	template <class T>
	void Apply(const untyped target, T data, size_t size)
	{
		DWORD protection = 0;

		Unprotect(target, size, protection);
		memcpy(target, (void*)data, size);
		Protect(target, size, protection);

		// flushing CPU cache
		FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
	}

public:
	MemoryPatch(const untyped target, std::vector<BYTE> bytes, std::function<bool()> conditionCallback = nullptr)
	{
		if (!conditionCallback || (conditionCallback && conditionCallback()))
			Apply(target, &bytes[0], bytes.size());
	};

	template <typename T>
	MemoryPatch(const untyped target, T data, std::function<bool()> conditionCallback = nullptr)
	{
		if (!conditionCallback || (conditionCallback && conditionCallback()))
			Apply(target, &data, sizeof(data));
	};
};