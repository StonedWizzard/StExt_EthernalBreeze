#pragma once
#include <UnionAfx.h>

namespace Gothic_II_Addon
{
    class StExt_Rand
    {
    private:
        template<typename T> 
        static void ShuffleArrayImpl(T& arr) 
        { 
            int count = arr.GetNum(); 
            if (count <= 1) return; 
            for (int i = count - 1; i > 0; --i) 
            { 
                unsigned int j = Next() % (i + 1); 
                if (i != static_cast<int>(j)) 
                { 
                    auto tmp = arr[i]; 
                    arr[i] = arr[j]; 
                    arr[j] = tmp; 
                } 
            } 
        } 
        
        template<typename T, typename U>
        static void RandomSequenceImpl(T& arr, U min, U max)
        {
            if (min > max) std::swap(min, max);
            const uint outCount = arr.GetNum();
            uint rangeCount = static_cast<uint>(max - min + 1);

            if (outCount == 0 || rangeCount == 0 || outCount > rangeCount) return;

            Array<U> available;
            for (uint i = 0; i < rangeCount; ++i)
                available.InsertEnd(static_cast<U>(min + i));

            uint remaining = available.GetNum();
            for (uint n = 0; n < outCount; ++n)
            {
                uint pickIndex = Next() % remaining;
                arr[n] = available[pickIndex];
                available[pickIndex] = available[remaining - 1];
                --remaining;
            }
        }

    public:
        static unsigned int g_seed;
        static void Seed(unsigned int seed);

        static unsigned int Next();
        static int Next(int max);
        static float Next(float max);
        static unsigned int Index(unsigned int count);

        static int Range(int min, int max);
        static float Range(float min, float max);
        static float SymmetricRange(float value);
        static int SymmetricRange(int value);

        static bool Bool();
        static bool Percent(int percent);
        static bool Percent(float percent);
        static bool Permille(int permille);
        static zSTRING RandomString(int length);

        static void ShuffleArray(Array<int>& arr);
        static void ShuffleArray(Array<unsigned int>& arr);
        static void ShuffleArray(Array<long>& arr);
        static void ShuffleArray(Array<unsigned long>& arr);
        static void ShuffleArray(Array<float>& arr);
        static void ShuffleArray(Array<byte>& arr);
        static void ShuffleArray(Array<zSTRING>& arr);

        static void RandomSequence(Array<int>& arr, int min, int max);
        static void RandomSequence(Array<unsigned int>& arr, unsigned int min, unsigned int max);
        static void RandomSequence(Array<long>& arr, long min, long max);
        static void RandomSequence(Array<unsigned long>& arr, unsigned long min, unsigned long max);
        static void RandomSequence(Array<float>& arr, float min, float max);
        static void RandomSequence(Array<byte>& arr, byte min, byte max);
    };
}
