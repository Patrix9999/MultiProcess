namespace GOTHIC_NAMESPACE
{
	// Set gameAbnormalExit to `1` only during unhandled exception (Access Violation)
	static LONG WINAPI zCExceptionHandler__UnhandledExceptionFilter(PEXCEPTION_POINTERS pExPtrs);
	auto Ivk_zCExceptionHandler__UnhandledExceptionFilter = Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x004BF560, 0x004CEC70, 0x004C6350, 0x004C88C0)), &zCExceptionHandler__UnhandledExceptionFilter, Union::HookType::Hook_Detours);
	static LONG WINAPI zCExceptionHandler__UnhandledExceptionFilter(PEXCEPTION_POINTERS pExPtrs)
	{
		Ivk_zCExceptionHandler__UnhandledExceptionFilter.Disable();

		zSTRING iniFile = zoptions->ParmValue("ini");
		if (iniFile.IsEmpty())
			iniFile = "Gothic.ini";

		zoptions->WriteBool("INTERNAL", "gameAbnormalExit", TRUE, FALSE);
		zoptions->Save(iniFile);

		return Ivk_zCExceptionHandler__UnhandledExceptionFilter(pExPtrs);
	}

	// Remove WS_SYSMENU flag while setting a game window style
	static LONG __stdcall IAT_SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);
	auto Ivk_SetWindowLongA = CreateIATHook(nullptr, "user32.dll", "SetWindowLongA", &IAT_SetWindowLongA);
	static LONG __stdcall IAT_SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong)
	{
		if (hWnd == hWndApp && nIndex == GWL_STYLE)
			dwNewLong &= ~WS_SYSMENU;

		return Ivk_SetWindowLongA(hWnd, nIndex, dwNewLong);
	}

	// Remove WS_SYSMENU flag while creating a game window
	static HWND __stdcall IAT_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
	auto Ivk_CreateWindowExA = CreateIATHook(nullptr, "user32.dll", "CreateWindowExA", &IAT_CreateWindowExA);
	static HWND __stdcall IAT_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
		if (strcmp(lpClassName, "DDWndClass") == 0)
			dwStyle &= ~WS_SYSMENU;

		return Ivk_CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	}

	// Apply IAT hooks, this must be done here, because when dll gets injected, the hInstApp is set to nullptr, so we can't construct a valid hook
	static int WINAPI Hook_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
	auto Ivk_WinMain = CreateHook(reinterpret_cast<void*>(zSwitch(0x004F3E10, 0x00506810, 0x005000F0, 0x00502D70)), &Hook_WinMain);
	static int WINAPI Hook_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
	{
		Ivk_WinMain.Detach();

		Ivk_SetWindowLongA.Attach(hInstance);
		Ivk_CreateWindowExA.Attach(hInstance);

		return Ivk_WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
	}

	static LRESULT CALLBACK Hook_AppWndProc(HWND hwnd, DWORD msg, WPARAM wParam, LPARAM lParam);
	auto AppWndProc_Original = (LRESULT(CALLBACK*)(HWND, zUWORD, WPARAM, LPARAM))zSwitch(0x004F4EE0, 0x005078E0, 0x00500A80, 0x00503770);
	auto Ivk_AppWndProc = Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x004F4EE0, 0x005078E0, 0x00500A80, 0x00503770)), &Hook_AppWndProc, Union::HookType::Hook_Detours);
	static LRESULT CALLBACK Hook_AppWndProc(HWND hwnd, DWORD msg, WPARAM wParam, LPARAM lParam)
	{
		Ivk_AppWndProc.Disable();

		LRESULT result = AppWndProc_Original(hwnd, msg, wParam, lParam);

		// Handle closing game by right-clicking on the game on task bar and choosing `close window` option
		if (msg == WM_SYSCOMMAND && wParam == SC_CLOSE)
			zerr->onexit();

		Ivk_AppWndProc.Enable();

		return result;
	}
}
