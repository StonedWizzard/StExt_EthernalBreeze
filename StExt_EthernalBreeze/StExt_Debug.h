#pragma once

namespace Gothic_II_Addon
{
	//-----------------------------------------------------------------
	//							DEBUG
	//-----------------------------------------------------------------
	class zSTRING;
	class oCNpc;

	#define DebugEnabled true
	#define DebugStackEnabled false

	#if DebugEnabled
		#define DEBUG_MSG(message) DebugMessage(message)
		#define DEBUG_MSG_FUNC(funcName, message) DebugFuncMessage(funcName, message)
		#define DEBUG_MSG_DAM(funcName, message, atk, target) DebugDamageMessage(funcName, message, atk, target)
		#define DEBUG_MSG_IF(condition, message) do { if (condition) { DebugMessage(message); } } while(0)
		#define DEBUG_MSG_IFELSE(condition, message_true, message_false) do { if (condition) { DebugMessage(message_true); } else { DebugMessage(message_false); } } while(0) 
	#else
		#define DEBUG_MSG(message) ((void)0)
		#define DEBUG_MSG_FUNC(funcName, message) ((void)0)
		#define DEBUG_MSG_DAM(funcName, message, atk, target) ((void)0)
		#define DEBUG_MSG_IF(condition, message) ((void)0)
		#define DEBUG_MSG_IFELSE(condition, message_true, message_false) ((void)0)
	#endif

	void CreateDebugFile();
	extern void PrintDebug(zSTRING message);
	extern void DebugMessage(zSTRING message);
	extern void DebugFuncMessage(zSTRING funcName, zSTRING message);
	extern void DebugDamageMessage(zSTRING funcName, zSTRING message, oCNpc* atk, oCNpc* target);
}