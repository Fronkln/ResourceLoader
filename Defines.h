#pragma once

class Fighter;

class CActionFighterManager;
class CActionMotionManager;
class CFileMotionProperty;

class MotionManager; //not to be confused with its action version

//Game functions
typedef void* (__fastcall* MOTIONMANAGER_LoadGMTDirect)(MotionManager* motMan, unsigned int gmtID, int unknown, int heapCategory);
typedef unsigned int(__fastcall* FILEMOTIONPROPERTY_GetGMTID)(CFileMotionProperty* fileMotProperty, const char* gmtName);

//YakuzaParless functions
typedef unsigned int (*t_YP_GET_NUM_MODS)();
typedef const char* (*t_YP_GET_MOD_NAME)(unsigned int idx);