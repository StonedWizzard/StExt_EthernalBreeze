#pragma once
#include <StExt_Debug.h>
#include <StExt_IO.h>

namespace Gothic_II_Addon
{
	struct ItemClassDescriptor;
	struct ItemsGeneratorConfig;

	enum class JsonFieldType
	{
		Int,
		Float,
		Bool,
		Byte,
		String,
		ArrayInt,
		ArrayFloat,
		ArrayBool,
		ArrayByte,
		ArrayString
	};

	struct JsonFieldDesc
	{
		const char* name;
		size_t offset;
		JsonFieldType type;
	};

	extern JsonFieldDesc ItemClassDescriptor_Meta[];
	extern JsonFieldDesc ItemsGeneratorConfig_Meta[];

	void InitItemsClassData();
	bool ReadItemsGeneratorConfigs(const zSTRING& path, ItemsGeneratorConfig& config);
}