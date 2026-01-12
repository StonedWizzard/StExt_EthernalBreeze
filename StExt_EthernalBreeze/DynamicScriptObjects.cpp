#include <UnionAfx.h>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    constexpr size_t BUFFER_SIZE = 1 * (1024 * 1024);
    static uint8_t ObjectsMemPool[BUFFER_SIZE];
    static size_t MemPoolCurrentOffset = 0;

    struct DynamicObjectMeta 
    {
        void* ptr;
        size_t size;
        bool used;

        DynamicObjectMeta() : ptr(nullptr), size(0), used(false) { }
        DynamicObjectMeta(void* p, size_t s, bool u) : ptr(p), size(s), used(u) { }
    };

    //static std::vector<DynamicObjectMeta> ObjectsTable = { };
    static Array<DynamicObjectMeta> ObjectsTable = { };

    void* AllocateDynamicObject(size_t size)
    {
        size = (size + 15) & ~15;

        for (auto& entry : ObjectsTable) 
        {
            if (!entry.used && entry.size >= size) 
            {
                entry.used = true;
                return entry.ptr;
            }
        }

        if (MemPoolCurrentOffset + size > BUFFER_SIZE) 
        {
            DEBUG_MSG("AllocateDynamicObject : Can't allocate new dynamic object - MemPool if overflowed!");
            return nullptr;
        }

        void* ptr = ObjectsMemPool + MemPoolCurrentOffset;
        DynamicObjectMeta meta = DynamicObjectMeta(ptr, size, true);
        ObjectsTable.InsertEnd(meta);
        MemPoolCurrentOffset += size;

        DEBUG_MSG("AllocateDynamicObject : Creatre pointer: " + Z((int)ptr) + " With size: " + Z((int)size));
        return ptr;
    }

    bool FreeDynamicObject(void* ptr)
    {
        for (auto& entry : ObjectsTable)
        {
            if (entry.ptr == ptr) 
            {
                entry.used = false;
                return true;
            }
        }
        DEBUG_MSG("FreeDynamicObject : Attempted to free unknown pointer: " + Z (int)ptr);
        return false;
    }

    bool IsObjectsTableAllocated(void* ptr) 
    {
        for (const auto& entry : ObjectsTable) {
            if (entry.ptr == ptr && entry.used) return true;
        }
        return false;
    }

    bool WriteToDynamicObject(void* basePtr, size_t offset, int value)
    {
        for (auto& entry : ObjectsTable)
        {
            if (entry.ptr == basePtr && entry.used)
            {
                if (offset + sizeof(int) > entry.size)
                {
                    DEBUG_MSG("WriteToDynamicObject: offset out of bounds!");
                    return false;
                }

                int* p = reinterpret_cast<int*>((uint8_t*)basePtr + offset);
                *p = value;
                return true;
            }
        }

        DEBUG_MSG("WriteToDynamicObject: unknown pointer!");
        return false;
    }

    bool ReadFormDynamicObject(void* basePtr, size_t offset, int& outValue)
    {
        for (auto& entry : ObjectsTable)
        {
            if (entry.ptr == basePtr && entry.used)
            {
                if (offset + sizeof(int) > entry.size)
                {
                    DEBUG_MSG("ReadFormDynamicObject: offset out of bounds!");
                    return false;
                }

                int* p = reinterpret_cast<int*>((uint8_t*)basePtr + offset);
                outValue = *p;
                return true;
            }
        }

        DEBUG_MSG("ReadFormDynamicObject: unknown pointer!");
        return false;
    }
}