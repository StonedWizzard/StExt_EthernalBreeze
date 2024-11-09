#include <UnionAfx.h>

namespace Gothic_II_Addon
{
	class C_ItemData
	{
	public:
		zSTRING InstanceName;
		zSTRING BaseInstanceName;

		int Type;
		int Flags;
		int EnchantmentId;
		int Power;

		C_ItemData();
		void Archive(zCArchiver& arc);
		void Unarchive(zCArchiver& arc);
	};
}