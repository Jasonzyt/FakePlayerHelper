// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#if defined(BDS_LATEST)
#include <LLAPI.h>
#endif
using namespace FPHelper;


void initConsole() {
	//system("chcp 65001");
	//enable colorful output
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode;
	GetConsoleMode(hOutput, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; //NOLINT
	SetConsoleMode(hOutput, dwMode);
	//std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2);
}

namespace FPHelper {
	void entry();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		initConsole();
#if defined(BDS_LATEST)
			LL::registerPlugin("FakePlayerHelper", "A manager for FakePlayer", 
				LL::Version(VERSION.major, VERSION.minor, VERSION.patch, 
				(LL::Version::Status)VERSION.tag), 
				{
					{"Author", "JasonZYT"},
				    {"GitHub", "https://github.com/Jasonzyt/FakePlayerHelper"},
					{"License", "GNU-GPLv3.0"}
				}
			);
#endif
		break;
	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;
	case DLL_PROCESS_DETACH: break;
	}
	return TRUE;
}

extern "C" {
	__declspec(dllexport) void onPostInit() {
		FPHelper::entry();
	}
}

