#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	// This function created to support generated items by game and scripts.
	bool CreateVirtualSymbol(const zSTRING& prototypeSymName, const zSTRING& virtualSymName, int& index)
	{
		DEBUG_MSG("Copy symbol - '" + prototypeSymName + "' to '" + virtualSymName + "' ...");
		zCPar_Symbol* ps = parser->GetSymbol(prototypeSymName);
		if (!ps)
		{
			DEBUG_MSG("Copy symbol - Original symbol '" + prototypeSymName + "' not found!");
			return false;
		}

		index = parser->GetIndex(virtualSymName);
		const bool isNewSym = (index == Invalid);
		zCPar_Symbol* sym = (!isNewSym) ? parser->GetSymbol(index) : new zCPar_Symbol();

		sym->name = NormalizeInstanceName(Z(virtualSymName));
		sym->adr = ps->adr;
		sym->intdata = ps->intdata;
		sym->floatdata = ps->floatdata;
		sym->stringdata = ps->stringdata;
		sym->single_intdata = ps->single_intdata;
		sym->single_floatdata = ps->single_floatdata;
		sym->offset = ps->offset;
		sym->type = ps->type;
		sym->flags = ps->flags;
		sym->space = ps->space;
		sym->filenr = ps->filenr;
		sym->line = ps->line;
		sym->line_anz = ps->line_anz;
		sym->pos_beg = ps->pos_beg;
		sym->pos_anz = ps->pos_anz;
		sym->parent = ps->parent;

		if (index != Invalid)
		{
			DEBUG_MSG("Copy symbol - '" + virtualSymName + "' already in symbols table. Just update it...");
			return true;
		}

		if (!parser->symtab.Insert(sym))
			DEBUG_MSG("Copy symbol - Symbol '" + virtualSymName + "' seems not inserted in table!");

		index = parser->GetIndex(virtualSymName);
		DEBUG_MSG_IF((index == Invalid), "Copy symbol - Symbol '" + virtualSymName + "' not created in table!");

		if (isNewSym && (index == Invalid)) SAFE_DELETE(sym);
		return index == Invalid;
	}


	ItemExtensionDataStorage::ItemExtensionDataStorage() 
	{
		Data = Map<unsigned int, ItemExtension*>();
		Indexer = Array<ItemExtensionIndexer>();
		Indexer_InstanceName = Map<zSTRING, const ItemExtension*>();
		ItemsCount = Data.GetNum();
	}

	template <class T>
	inline void ItemExtensionDataStorage::GetImpl(Array<T*>& buffer, const ItemExtensionIndexer& indexer, const uint bufferSize)
	{
		if (Indexer.IsEmpty()) return;

		uint startIndex = Indexer.SearchEqualSorted(indexer);
		if (startIndex == Invalid) return;
		while (startIndex > 0U && Indexer[startIndex - 1U] == indexer) --startIndex;

		const uint count = Indexer.GetNum();
		uint pickCount = 0U;
		for (uint i = startIndex; i < count; ++i)
		{
			const auto& elem = Indexer[i];
			if (elem != indexer) break;

			if (elem.Data)
			{
				buffer.InsertEnd(const_cast<ItemExtension*>(elem.Data));
				++pickCount;
			}
			if (bufferSize && pickCount >= bufferSize) break;
		}
	}

	ItemExtension* ItemExtensionDataStorage::Get(const unsigned int uId) 
	{
		auto* pair = Data.GetSafePair(uId);
		if (pair) return pair->GetValue();
		return Null;
	}
	ItemExtension* ItemExtensionDataStorage::Get(const zSTRING& instanceName)
	{
		auto* pair = Indexer_InstanceName.GetSafePair(instanceName);
		if (pair) return const_cast<ItemExtension*>(pair->GetValue());
		return Null;
	}
	ItemExtension* ItemExtensionDataStorage::Get(const ItemExtensionIndexer& indexer)
	{
		const uint index = Indexer.SearchEqualSorted(indexer);
		return (index != Invalid) ? const_cast<ItemExtension*>(Indexer[index].Data) : Null;
	}
	ItemExtension* ItemExtensionDataStorage::Get(oCItem* item)
	{
		const zSTRING instanceName = item->GetInstanceName();
		return item ? Get(instanceName) : Null;
	}
	void ItemExtensionDataStorage::Get(Array<ItemExtension*>& buffer, const ItemExtensionIndexer& indexer, const uint bufferSize) { GetImpl(buffer, indexer, bufferSize); }
	ItemExtension* ItemExtensionDataStorage::Get(byte type, byte cls, byte subClass, uflag64 flags, int level, int rank, int power, int levelDelta, int powerDelta)
	{
		const ItemExtensionIndexer indexer = ItemExtensionIndexer(type, cls, subClass, flags, level, rank, power, levelDelta, powerDelta);
		return Get(indexer);
	}
	void ItemExtensionDataStorage::Get(Array<ItemExtension*>& buffer, const uint bufferSize, byte type, byte cls, byte subClass, uflag64 flags, int level, int rank, int power, int levelDelta, int powerDelta)
	{
		const ItemExtensionIndexer indexer = ItemExtensionIndexer(type, cls, subClass, flags, level, rank, power, levelDelta, powerDelta);
		Get(buffer, indexer, bufferSize);
	}

	const ItemExtension* ItemExtensionDataStorage::GetSafe(const unsigned int uId) { return Get(uId); }
	const ItemExtension* ItemExtensionDataStorage::GetSafe(const zSTRING& instanceName) { return Get(instanceName); }
	const ItemExtension* ItemExtensionDataStorage::GetSafe(const ItemExtensionIndexer& indexer)
	{
		const uint index = Indexer.SearchEqualSorted(indexer);
		return (index != Invalid) ? Indexer[index].Data : Null;
	}
	const ItemExtension* ItemExtensionDataStorage::GetSafe(oCItem* item) { return Get(item); }
	void ItemExtensionDataStorage::GetSafe(Array<const ItemExtension*>& buffer, const ItemExtensionIndexer& indexer, const uint bufferSize) { GetImpl(buffer, indexer, bufferSize); }
	const ItemExtension* ItemExtensionDataStorage::GetSafe(byte type, byte cls, byte subClass, uflag64 flags, int level, int rank, int power, int levelDelta, int powerDelta)
	{
		const ItemExtensionIndexer indexer = ItemExtensionIndexer(type, cls, subClass, flags, level, rank, power, levelDelta, powerDelta);
		return GetSafe(indexer);
	}
	void ItemExtensionDataStorage::GetSafe(Array<const ItemExtension*>& buffer, const uint bufferSize, byte type, byte cls, byte subClass, uflag64 flags, int level, int rank, int power, int levelDelta, int powerDelta)
	{
		const ItemExtensionIndexer indexer = ItemExtensionIndexer(type, cls, subClass, flags, level, rank, power, levelDelta, powerDelta);
		GetSafe(buffer, indexer, bufferSize);
	}


	inline void ItemExtensionDataStorage::InsertIndex(const ItemExtension* itemExtension)
	{
		Indexer.InsertSorted(ItemExtensionIndexer(itemExtension));
		Indexer_InstanceName.Insert(itemExtension->InstanceName, itemExtension);
	}

	bool ItemExtensionDataStorage::Insert(ItemExtension* itemExtension) 
	{
		if (!itemExtension)
		{
			DEBUG_MSG("ItemExtensionDataStorage::Insert: itemExtension is null!");
			return false;
		}

		itemExtension->InstanceName = NormalizeInstanceName(itemExtension->InstanceName);
		const ItemExtension* uIdStorageExtension = GetSafe(itemExtension->UId);
		const ItemExtension* instanceNameStorageExtension = GetSafe(itemExtension->InstanceName);

		DEBUG_MSG_IF(uIdStorageExtension, "ItemExtensionDataStorage::Insert: itemExtension with such uid already exists in storage! UId: " + Z static_cast<int>(itemExtension->UId));
		DEBUG_MSG_IF(instanceNameStorageExtension, "ItemExtensionDataStorage::Insert: itemExtension with such instance name already exists in storage! InstanceName: " + itemExtension->InstanceName);
		if (uIdStorageExtension || instanceNameStorageExtension) return false;

		int parserId = Invalid;
		if (!CreateVirtualSymbol(itemExtension->BaseInstanceName, itemExtension->InstanceName, parserId))
		{
			DEBUG_MSG("ItemExtensionDataStorage::Insert: fail to create virtual symbol!");
			return false;
		}

		Data.Insert(itemExtension->UId, itemExtension);
		InsertIndex(itemExtension);
		ItemsCount = Data.GetNum();
		return true;
	}

	void ItemExtensionDataStorage::Clear() 
	{
		DEBUG_MSG("ItemExtensionDataStorage::Clear: clear items storage...");
		for (auto& pair : Data)
		{
			auto& value = pair.GetValue();
			SAFE_DELETE(value);
		}
		
		Data.Clear();
		ItemsCount = Data.GetNum();
		Indexer.Clear();
		Indexer_InstanceName.Clear();
	}


	void ItemExtensionDataStorage::Archive(zCArchiver& arc) 
	{
		DEBUG_MSG("ItemExtensionDataStorage::Archive: start write ItemsExtension storage...");
		ItemsCount = Data.GetNum();
		uint writeTotal = 0U;

		arc.WriteRaw("ItemExtensionsCount", &ItemsCount, sizeof(uint));
		for (auto& pair : Data)
		{
			ItemExtension* extension = pair.GetValue();
			if (!extension) continue;
			extension->Archive(arc);
			++writeTotal;
		}		
		DEBUG_MSG_IF(writeTotal != ItemsCount,"ItemExtensionDataStorage::Archive: writed " + Z(static_cast<int>(writeTotal)) + "/" + Z(static_cast<int>(ItemsCount)) + "items!");
	}

	void ItemExtensionDataStorage::UnArchive(zCArchiver& arc) 
	{
		DEBUG_MSG("ItemExtensionDataStorage::UnArchive: start read ItemsExtension storage...");
		uint readTotal = 0U;

		arc.ReadRaw("ItemExtensionsCount", &ItemsCount, sizeof(uint));
		for (uint i = 0; i < ItemsCount; ++i)
		{
			ItemExtension* extension = new ItemExtension();
			extension->UnArchive(arc);
			if (Insert(extension))
				++readTotal;
		}
		DEBUG_MSG_IF(readTotal != ItemsCount, "ItemExtensionDataStorage::UnArchive: readed " + Z(static_cast<int>(readTotal)) + "/" + Z(static_cast<int>(ItemsCount)) + "items!");
	}

	ItemExtensionDataStorage::~ItemExtensionDataStorage() { Clear(); }
}