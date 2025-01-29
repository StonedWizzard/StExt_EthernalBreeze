#include <UnionAfx.h>

namespace Gothic_II_Addon
{	
	class C_ConfigPreset
	{
	public:
		zSTRING Name;
		zSTRING Text;
		zSTRING TextColor;
		zSTRING OnApply;

		void SetByScript(int index);
	};
}