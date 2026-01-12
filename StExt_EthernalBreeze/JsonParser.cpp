#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include <JsonParser.h>
#include <StonedExtension.h>

#define READ_JSON(expr) do { if (!(expr)) return false; } while(0)
namespace Gothic_II_Addon
{
	inline bool ParseJsonContent(const char* buffer, rapidjson::Document& doc)
	{
		constexpr auto parseFlags = rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag;
		bool isSuccess = true;

		doc = rapidjson::Document();
		rapidjson::ParseResult parseResult = doc.Parse<parseFlags>(buffer);
		if (parseResult.IsError())
		{
			DEBUG_MSG("ParseJsonFile: file parsing failed! Error: " + Z(GetParseError_En(parseResult.Code())) + " | Offset: " + Z((int)parseResult.Offset()));
			isSuccess = false;
		}
		return isSuccess;
	}


	inline bool ReadJsonInt(const rapidjson::Value& source, const char* name, int& value)
	{
		if (source.HasMember(name) && source[name].IsInt()) {
			value = source[name].GetInt();
			return true;
		}
		DEBUG_MSG("ReadJsonInt: fail read property '" + Z(name) + "'");
		return false;
	}

	inline bool ReadJsonBool(const rapidjson::Value& source, const char* name, bool& value)
	{
		if (source.HasMember(name) && source[name].IsBool()) {
			value = source[name].GetBool();
			return true;
		}
		DEBUG_MSG("ReadJsonBool: fail read property '" + Z(name) + "'");
		return false;
	}

	inline bool ReadJsonByte(const rapidjson::Value& source, const char* name, byte& value)
	{
		if (source.HasMember(name) && source[name].IsInt()) {
			value = static_cast<byte>(source[name].GetInt());
			return true;
		}
		DEBUG_MSG("ReadJsonByte: fail read property '" + Z(name) + "'");
		return false;
	}

	inline bool ReadJsonFloat(const rapidjson::Value& source, const char* name, float& value)
	{
		if (source.HasMember(name))
		{
			if (source[name].IsFloat()) { value = source[name].GetFloat(); return true; }
			if (source[name].IsInt()) { value = static_cast<float>(source[name].GetInt()); return true; }
		}
		DEBUG_MSG("ReadJsonFloat: fail read property '" + Z(name) + "'");
		return false;
	}

	inline bool ReadJsonString(const rapidjson::Value& source, const char* name, zSTRING& value)
	{
		if (source.HasMember(name) && source[name].IsString()) {
			value = source[name].GetString();
			return true;
		}
		DEBUG_MSG("ReadJsonString: fail read property '" + Z(name) + "'");
		return false;
	}


	template<typename T>
	using JsonElementReader = bool(*)(const rapidjson::Value&, T&);

	inline bool ReadJsonIntElement(const rapidjson::Value& v, int& out)
	{
		if (!v.IsInt()) return false;
		out = v.GetInt();
		return true;
	}

	inline bool ReadJsonFloatElement(const rapidjson::Value& v, float& out)
	{
		if (v.IsFloat()) { out = v.GetFloat(); return true; }
		if (v.IsInt()) { out = (float)v.GetInt(); return true; }
		return false;
	}

	inline bool ReadJsonBoolElement(const rapidjson::Value& v, bool& out)
	{
		if (!v.IsBool()) return false;
		out = v.GetBool();
		return true;
	}

	inline bool ReadJsonByteElement(const rapidjson::Value& v, byte& out)
	{
		if (!v.IsInt()) return false;
		out = (byte)v.GetInt();
		return true;
	}

	inline bool ReadJsonStringElement(const rapidjson::Value& v, zSTRING& out)
	{
		if (!v.IsString()) return false;
		out = v.GetString();
		return true;
	}


	template<typename T>
	bool ReadJsonArray(const rapidjson::Value& source, const char* name, Array<T>& out, JsonElementReader<T> reader)
	{
		if (!source.HasMember(name))
		{
			DEBUG_MSG("ReadJsonArray: array element '" + Z(name) + "' not found!");
			return false;
		}
		const rapidjson::Value& arrVal = source[name];
		if (!arrVal.IsArray())
		{
			DEBUG_MSG("ReadJsonArray: member '" + Z(name) + "' is not an array!");
			return false;
		}

		out = Array<T>();
		int index = 0;
		for (auto& e : arrVal.GetArray())
		{
			T value;
			if (!reader(e, value))
			{
				DEBUG_MSG("ReadJsonArray: failed parse value in '" + Z(name) + "' Element index: " + Z(index));
				return false;
			}
			out.InsertEnd(value);
			++index;
		}
		return true;
	}

	bool ReadJsonStruct(const rapidjson::Value& src, void* structPtr, const JsonFieldDesc* desc)
	{
		for (int i = 0; desc[i].name != nullptr; ++i)
		{
			const JsonFieldDesc& f = desc[i];
			char* fieldPtr = (char*)structPtr + f.offset;

			switch (f.type)
			{
				case JsonFieldType::Int: READ_JSON(ReadJsonInt(src, f.name, *(int*)fieldPtr)); break;
				case JsonFieldType::Float: READ_JSON(ReadJsonFloat(src, f.name, *(float*)fieldPtr)); break;
				case JsonFieldType::Bool: READ_JSON(ReadJsonBool(src, f.name, *(bool*)fieldPtr)); break;
				case JsonFieldType::Byte: READ_JSON(ReadJsonByte(src, f.name, *(byte*)fieldPtr)); break;
				case JsonFieldType::String: READ_JSON(ReadJsonString(src, f.name, *(zSTRING*)fieldPtr)); break;
				case JsonFieldType::ArrayInt: READ_JSON(ReadJsonArray<int>(src, f.name, *(Array<int>*)fieldPtr, ReadJsonIntElement)); break;
				case JsonFieldType::ArrayFloat: READ_JSON(ReadJsonArray<float>(src, f.name, *(Array<float>*)fieldPtr, ReadJsonFloatElement)); break;
				case JsonFieldType::ArrayBool: READ_JSON(ReadJsonArray<bool>(src, f.name, *(Array<bool>*)fieldPtr, ReadJsonBoolElement)); break;
				case JsonFieldType::ArrayByte: READ_JSON(ReadJsonArray<byte>(src, f.name, *(Array<byte>*)fieldPtr, ReadJsonByteElement)); break;
				case JsonFieldType::ArrayString: READ_JSON(ReadJsonArray<zSTRING>(src, f.name, *(Array<zSTRING>*)fieldPtr, ReadJsonStringElement)); break;
			}
		}
		return true;
	}

	bool ReadJsonItemClassDescriptor(const rapidjson::Value& source, ItemClassDescriptor& value) { return ReadJsonStruct(source, &value, ItemClassDescriptor_Meta); }
	bool ReadJsonItemsGeneratorConfigs(const rapidjson::Value& source, ItemsGeneratorConfig& value) { return ReadJsonStruct(source, &value, ItemsGeneratorConfig_Meta); }

	inline void RegisterItemInstancesName(const Array<zSTRING>& arr, const ItemClassKey classKey)
	{
		if (arr.IsEmpty()) return;
		for (uint i = 0; i < arr.GetNum(); ++i)
		{
			if (ItemsClassDataIndexer_InstanceName.GetSafePair(arr[i]))
			{
				DEBUG_MSG("RegisterItemInstancesName: instance '" + Z(arr[i]) + "' has already assigned class!");
				continue;
			}
			ItemsClassDataIndexer_InstanceName.Insert(arr[i], classKey);
		}
	}

	inline void RegisterItemClassData(const ItemClassKey classKey, const ItemClassDescriptor& classData)
	{
		auto pair = ItemsClassData.GetSafePair(classKey);
		if (pair)
		{
			DEBUG_MSG("RegisterItemClassData: fail register item class data: " + Z((int)classKey) + " because it already registered!");
			return;
		}

		RegisterItemInstancesName(classData.LowTierPrototypes, classKey);
		RegisterItemInstancesName(classData.MedTierPrototypes, classKey);
		RegisterItemInstancesName(classData.TopTierPrototypes, classKey);
		RegisterItemInstancesName(classData.ExtraTierPrototypes, classKey);
		ItemsClassData.Insert(classKey, classData);
	}

	inline void CalculateItemRankThresholds(ItemsGeneratorConfig& config)
	{
		config.ItemRankThresholds.Clear();
		config.ItemMaxRank = parser->GetSymbol("StExt_ItemRankMax")->single_intdata;

		int current = 0;
		config.ItemRankThresholds.InsertEnd(current);

		int step = config.ItemRankBaseStep;
		for (int i = 1; i < config.ItemMaxRank; i++)
		{
			current += step;
			config.ItemRankThresholds.InsertEnd(current);
			step = static_cast<int>(step * config.ItemRankGrowthFactor);
		}
	}

	//-----------------------------------------------------------------------------
	//							(ItemsClass reader API)
	//-----------------------------------------------------------------------------

	void InitItemsClassData()
	{
		const zSTRING path = zoptions->GetDirString(zTOptionPaths::DIR_SYSTEM) + "\\Autorun\\EthernalBreeze\\Data\\ItemClassData.json";
		DEBUG_MSG("InitializeItemsClassData: reading '" + Z(path) + "' ...");

		zSTRING FileBuffer;
		if (!ParseTextFile(path, FileBuffer))
		{
			Message::Error((string)("Fail to read ItemClassData!\nPath: " + path), "Critical error!");
			gameMan->ExitGame();
			return;
		}

		rapidjson::Document itemClassConfigsJson;
		if (!ParseJsonContent(FileBuffer.ToChar(), itemClassConfigsJson))
		{
			Message::Error((string)("Fail to parse ItemClassData!\nPath: " + path), "Critical error!");
			gameMan->ExitGame();
			return;
		}

		bool isCorrupted = (!itemClassConfigsJson.HasMember("BaseItemClassDescriptor") || !itemClassConfigsJson["BaseItemClassDescriptor"].IsObject()) ||
			(!itemClassConfigsJson.HasMember("ItemsClassData") || !itemClassConfigsJson["ItemsClassData"].IsArray());
		if (isCorrupted)
		{
			DEBUG_MSG("InitializeItemsClassData: Fail to read ItemClassData! Path: " + path);
			Message::Error((string)("Fail to read ItemClassData!\nFile structure seems icorrect or corrupted.\nPath: " + path), "Critical error!");
			gameMan->ExitGame();
			return;
		}

		BaseItemClassDescriptor = ItemClassDescriptor();
		ItemsClassData = Map<ItemClassKey, ItemClassDescriptor>();
		ItemsClassDataIndexer_InstanceName = Map<zSTRING, ItemClassKey>();

		if (!ReadJsonItemClassDescriptor(itemClassConfigsJson["BaseItemClassDescriptor"], BaseItemClassDescriptor))
			DEBUG_MSG("InitializeItemsClassData: Base item class has some syntax errors!");

		const rapidjson::Value& arr = itemClassConfigsJson["ItemsClassData"];
		for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
		{
			ItemClassDescriptor classDescriptor = BaseItemClassDescriptor;
			if (!ReadJsonItemClassDescriptor(arr[i], classDescriptor))
			{
				DEBUG_MSG("InitializeItemsClassData: Item class descriptor has some syntax errors! Entry index: " + Z((int)i));
				continue;
			}

			const ItemClassKey classKey = ItemClassKey_Create(classDescriptor.Type, classDescriptor.Class, classDescriptor.SubClass);
			classDescriptor.ItemClassID = classKey;

			classDescriptor.PrimaryStatsList.QuickSort();
			classDescriptor.PrimaryAbilitiesList.QuickSort();
			classDescriptor.IncopatibleStatsList.QuickSort();
			classDescriptor.IncopatibleAbilitiesList.QuickSort();

			classDescriptor.ConditionsList.QuickSort();
			classDescriptor.PrimaryDamageTypesList.QuickSort();
			classDescriptor.PrimaryProtectionTypesList.QuickSort();
			classDescriptor.IncopatibleDamageTypesList.QuickSort();

			classDescriptor.LowTierPrototypes.QuickSort();
			classDescriptor.MedTierPrototypes.QuickSort();
			classDescriptor.TopTierPrototypes.QuickSort();
			classDescriptor.ExtraTierPrototypes.QuickSort();

			RegisterItemClassData(classKey, classDescriptor);
		}
		DEBUG_MSG("InitializeItemsClassData: Done!");
	}

	bool ReadItemsGeneratorConfigs(const zSTRING& path, ItemsGeneratorConfig& config)
	{
		config = ItemsGeneratorConfig();
		rapidjson::Document generatorConfigsJson;

		zSTRING FileBuffer;
		if (!ParseTextFile(path, FileBuffer))
		{
			DEBUG_MSG("ReadItemsGeneratorConfigs: fail to read configs: " + path);
			return false;
		}

		if (!ParseJsonContent(FileBuffer.ToChar(), generatorConfigsJson))
		{
			DEBUG_MSG("ReadItemsGeneratorConfigs: fail to parse configs: " + path);
			return false;
		}

		if (!ReadJsonItemsGeneratorConfigs(generatorConfigsJson, config))
		{
			DEBUG_MSG("ReadItemsGeneratorConfigs: config file has some errors!");
			return false;
		}

		CalculateItemRankThresholds(config);
		return true;
	}
}