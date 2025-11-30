#pragma once

#include <cstring> // memcpy, std::size_t
#include <windows.h> // WinAPI functions

#include "untyped.h"
#include "InvokeType.h"

template <typename T>
class IATHook
{
private:
	HMODULE module;
	const char* target_dll = "";
	const char* func_name = "";
	void* detour;

	DWORD backup = 0;
	DWORD protection = 0;

	inline DWORD VirtualAddress(DWORD offset)
	{
		return reinterpret_cast<DWORD>(module) + offset;
	}

	inline PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor()
	{
		PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
		if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			return nullptr;

		PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(VirtualAddress(dosHeader->e_lfanew));
		if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
			return nullptr;

		PIMAGE_IMPORT_DESCRIPTOR importDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(VirtualAddress(ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));
		for (int i = 0; importDescriptor[i].Characteristics != 0; ++i)
		{
			LPCSTR current_dll = reinterpret_cast<LPCSTR>(VirtualAddress(importDescriptor[i].Name));
			if (!importDescriptor[i].FirstThunk || _stricmp(current_dll, target_dll) != 0)
				continue;

			return &importDescriptor[i];
		}

		return nullptr;
	}

	PIMAGE_THUNK_DATA FindIATRecord()
	{
		PIMAGE_IMPORT_DESCRIPTOR importDescriptor = GetImportDescriptor();
		if (importDescriptor == nullptr)
			return nullptr;

		PIMAGE_THUNK_DATA importNameTableEntry = reinterpret_cast<PIMAGE_THUNK_DATA>(VirtualAddress(importDescriptor->OriginalFirstThunk));
		PIMAGE_THUNK_DATA importAddressTableEntry = reinterpret_cast<PIMAGE_THUNK_DATA>(VirtualAddress(importDescriptor->FirstThunk));

		while (importNameTableEntry->u1.AddressOfData != NULL)
		{
			PIMAGE_IMPORT_BY_NAME importedFunction = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(VirtualAddress(importNameTableEntry->u1.AddressOfData));
			if (strcmp(importedFunction->Name, func_name) == 0)
				return importAddressTableEntry;

			++importNameTableEntry;
			++importAddressTableEntry;
		}

		return nullptr;
	}

public:
	IATHook(const HMODULE module, const char* target_dll, const char* func_name, const T detour, const InvokeType invokeType = InvokeType::kNormal)
	{
		this->module = module ? module : GetModuleHandle(module);
		this->target_dll = target_dll;
		this->func_name = func_name;
		this->detour = detour;

		if (module != nullptr && invokeType == InvokeType::kNormal)
			Attach();
	}

	~IATHook()
	{
		Detach();
	}

	bool Attach(HMODULE module = nullptr)
	{
		if (module != nullptr)
			this->module = module;

		if (IsAttached())
			return false;

		// finding IAT record
		PIMAGE_THUNK_DATA record = FindIATRecord();
		if (!record)
			return false;

		// patching memory
		if (!Unprotect(record))
			return false;

		backup = record->u1.Function;
		record->u1.Function = reinterpret_cast<DWORD>(detour);

		if (!Protect(record))
			return false;

		// flushing CPU cache
		FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
		return true;
	}

	bool Detach()
	{
		if (!IsAttached())
			return false;

		// finding IAT record
		PIMAGE_THUNK_DATA record = FindIATRecord();
		if (!record)
			return false;

		// patching memory
		if (!Unprotect(record))
			return false;

		record->u1.Function = backup;
		backup = 0;

		if (!Protect(record))
			return false;

		return true;
	}

	bool IsAttached()
	{
		return backup != 0;
	}

	bool Protect(void* address)
	{
		if (!VirtualProtect(address, sizeof(detour), protection, &protection))
			return false;

		protection = 0;
		return true;
	}

	bool Unprotect(void* address)
	{
		if (!VirtualProtect(address, sizeof(detour), PAGE_READWRITE, &protection))
			return false;

		return true;
	}

	bool IsProtected()
	{
		return protection != 0;
	}

	operator T()
	{
		return *(T*)(&backup);
	}
};

template <typename T>
static inline IATHook<T> CreateIATHook(const HMODULE module, const char* target_dll, const char* func_name, const T detour, const InvokeType invokeType = InvokeType::kNormal)
{
	return IATHook<T>(module, target_dll, func_name, detour, invokeType);
}