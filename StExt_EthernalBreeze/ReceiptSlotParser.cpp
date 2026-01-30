#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	enum class SlotParserValueMode : uint
	{
		Single,
		Range,
		Flags,
		Array,
		Dict,
		DictRange,
	};

	enum class SlotParserValueType : uint
	{
		Int,
		Float,
		Bool,
		Byte,
		String
	};

	struct SlotParserToken
	{
		const zSTRING Token;
		const size_t ValueOffset;
		const SlotParserValueType ValueType;
		const SlotParserValueMode ValueMode;
		const FilterFlags EnableFilter;

		SlotParserToken(zSTRING token, size_t valueOffset, SlotParserValueType valueType, SlotParserValueMode valueMode, FilterFlags enableFilter) :
			Token(token), ValueOffset(valueOffset), ValueType(valueType), ValueMode(valueMode), EnableFilter(enableFilter) { }
	};

	struct SlotParserExpression
	{
		const SlotParserToken* Token;
		Array<zSTRING> Values;

		SlotParserExpression() 
		{
			Token = Null;
			Values = Array<zSTRING>();
		}
		SlotParserExpression(const SlotParserToken* token) :Token(token) { Values = Array<zSTRING>(); }
	};

	StringMap<SlotParserToken*> SlotParserTokens(256);


	void InitReceiptSlotParser()
	{
		SlotParserTokens.Insert("COUNT", new SlotParserToken("COUNT", offsetof(ReceiptSlotInfo, ItemsCount), SlotParserValueType::Int, SlotParserValueMode::Single, FilterFlags::ItemsCount));
		SlotParserTokens.Insert("ITEM", new SlotParserToken("ITEM", offsetof(ReceiptSlotInfo, ItemInstances), SlotParserValueType::String, SlotParserValueMode::Array, FilterFlags::ItemInstances));
		SlotParserTokens.Insert("PROTOTYPE", new SlotParserToken("PROTOTYPE", offsetof(ReceiptSlotInfo, BaseItemInstances), SlotParserValueType::String, SlotParserValueMode::Array, FilterFlags::BaseItemInstances));
		SlotParserTokens.Insert("CLASS", new SlotParserToken("CLASS", offsetof(ReceiptSlotInfo, ItemClasses), SlotParserValueType::Int, SlotParserValueMode::Array, FilterFlags::ItemClasses));

		SlotParserTokens.Insert("MAIN_FLAGS", new SlotParserToken("MAIN_FLAGS", offsetof(ReceiptSlotInfo, ItemMainFlags), SlotParserValueType::Int, SlotParserValueMode::Flags, FilterFlags::ItemMainFlags));
		SlotParserTokens.Insert("BASE_FLAGS", new SlotParserToken("BASE_FLAGS", offsetof(ReceiptSlotInfo, ItemBaseFlags), SlotParserValueType::Int, SlotParserValueMode::Flags, FilterFlags::ItemBaseFlags));
		SlotParserTokens.Insert("ADDITIONAL_FLAGS", new SlotParserToken("ADDITIONAL_FLAGS", offsetof(ReceiptSlotInfo, ItemAdditionalFlags), SlotParserValueType::Int, SlotParserValueMode::Flags, FilterFlags::ItemAdditionalFlags));
		SlotParserTokens.Insert("DAMAGE_FLAGS", new SlotParserToken("DAMAGE_FLAGS", offsetof(ReceiptSlotInfo, ItemDamageType), SlotParserValueType::Int, SlotParserValueMode::Flags, FilterFlags::ItemDamageType));
		
		SlotParserTokens.Insert("LEVEL", new SlotParserToken("LEVEL", offsetof(ReceiptSlotInfo, ItemLevel), SlotParserValueType::Int, SlotParserValueMode::Range, FilterFlags::ItemLevel));
		SlotParserTokens.Insert("RANK", new SlotParserToken("RANK", offsetof(ReceiptSlotInfo, ItemRank), SlotParserValueType::Int, SlotParserValueMode::Range, FilterFlags::ItemRank));
		SlotParserTokens.Insert("QUALITY", new SlotParserToken("QUALITY", offsetof(ReceiptSlotInfo, ItemQuality), SlotParserValueType::Int, SlotParserValueMode::Range, FilterFlags::ItemQuality));

		SlotParserTokens.Insert("TAG_INCLUDED", new SlotParserToken("TAG_INCLUDED", offsetof(ReceiptSlotInfo, ItemIncludeTags), SlotParserValueType::Byte, SlotParserValueMode::Array, FilterFlags::ItemIncludeTags));
		SlotParserTokens.Insert("TAG_EXCLUDED", new SlotParserToken("TAG_EXCLUDED", offsetof(ReceiptSlotInfo, ItemExcludeTags), SlotParserValueType::Byte, SlotParserValueMode::Array, FilterFlags::ItemExcludeTags));
		SlotParserTokens.Insert("CRAFT_DATA", new SlotParserToken("CRAFT_DATA", offsetof(ReceiptSlotInfo, ItemCraftData), SlotParserValueType::Int, SlotParserValueMode::DictRange, FilterFlags::ItemCraftData));
		SlotParserTokens.Insert("CRAFT_FLAGS", new SlotParserToken("CRAFT_FLAGS", offsetof(ReceiptSlotInfo, ItemCraftFlags), SlotParserValueType::Byte, SlotParserValueMode::DictRange, FilterFlags::ItemCraftFlags));
	}

	inline const SlotParserToken* FindToken(const zSTRING& name)
	{
		auto it = SlotParserTokens.Find(name);
		if (it) return *it;
		return Null;
	}

	inline zSTRING NormalizePattern(zSTRING& pattern) 
	{ 
		zSTRING result;
		for (uint i = 0; i < (uint)pattern.Length(); ++i)
		{
			const char c = pattern[i];
			if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue;
			result += c;
		}
		return result.Upper();
	}

	inline zSTRING NormalizeValue(const zSTRING& pattern)
	{
		zSTRING result;
		for (uint i = 0; i < (uint)pattern.Length(); ++i)
		{
			const char c = pattern[i];
			if (c == '{' || c == '}' || c == ',' || c == '=' || c == ':' || c == ';') continue;
			result += c;
		}
		return result;
	}


	bool ParsePattern(const zSTRING& pattern, const SlotFlags slotFlags, Array<SlotParserExpression>& expressions)
	{
		if (pattern.IsEmpty()) return true;

		uint pos = 0;
		const uint len = pattern.Length();
		while (pos < len) 
		{
			uint sep = pattern.Search(";", pos);
			zSTRING rawExpression;
			if (sep == Invalid) 
			{
				rawExpression = pattern.Copy(pos, len - pos);
				pos = len;
			}
			else 
			{
				rawExpression = pattern.Copy(pos, sep - pos);
				pos = sep + 1;
			}
			if (rawExpression.IsEmpty()) continue;

			int eq = rawExpression.Search("=", 0U);
			zSTRING tokenKey, tokenValues;
			if (eq == Invalid) { tokenKey = rawExpression; }
			else 
			{
				tokenKey = rawExpression.Copy(0U, eq);
				ulong length = rawExpression.Length() - (eq + 1);
				tokenValues = rawExpression.Copy(eq + 1, length);
			}

			const SlotParserToken* tokenPtr = FindToken(tokenKey);
			if (tokenPtr == Null)
			{
				DEBUG_MSG("ParsePattern: token '" + tokenKey + "' not defined!");
				continue;
			}

			SlotParserExpression expr = SlotParserExpression(tokenPtr);
			if (tokenValues.IsEmpty())
			{
				expr.Values.Insert("");
				expressions.Insert(expr);
				continue;
			}

			bool hasBraces = (tokenValues.Search("{", 0U) != Invalid);
			if (hasBraces) 
			{
				int depth = 0;
				int start = 0;
				for (int i = 0; i < tokenValues.Length(); ++i)
				{
					char c = tokenValues[static_cast<uint>(i)];
					if (c == '{') { depth++; }
					else if (c == '}') { if (depth > 0) depth--; }
					else if (c == ',' && depth == 0) 
					{
						zSTRING val = tokenValues.Copy(start, i - start);
						if (!val.IsEmpty()) expr.Values.Insert(val);
						start = i + 1;
					}
				}

				if (start < tokenValues.Length())
				{
					zSTRING val = tokenValues.Copy(start, tokenValues.Length() - start);
					if (!val.IsEmpty()) expr.Values.Insert(val);
				}
				DEBUG_MSG_IF((depth != 0), "ParsePattern: unbalanced braces in token '" + tokenKey + "'");
			}
			else 
			{
				bool splitByComma = false;
				if (tokenPtr) 
				{
					switch (tokenPtr->ValueMode) 
					{
						case SlotParserValueMode::Range:
						case SlotParserValueMode::Array:
						case SlotParserValueMode::Dict:
						case SlotParserValueMode::DictRange:
							splitByComma = true;
							break;
						default: splitByComma = false;
					}
				}
				else { splitByComma = (tokenValues.Search(",", 0U) != Invalid); }

				if (splitByComma) 
				{
					int start = 0;
					for (int i = 0; i < tokenValues.Length(); ++i) 
					{
						if (tokenValues[static_cast<uint>(i)] == ',')
						{
							zSTRING val = tokenValues.Copy(start, i - start);
							if (!val.IsEmpty()) expr.Values.Insert(val);
							start = i + 1;
						}
					}
					if (start < tokenValues.Length()) 
					{
						zSTRING val = tokenValues.Copy(start, tokenValues.Length() - start);
						if (!val.IsEmpty()) expr.Values.Insert(val);
					}
				}
				else { expr.Values.Insert(tokenValues); }
			}
			expressions.Insert(expr);
		}
		return true;
	}


	inline zCPar_Symbol* ValueIsSymbol(const zSTRING& value) { return parser->GetSymbol(value); }

	inline int ParseInt(const zSTRING& rawValue)
	{
		const zSTRING value = NormalizeValue(rawValue);
		if (value.IsEmpty()) return 0;
		zCPar_Symbol* sym = ValueIsSymbol(value);
		if (sym) return sym->single_intdata;
		return value.ToInt();
	}
	inline byte ParseByte(const zSTRING& rawValue)
	{
		const zSTRING value = NormalizeValue(rawValue);
		if (value.IsEmpty()) return 0;
		zCPar_Symbol* sym = ValueIsSymbol(value);
		if (sym) return static_cast<byte>(sym->single_intdata);
		return static_cast<byte>(value.ToInt());
	}
	inline float ParseFloat(const zSTRING& rawValue)
	{
		const zSTRING value = NormalizeValue(rawValue);
		if (value.IsEmpty()) return 0.0f;
		zCPar_Symbol* sym = ValueIsSymbol(value);
		if (sym) return sym->single_floatdata;
		return value.ToFloat();
	}
	inline bool ParseBool(const zSTRING& rawValue)
	{
		const zSTRING value = NormalizeValue(rawValue);
		if (value.IsEmpty()) return false;
		if (value.CompareI("1") || value.CompareI("true") || value.CompareI("yes")) return true;
		if (value.CompareI("0") || value.CompareI("false") || value.CompareI("no")) return false;

		zCPar_Symbol* sym = ValueIsSymbol(value);
		if (sym) return sym->single_intdata != 0;
		return value.ToInt() != 0;
	}
	inline zSTRING ParseString(const zSTRING& rawValue)
	{
		zSTRING result = NormalizeValue(rawValue);
		zCPar_Symbol* sym = ValueIsSymbol(result);
		if (sym && sym->stringdata) { result = sym->stringdata->ToChar(); }
		return result;
	}


	void CompileSingleValue(const SlotParserExpression& expr, void* fieldPtr)
	{
		if (expr.Values.GetNum() == 0) return;
		const SlotParserToken& token = *expr.Token;
		const zSTRING& rawValue = expr.Values[0];

		switch (token.ValueType)
		{
			case SlotParserValueType::Int: *reinterpret_cast<int*>(fieldPtr) = ParseInt(rawValue); break;
			case SlotParserValueType::Float: *reinterpret_cast<float*>(fieldPtr) = ParseFloat(rawValue); break;
			case SlotParserValueType::Bool: *reinterpret_cast<bool*>(fieldPtr) = ParseBool(rawValue); break;
			case SlotParserValueType::Byte: *reinterpret_cast<byte*>(fieldPtr) = ParseByte(rawValue); break;
			case SlotParserValueType::String: *reinterpret_cast<zSTRING*>(fieldPtr) = ParseString(rawValue); break;
			default: break;
		}
	}

	template <typename T, typename ParseFunc>
	inline void CompileRangeValueImpl(void* fieldPtr, const zSTRING& rawValueMin, const zSTRING& rawValueMax, ParseFunc parse)
	{
		auto& range = *reinterpret_cast<ValueRange<T>*>(fieldPtr);
		if (!rawValueMin.IsEmpty()) range.Min = parse(rawValueMin);
		if (!rawValueMax.IsEmpty()) range.Max = parse(rawValueMax);
	}
	void CompileRangeValue(const SlotParserExpression& expr, void* fieldPtr)
	{
		if (expr.Values.GetNum() == 0) return;
		const SlotParserToken& token = *expr.Token;
		zSTRING rawValueMin;
		zSTRING rawValueMax;

		if (expr.Values.GetNum() == 1)
		{
			uint sep = expr.Values[0].Search("-", 0U);
			if (sep != Invalid)
			{
				rawValueMin = expr.Values[0].Copy(0U, sep);
				rawValueMax = expr.Values[0].Copy(sep + 1U, expr.Values[0].Length() - (sep + 1U));
			}
			else rawValueMin = expr.Values[0];
		}
		else
		{
			rawValueMin = expr.Values[0];
			rawValueMax = expr.Values[1];
		}

		switch (token.ValueType)
		{
			case SlotParserValueType::Int:
				CompileRangeValueImpl<int>(fieldPtr, rawValueMin, rawValueMax, ParseInt);
				break;
			case SlotParserValueType::Float:
				CompileRangeValueImpl<float>(fieldPtr, rawValueMin, rawValueMax, ParseFloat);
				break;
			case SlotParserValueType::Byte:
				CompileRangeValueImpl<byte>(fieldPtr, rawValueMin, rawValueMax, ParseByte);
				break;
			
			case SlotParserValueType::Bool: break;
			case SlotParserValueType::String: break;
			default: break;
		}
	}

	template <typename T, typename ParseFunc>
	inline void CompileFlagsValueImpl(void* fieldPtr, const SlotParserExpression& expr, ParseFunc parse)
	{
		T value = 0;
		for (uint i = 0; i < expr.Values.GetNum(); ++i)
			value |= static_cast<T>(parse(expr.Values[i]));
		*reinterpret_cast<T*>(fieldPtr) = value;
	}
	void CompileFlagsValue(const SlotParserExpression& expr, void* fieldPtr)
	{
		if (expr.Values.GetNum() == 0) return;
		const SlotParserToken& token = *expr.Token;
		switch (token.ValueType)
		{
			case SlotParserValueType::Int: CompileFlagsValueImpl<int>(fieldPtr, expr, ParseInt); break;
			case SlotParserValueType::Byte: CompileFlagsValueImpl<byte>(fieldPtr, expr, ParseByte); break;
			default: break;
		}
	}

	template <typename T, typename ParseFunc>
	inline void CompileArrayValueImpl(void* fieldPtr, const SlotParserExpression& expr, ParseFunc parse)
	{
		auto& arr = *reinterpret_cast<Array<T>*>(fieldPtr);
		arr.Clear();
		for (uint i = 0; i < expr.Values.GetNum(); ++i)
			arr.InsertEnd(parse(expr.Values[i]));
	}
	void CompileArrayValue(const SlotParserExpression& expr, void* fieldPtr)
	{
		if (expr.Values.GetNum() == 0) return;
		const SlotParserToken& token = *expr.Token;
		switch (token.ValueType)
		{
			case SlotParserValueType::Int: CompileArrayValueImpl<int>(fieldPtr, expr, ParseInt); break;
			case SlotParserValueType::Float: CompileArrayValueImpl<float>(fieldPtr, expr, ParseFloat); break;
			case SlotParserValueType::Bool: CompileArrayValueImpl<bool>(fieldPtr, expr, ParseBool); break;
			case SlotParserValueType::Byte: CompileArrayValueImpl<byte>(fieldPtr, expr, ParseByte); break;
			case SlotParserValueType::String: CompileArrayValueImpl<zSTRING>(fieldPtr, expr, ParseString); break;
			default: break;
		}
	}

	template <typename T, typename ParseFunc>
	inline void CompileDictValueImpl(void* fieldPtr, const SlotParserExpression& expr, ParseFunc parse)
	{
		auto& map = *reinterpret_cast<Map<int, T>*>(fieldPtr);
		for (uint j = 0; j < expr.Values.GetNum(); ++j)
		{
			const zSTRING& rawValue = expr.Values[j];
			int keySeparator = rawValue.Search(":", 0U);
			if (keySeparator == Invalid)
			{
				DEBUG_MSG("CompileDictValueImpl: fail to compile expression: '" + rawValue + "' - index required!");
				continue;
			}

			int key = ParseInt(rawValue.Copy(0U, keySeparator));
			T value = parse(rawValue.Copy(keySeparator + 1U, rawValue.Length() - (keySeparator + 1U)));
			map.Insert(key, value);
		}
	}
	void CompileDictValue(const SlotParserExpression& expr, void* fieldPtr)
	{
		if (expr.Values.GetNum() == 0) return;
		const SlotParserToken& token = *expr.Token;
		switch (token.ValueType)
		{
			case SlotParserValueType::Int: CompileDictValueImpl<int>(fieldPtr, expr, ParseInt); break;
			case SlotParserValueType::Float: CompileDictValueImpl<float>(fieldPtr, expr, ParseFloat); break;
			case SlotParserValueType::Bool: CompileDictValueImpl<bool>(fieldPtr, expr, ParseBool); break;
			case SlotParserValueType::Byte: CompileDictValueImpl<byte>(fieldPtr, expr, ParseByte); break;
			case SlotParserValueType::String: CompileDictValueImpl<zSTRING>(fieldPtr, expr, ParseString); break;
			default: break;
		}
	}

	template <typename T, typename ParseFunc>
	inline void CompileDictRangeValueImpl(void* fieldPtr, const SlotParserExpression& expr, ParseFunc parse)
	{
		auto& map = *reinterpret_cast<Map<int, ValueRange<T>>*>(fieldPtr);
		for (uint j = 0; j < expr.Values.GetNum(); ++j)
		{
			const zSTRING& rawValue = expr.Values[j];
			int keySeparator = rawValue.Search(":", 0U);
			if (keySeparator == Invalid)
			{
				DEBUG_MSG("CompileDictRangeValueImpl: fail to compile expression: '" + rawValue + "' - index required!");
				continue;
			}

			int key = ParseInt(rawValue.Copy(0U, keySeparator));
			zSTRING rangeStr = rawValue.Copy(keySeparator + 1U, rawValue.Length() - (keySeparator + 1U));

			ValueRange<T> range;
			int dash = rangeStr.Search("-", 0U);
			if (dash == Invalid) dash = rangeStr.Search(",", 0U);

			range.Index = key;
			if (dash == Invalid) range.Min = parse(rangeStr);			
			else
			{
				range.Min = parse(rangeStr.Copy(0U, dash));
				range.Max = parse(rangeStr.Copy(static_cast<uint>(dash + 1), rangeStr.Length() - (dash + 1)));
			}
			map.Insert(key, range);
		}
	}
	void CompileDictRangeValue(const SlotParserExpression& expr, void* fieldPtr)
	{
		if (expr.Values.GetNum() == 0) return;
		const SlotParserToken& token = *expr.Token;
		switch (token.ValueType)
		{
			case SlotParserValueType::Int: CompileDictRangeValueImpl<int>(fieldPtr, expr, ParseInt); break;
			case SlotParserValueType::Float: CompileDictRangeValueImpl<float>(fieldPtr, expr, ParseFloat); break;
			case SlotParserValueType::Bool: CompileDictRangeValueImpl<bool>(fieldPtr, expr, ParseBool); break;
			case SlotParserValueType::Byte: CompileDictRangeValueImpl<byte>(fieldPtr, expr, ParseByte); break;
			case SlotParserValueType::String: CompileDictRangeValueImpl<zSTRING>(fieldPtr, expr, ParseString); break;
			default: break;
		}
	}


	bool CompileExpressions(const Array<SlotParserExpression>& expressions, ReceiptSlotInfo& slot)
	{
		for (uint i = 0; i < expressions.GetNum(); ++i)
		{
			const SlotParserExpression& expr = expressions[i];
			if (!expr.Token) continue;

			const SlotParserToken& token = *expr.Token;
			void* fieldPtr = reinterpret_cast<char*>(&slot) + token.ValueOffset;
			switch (token.ValueMode)
			{
				case SlotParserValueMode::Single: CompileSingleValue(expr, fieldPtr); break;
				case SlotParserValueMode::Range: CompileRangeValue(expr, fieldPtr); break;
				case SlotParserValueMode::Flags: CompileFlagsValue(expr, fieldPtr); break;
				case SlotParserValueMode::Array: CompileArrayValue(expr, fieldPtr); break;
				case SlotParserValueMode::Dict: CompileDictValue(expr, fieldPtr); break;
				case SlotParserValueMode::DictRange: CompileDictRangeValue(expr, fieldPtr); break;

				default:
					DEBUG_MSG("CompileExpressions: unrecognized value mode: "+ Z((int)token.ValueMode) + "!");
					return false;
			}
			slot.EnabledFilters |= token.EnableFilter;
		}
		return true;
	}


	//--------------------------------------------------------------------------------------------

	bool CompileSlotPattern(zSTRING& rawPattern, const int slotFlags, ReceiptSlotInfo& slot)
	{
		if (rawPattern.IsEmpty())
		{
			slot.IsEmpty = true;
			return true;
		}

		const zSTRING pattern = NormalizePattern(rawPattern);
		const SlotFlags flags = static_cast<SlotFlags>(slotFlags);
		Array<SlotParserExpression> expressions = Array<SlotParserExpression>();
		if (!ParsePattern(pattern, flags, expressions))
		{
			slot.IsEmpty = true;
			DEBUG_MSG("CompileSlotPattern: fail to parse slot pattern: '" + rawPattern + "'!");
			return false;
		}

		if (!CompileExpressions(expressions, slot))
		{
			slot.IsEmpty = true;
			DEBUG_MSG("CompileSlotPattern: fail to compile slot pattern: '" + rawPattern + "'!");
			return false;
		}

		slot.IsEmpty = false;
		slot.Flags = flags;
		slot.Pattern = pattern;

		return true;
	}
}