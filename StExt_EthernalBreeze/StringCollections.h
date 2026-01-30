#pragma once
#include <UnionAfx.h>

namespace Gothic_II_Addon
{
    template<class TValue>
    struct StringMapNode
    {
        zSTRING key;
        TValue  value;
        StringMapNode* next;

        StringMapNode(const zSTRING& k, const TValue& v) : key(k), value(v), next(Null) {}
    };

    template<class TValue>
    class StringMap
    {
    private:
        StringMapNode<TValue>** buckets;
        uint bucketCount;
        uint size;

        uint Hash(const zSTRING& key) const;
        StringMap(const StringMap&) = delete;
        StringMap& operator=(const StringMap&) = delete;

    public:
        explicit StringMap(uint bucketCount = 512);
        ~StringMap();

        bool    Insert(const zSTRING& key, const TValue& value);
        TValue* Find(const zSTRING& key);
        bool    Remove(const zSTRING& key);
        void    Clear();
        uint    Size() const;
        bool    IsEmpty() const;
        bool    HasKey(const zSTRING& key);

        // ---------------- Iterator ----------------
        class Iterator
        {
        private:
            StringMap<TValue>* map;
            uint bucketIndex;
            StringMapNode<TValue>* node;

            void advanceToNextValid()
            {
                while (!node && bucketIndex < map->bucketCount) {
                    node = map->buckets[bucketIndex++];
                }
            }

        public:
            Iterator(StringMap<TValue>* m, uint idx, StringMapNode<TValue>* n) : map(m), bucketIndex(idx), node(n) {
                advanceToNextValid();
            }

            Iterator& operator++()
            {
                if (node) node = node->next;
                advanceToNextValid();
                return *this;
            }

            TValue& operator*() { return node->value; }
            TValue* operator->() { return &node->value; }

            bool operator!=(const Iterator& other) const { return node != other.node; }
        };

        Iterator begin() { return Iterator(this, 0, Null); }
        Iterator end() { return Iterator(this, bucketCount, Null); }
    };


    //------------------------------------------------------------------------------------

    template<class TValue>
    StringMap<TValue>::StringMap(uint bucketCnt) : bucketCount(bucketCnt), size(0)
    {
        buckets = new StringMapNode<TValue>*[bucketCount];
        for (uint i = 0; i < bucketCount; ++i) buckets[i] = Null;
    }

    template<class TValue>
    StringMap<TValue>::~StringMap()
    {
        Clear();
        delete[] buckets;
    }

    template<class TValue>
    void StringMap<TValue>::Clear()
    {
        for (uint i = 0; i < bucketCount; ++i)
        {
            StringMapNode<TValue>* node = buckets[i];
            while (node)
            {
                StringMapNode<TValue>* next = node->next;
                delete node;
                node = next;
            }
            buckets[i] = Null;
        }
        size = 0U;
    }

    template<class TValue>
    uint StringMap<TValue>::Size() const { return size; }

    template<class TValue>
    bool StringMap<TValue>::IsEmpty() const { return size == 0U; }

    template<class TValue>
    uint StringMap<TValue>::Hash(const zSTRING& key) const
    {
        const char* str = key.ToChar();
        uint hash = 5381;

        while (*str) 
        {
            hash = ((hash << 5) + hash) ^ (uint)(*str);
            ++str;
        }
        return hash;
    }

    template<class TValue>
    bool StringMap<TValue>::Insert(const zSTRING& key, const TValue& value) 
    {
        uint idx = Hash(key) % bucketCount;
        StringMapNode<TValue>* node = buckets[idx];

        while (node) 
        {
            if (node->key == key) return false;
            node = node->next;
        }

        StringMapNode<TValue>* n = new StringMapNode<TValue>(key, value);
        n->next = buckets[idx];
        buckets[idx] = n;
        ++size;
        return true;
    }

    template<class TValue>
    TValue* StringMap<TValue>::Find(const zSTRING& key) 
    {
        uint idx = Hash(key) % bucketCount;
        StringMapNode<TValue>* node = buckets[idx];

        while (node) 
        {
            if (node->key == key)
                return &node->value;
            node = node->next;
        }
        return Null;
    }

    template<class TValue>
    bool StringMap<TValue>::HasKey(const zSTRING& key) { return Find(key) != Null; }

    template<class TValue>
    bool StringMap<TValue>::Remove(const zSTRING& key) 
    {
        uint idx = Hash(key) % bucketCount;
        StringMapNode<TValue>* node = buckets[idx];
        StringMapNode<TValue>* prev = Null;

        while (node) 
        {
            if (node->key == key) 
            {
                if (prev) prev->next = node->next;
                else buckets[idx] = node->next;

                delete node;
                --size;
                return true;
            }
            prev = node;
            node = node->next;
        }
        return false;
    }
}