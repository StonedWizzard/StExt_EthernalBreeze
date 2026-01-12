#pragma once

namespace Gothic_II_Addon
{
	//-----------------------------------------------------------------
	//							DEBUG
	//-----------------------------------------------------------------
	class zSTRING;

	#define DebugEnabled true
	#define DebugStackEnabled false

	#if DebugEnabled
		#define DEBUG_MSG(message) DebugMessage(message)
		#define DEBUG_MSG_IF(condition, message) do { if (condition) { DebugMessage(message); } } while(0)
		#define DEBUG_MSG_IFELSE(condition, message_true, message_false) do { if (condition) { DebugMessage(message_true); } else { DebugMessage(message_false); } } while(0) 
	#else
		#define DEBUG_MSG(message) {}
		#define DEBUG_MSG_IF(condition, message) {}
		#define DEBUG_MSG_IFELSE(condition, message_true, message_false) {}
	#endif

	void CreateDebugFile();
	void PrintDebug(zSTRING message);
	void DebugMessage(zSTRING message);
}