// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "program.h"

bool load = false;

void Init() {
	// Get the name of the current game
	wchar_t exePath[MAX_PATH + 1];
	GetModuleFileNameW(NULL, exePath, MAX_PATH);

	wstring wstr(exePath);
	string currentGameName = basenameBackslashNoExt(string(wstr.begin(), wstr.end()));

	currentGame = getGame(currentGameName);

	std::thread thread = std::thread(InitializationThread);
	thread.detach();
}

void InitializationThread()
{
	CActionMotionManager** motionManagerAddress;
	CActionFighterManager** fighterManagerAddress;

	switch (currentGame)
	{
	default:
	{
		return;
	}
	case Game::Yakuza5:
	{
		motionManagerAddress = (CActionMotionManager**)resolve_relative_addr(PatternScan("48 8B 0D ? ? ? ? 4C 8D 4D ? C5 FA 11 45"), 7);
		fighterManagerAddress = (CActionFighterManager**)resolve_relative_addr(PatternScan("48 8B 0D ? ? ? ? E8 ? ? ? ? F7 80 ? ? ? ? ? ? ? ? 0F 84"), 7);
		MotionManager_LoadGMT = (MOTIONMANAGER_LoadGMTDirect)Memory::ReadCall2(PatternScan("E8 ? ? ? ? 48 8D 76 ? 48 83 ED ? 75 ? 48 8D 35"));
		FileMotionProperty_GetGMTID = (FILEMOTIONPROPERTY_GetGMTID)Memory::ReadCall2(PatternScan("E8 ? ? ? ? C5 F8 28 B4 24 ? ? ? ? 48 8B BC 24 ? ? ? ? 48 8B 9C 24"));
		break;
	}
	}

	if (motionManagerAddress == nullptr || fighterManagerAddress == nullptr)
		return;

	while (true)
	{
		motionManager = *motionManagerAddress;
		fighterManager = *fighterManagerAddress;

		if (motionManager != nullptr && fighterManager != nullptr)
			break;

		Sleep(100);
	}

	CollectResources();

	std::thread thread = std::thread(UpdateThread);
	thread.detach();
}

void CollectResources() 
{
	auto parless = GetModuleHandle(L"YakuzaParless.asi");

	if (parless == nullptr)
	{
		std::cout << "ResourceLoader: Couldn't get module handle to YakuzaParless.asi. Aborting\n";
		return;
	}

	auto YP_GET_NUM_MODS = (t_YP_GET_NUM_MODS)GetProcAddress(parless, "YP_GET_NUM_MODS");
	auto YP_GET_MOD_NAME = (t_YP_GET_MOD_NAME)GetProcAddress(parless, "YP_GET_MOD_NAME");

	unsigned int numMods = YP_GET_NUM_MODS();
	
	for (int i = 0; i < numMods; i++)
	{
		const char* modName = YP_GET_MOD_NAME(i);
		std::filesystem::path modDir = std::filesystem::path("mods");
		modDir += "/";
		modDir += modName;

		if (std::filesystem::is_directory(modDir))
		{
			std::filesystem::path resourcesLoadList = modDir;
			resourcesLoadList += "/player_spawn_load_resource_list.txt";

			if (std::filesystem::is_regular_file(resourcesLoadList))
			{
				std::ifstream file(resourcesLoadList);  // Replace with your file name
				std::string line;

				if (!file)
					continue;		

				while (std::getline(file, line)) {

					if (auto pos = line.find(".gmt"); pos != std::string::npos) {
						line.erase(pos, 4);
						gmtResourceList.push_back(line.c_str());
					}
				}

				file.close();
			}
		}
	}
}

void LoadThread() {
	//Really janky, but this ensures anything the game couldnt load gets loaded for sure
	//It used to be worse, it used to load every frame, yet everything was functional
	//So i'm experimenting with just adding 3 loading attempts instead
	Sleep(2000);
	LoadResources();
	Sleep(1500);
	LoadResources();
	Sleep(1500);
}

void UpdateThread() 
{
	while (true)
	{
		if (!playerFighterExistsDoOnce)
		{
			if (IsPlayerFighterPresent())
			{
				std::thread loadThread(LoadThread);
				loadThread.detach();
				playerFighterExistsDoOnce = true;
			}
		}
		else
		{
			if (!IsPlayerFighterPresent())
				playerFighterExistsDoOnce = false;
		}

		CFileMotionProperty* prop = GetPropertyClass();
	}
}

bool IsPlayerFighterPresent() 
{
	int playerIdx = -1;

	//Fighter index 0 is always the player.
	//If there are no fighters, its probably a minigame
	//But we dont care about loading resources at those moments at this point in time.
	switch (currentGame)
	{
	case Game::Yakuza5:
		playerIdx = *(int*)((__int64)fighterManager + 0x5E4);
		break;
	}

	return playerIdx > -1;
}

MotionManager* GetMotionManager()
{
	switch (currentGame)
	{
	case Game::Yakuza5:
	{
		MotionManager* manager = (MotionManager*)((__int64)motionManager + 0x1D0);
		return manager;
		break;
	}
	}

	return nullptr;
}

CFileMotionProperty* GetPropertyClass()
{
	switch (currentGame)
	{
	case Game::Yakuza5:
	{
		CFileMotionProperty** file = (CFileMotionProperty**)((__int64)motionManager + 0x4CF0);
		return *file;
		break;
	}
	}

	return nullptr;
}

void LoadResources() 
{
	MotionManager* motionManager = GetMotionManager();
	CFileMotionProperty* fileProperty = GetPropertyClass();
	
	int waitingGmt = 0;
	int chunkSize = 32;

	/*
	int total = 0xdeadbeef;

	while (true)
	{
		for (int i = 0; i < total; i += chunkSize) {
			int end = std::min(i + chunkSize, total); // don't go past 48!

			for (int j = i; j < end; ++j) {
				objects[j].doSomething();
			}

			std::cout << "--- Finished a chunk! ---\n";
		}
	}
	*/

	for (auto& element : gmtResourceList) {
		
		unsigned int gmtID = FileMotionProperty_GetGMTID(fileProperty, element.c_str());

		if (gmtID == 0)
			continue;

		MotionManager_LoadGMT(motionManager, gmtID, 16, 6);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Init();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
