#include "source.hpp"
#include "visitor.hpp"
#include "hooge.hpp"
#include "icons.hpp"
//#include "threading/threading.h"
//#include "threading/shared_mutex.h"
#include "sdk.hpp"

//#include <thread>
#define _ITERATOR_DEBUG_LEVEL 0

DWORD installed;

class init_font
{
public:
	init_font(void* font, uint32_t length)
	{
		if (handle = AddFontMemResourceEx(font, length, nullptr, &installed); handle == nullptr)
			return;

		VirtualProtect(font, length, PAGE_READWRITE, 0);
		memset(font, 0, length);
	}

private:
	HANDLE handle;
};

//static Semaphore dispatchSem;
//static SharedMutex smtx;
//
//using ThreadIDFn = int(_cdecl*)();
//
//ThreadIDFn AllocateThreadID;
//ThreadIDFn FreeThreadID;
//
//int AllocateThreadIDWrapper() {
//	return AllocateThreadID();
//}
//
//int FreeThreadIDWrapper() {
//	return FreeThreadID();
//}
//
//template<typename T, T& Fn>
//static void AllThreadsStub(void*) {
//	dispatchSem.Post();
//	smtx.rlock();
//	smtx.runlock();
//	Fn();
//}
//
////TODO: Build this into the threading library
//template<typename T, T& Fn>
//static void DispatchToAllThreads(void* data, bool t = false) {
//	smtx.wlock();
//
//	for (size_t i = 0; i < Threading::numThreads; i++)
//		Threading::QueueJobRef(AllThreadsStub<T, Fn>, data);
//
//	if (t) {
//		for (size_t i = 0; i < Threading::numThreads; i++)
//			dispatchSem.Wait();
//	}
//
//	smtx.wunlock();
//
//	Threading::FinishQueue(false);
//}

#ifdef AUTH
//void Entry();
//void* initptr = &Entry;

void Entry()
{
	init_font(static_cast<void*>(visitor), sizeof(visitor));
	init_font(static_cast<void*>(hooge), sizeof(hooge));
	init_font(static_cast<void*>(icons_font), sizeof(icons_font));

	while (!GetModuleHandleA(sxor("serverbrowser.dll")))
		Sleep(200);

	//Beep(550, 200);

	//auto tier0 = GetModuleHandleA("tier0.dll");

	//AllocateThreadID = (ThreadIDFn)GetProcAddress(tier0, "AllocateThreadID");
	//FreeThreadID = (ThreadIDFn)GetProcAddress(tier0, "FreeThreadID");

	//Threading::InitThreads();

	//DispatchToAllThreads<decltype(AllocateThreadIDWrapper), AllocateThreadIDWrapper>(nullptr, true);

	Source::Create();

	//{
	//	while (!GetAsyncKeyState(VK_F11))
	//		Sleep(200);

	//	Source::Destroy();

	//	Sleep(1000);
	//}

	//initptr = 0;
	/*erase_end;*/
}
#else
FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;

void Entry(HMODULE hModule)
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen_s(&fpstdin, "CONIN$", "r", stdin);
	freopen_s(&fpstdout, "CONOUT$", "w", stdout);
	freopen_s(&fpstderr, "CONOUT$", "w", stderr);
	SetConsoleTitleA("F"); //press F to pay respects to Microsoft

	init_font(static_cast<void*>(visitor), sizeof(visitor));
	init_font(static_cast<void*>(hooge), sizeof(hooge));
	init_font(static_cast<void*>(icons_font), sizeof(icons_font));

	while (!GetModuleHandleA(sxor("serverbrowser.dll")))
		Sleep(200);

	/*auto tier0 = GetModuleHandleA("tier0.dll");

	AllocateThreadID = (ThreadIDFn)GetProcAddress(tier0, "AllocateThreadID");
	FreeThreadID = (ThreadIDFn)GetProcAddress(tier0, "FreeThreadID");

	Threading::InitThreads();

	DispatchToAllThreads<decltype(AllocateThreadIDWrapper), AllocateThreadIDWrapper>(nullptr, true);*/

	if (Source::Create())
	{
		while (!GetAsyncKeyState(VK_F11)) {
			//Source::QueueJobs();
			//Threading::FinishQueue(true);
			Sleep(200);
		}

		/*DispatchToAllThreads<decltype(FreeThreadIDWrapper), FreeThreadIDWrapper>(nullptr);
		Threading::EndThreads();*/

		Source::Destroy();

		
		Sleep(1000);
	}

	fclose(stdin); fclose(stdout); fclose(stderr); // else console won't close
	FreeConsole();
	FreeLibraryAndExitThread(hModule, EXIT_SUCCESS);
}

#endif // AUTH

//extern "C"
//{
//	__declspec(dllexport)
//	s_interface m_interfaces[] =
//	{
//	  { sxor("client.dll"), sxor("VClient018"), &csgo.m_client.m_interface },
//	  { sxor("engine.dll"), sxor("VEngineClient014"), &csgo.m_engine.m_interface },
//	  { sxor("client.dll"), sxor("VClientEntityList003"), &csgo.m_entity_list.m_interface },
//	  { sxor("client.dll"), sxor("GameMovement001"), &csgo.m_movement.m_interface },
//	  { sxor("client.dll"), sxor("VClientPrediction001"), &csgo.m_prediction.m_interface },
//	  { sxor("vgui2.dll"), sxor("VGUI_Panel009"), &csgo.m_panel.m_interface },
//	  { sxor("vguimatsurface.dll"), sxor("VGUI_Surface031"), &csgo.m_surface.m_interface },
//	  { sxor("engine.dll"), sxor("VEngineVGui001"), &csgo.m_engine_vgui.m_interface },
//	  { sxor("vstdlib.dll"), sxor("VEngineCvar007"), &csgo.m_engine_cvars.m_interface },
//	  { sxor("engine.dll"), sxor("EngineTraceClient004"), &csgo.m_engine_trace.m_interface },
//	  { sxor("engine.dll"), sxor("VModelInfoClient004"), &csgo.m_model_info.m_interface },
//	  { sxor("inputsystem.dll"), sxor("InputSystemVersion001"), &csgo.m_input_system.m_interface },
//	  { sxor("engine.dll"), sxor("VEngineModel016"), &csgo.m_model_render.m_interface },
//	  { sxor("engine.dll"), sxor("VEngineRenderView014"), &csgo.m_render_view.m_interface },
//	  { sxor("materialsystem.dll"), sxor("VMaterialSystem080"), &csgo.m_material_system.m_interface },
//	  { sxor("vphysics.dll"), sxor("VPhysicsSurfaceProps001"), &csgo.m_phys_props.m_interface },
//	  { sxor("engine.dll"), sxor("VDebugOverlay004"), &csgo.m_debug_overlay.m_interface },
//	  { sxor("engine.dll"), sxor("GAMEEVENTSMANAGER002"), &csgo.m_event_manager.m_interface },
//	  { sxor("engine.dll"), sxor("StaticPropMgrServer002"), &csgo.m_static_prop.m_interface },
//	  { sxor("localize.dll"), sxor("Localize_001"), &csgo.m_localize.m_interface },
//	  { sxor("datacache.dll"), sxor("MDLCache004"), &csgo.m_mdl_cache.m_interface },
//	  { nullptr,               nullptr,            nullptr }
//	};
//}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	if( dwReason == DLL_PROCESS_ATTACH )
	{

#ifndef AUTH
		CreateThread(nullptr, 0u, (LPTHREAD_START_ROUTINE)Entry, hModule, 0u, nullptr);
#else
		ctx.data = (_MANUAL_INJECTEX32*)lpReserved;

		if (ctx.data != nullptr) {
			//Beep(400, 200);
			Entry();//CreateThread(nullptr, 0u, (LPTHREAD_START_ROUTINE)initptr, 0, 0u, nullptr);
		}
#endif



		DisableThreadLibraryCalls(hModule);
	}
	/*else if (dwReason == DLL_PROCESS_DETACH)
	{
		RemoveVectoredExceptionHandler(exception_handle);
		Source::Destroy();
	}*/

	return TRUE;
}