#pragma once
#include <UnionAfx.h>
#include <limits>

namespace Gothic_II_Addon
{
	inline zSTRING& NormalizeInstanceName(zSTRING& s) { return s = s.Upper(); }

	inline zSTRING GetItemInstanceName(const oCItem* itm) { return itm ? const_cast<oCItem*>(itm)->GetInstanceName().Upper() : "Null"; }

	template <typename T>
	inline constexpr T ValidateValue(const T& val, const T& min_val, const T& max_val) { return (val < min_val) ? min_val : (val > max_val) ? max_val : val; }

	template <typename T>
	inline constexpr T ValidateValueMin(const T& val, const T& min_val) { return (val < min_val) ? min_val : val; }

	template <typename T>
	inline constexpr T ValidateValueMax(const T& val, const T& max_val) { return (val > max_val) ? max_val : val; }

	template <typename T>
	inline constexpr bool ValueInRange(const T& val, const T& min_val, const T& max_val) { return val >= min_val && val <= max_val; }

	template <typename T>
	inline constexpr bool IsIndexInBounds(const T& index, const T& max_size) { return index >= 0 && index < max_size; }

	template <typename T>
	inline constexpr bool HasFlag(const T& flags, const T& bits) { return (flags & bits) != T(); }

	template <typename T>
	inline void AddFlag(T& flags, const T& bits) { flags |= bits; }

	template <typename T>
	inline void RemoveFlag(T& flags, const T& bits) { flags &= ~bits; }

	template <typename T, uint N>
	inline int FindIndexInArray(const T(&arr)[N], const T& value)
	{
		for (uint i = 0; i < N; ++i) {
			if (arr[i] == value) return static_cast<int>(i);
		}
		return Invalid;
	}

	template <typename T, uint N>
	inline void FillArray(T(&arr)[N], const T& value) {
		for (uint i = 0; i < N; ++i) arr[i] = value;
	}

	template <uint N>
	inline int FindEmptySlotIndex(const int(&slots)[N], const int maxSlots, const int emptyValue)
	{
		int slotsUsed = 0;
		for (int i = 0; i < static_cast<int>(N); ++i) {
			if (slots[i] == emptyValue) return i;

			++slotsUsed;
			if ((maxSlots > Invalid) && (slotsUsed >= maxSlots))
				return Invalid;
		}
		return Invalid;
	}

	inline zSTRING FormatNumberPad(int number, int width = 0)
	{
		char buf[32];
		int pos = sizeof(buf) - 1;
		buf[pos] = '\0';
		bool isNegative = number < 0;
		uint value = isNegative ? (uint)(-number) : (uint)number;

		do
		{
			buf[--pos] = '0' + (value % 10);
			value /= 10;
		} 
		while (value > 0);
		if (isNegative) buf[--pos] = '-';

		int numLen = (int)(sizeof(buf) - pos - 1);
		int totalLen = width > numLen ? width : numLen;
		int padding = totalLen - numLen;

		if (padding > 0)
		{
			memmove(buf + padding, buf + pos, numLen + 1);
			for (int i = 0; i < padding; ++i) buf[i] = '0';
			pos = 0;
		}
		return zSTRING(buf + pos);
	}

	inline void AppendTag(zSTRING& line, const zSTRING& tag)
	{
		if (!line.IsEmpty()) line += " | ";
		line += tag;
	}


#ifdef max
	#pragma push_macro("max")
	#undef max
	#define HAD_MAX_MACRO
#endif

	template <class T>
	struct ValueRange
	{
		int Index;
		T Min;
		T Max;

		constexpr ValueRange() noexcept : Index(Invalid), Min(0), Max(std::numeric_limits<T>::max()) {}
		constexpr ValueRange(int index, T min, T max) noexcept : Index(index), Min(min), Max(max) {}
		constexpr explicit ValueRange(int index, T value) noexcept : Index(index), Min(value), Max(std::numeric_limits<T>::max()) {}
		constexpr explicit ValueRange(T value) noexcept : Index(Invalid), Min(value), Max(std::numeric_limits<T>::max()) {}
	};

#ifdef HAD_MAX_MACRO
	#pragma pop_macro("max")
	#undef HAD_MAX_MACRO
#endif


	// String extensions
	inline bool operator<(const zSTRING& a, const zSTRING& b) { return a.Compare(b.ToChar()) < 0; }
	inline bool operator>(const zSTRING& a, const zSTRING& b) { return a.Compare(b.ToChar()) > 0; }
	//inline bool operator<=(const zSTRING& a, const zSTRING& b) { return a.CompareI(b.ToChar()) <= 0; }
	//inline bool operator>=(const zSTRING& a, const zSTRING& b) { return a.CompareI(b.ToChar()) >= 0; }


	zSTRING GetModVersion();
	string GetSaveSlotNameByID(int ID);
}