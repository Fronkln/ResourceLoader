#pragma once
#include "Maps.h"
#include "Defines.h"
#include "PatternScan.h"
#include "MemoryMgr.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <filesystem>

//Resources to load
std::vector<std::string> gmtResourceList;

//Program variables
Game currentGame;
bool playerFighterExistsDoOnce;

//Important game classes
CActionMotionManager* motionManager;
CActionFighterManager* fighterManager;

//Application
void InitializationThread();
void UpdateThread();
bool IsPlayerFighterPresent();
void CollectResources();

//GMT Loading
MotionManager* GetMotionManager();
CFileMotionProperty* GetPropertyClass();
FILEMOTIONPROPERTY_GetGMTID FileMotionProperty_GetGMTID;
MOTIONMANAGER_LoadGMTDirect MotionManager_LoadGMT;
void LoadResources();