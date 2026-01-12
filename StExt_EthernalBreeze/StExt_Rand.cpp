#include <ctime>
#include <StExt_Rand.h>

namespace Gothic_II_Addon
{
    const char RandomCharset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const int RandomStringMaxLength = 64;

    unsigned int StExt_Rand::g_seed = static_cast<unsigned int>(time(nullptr));
    void StExt_Rand::Seed(unsigned int seed) { g_seed = seed ? seed : 1; }

    unsigned int StExt_Rand::Next()
    {
        g_seed ^= g_seed << 13;
        g_seed ^= g_seed >> 17;
        g_seed ^= g_seed << 5;
        return g_seed;
    }

    int StExt_Rand::Next(int max) { return max <= 0 ? 0 : static_cast<int>(Next() % (static_cast<unsigned int>(max) + 1)); }
    float StExt_Rand::Next(float max) { return max <= 0.0f ? 0.0f : (Next() / static_cast<float>(UINT32_MAX)) * max; }
    unsigned int StExt_Rand::Index(unsigned int count) { return count <= 1 ? 0 : Next() % count; }

    int StExt_Rand::Range(int min, int max) { if (max < min) std::swap(min, max); return static_cast<int>(Next() % (max - min + 1)) + min; }
    float StExt_Rand::Range(float min, float max) { if (max < min) std::swap(min, max); return min + (Next() / float(0xFFFFFFFFu)) * (max - min); }

    float StExt_Rand::SymmetricRange(float value) { return Range(-value, value); }
    int StExt_Rand::SymmetricRange(int value) { return Range(-value, value); }

    bool StExt_Rand::Bool() { return (Next() & 1u) != 0; }
    bool StExt_Rand::Percent(int percent) { if (percent <= 0) return false; if (percent >= 100) return true; return Range(0, 100) <= percent; }
    bool StExt_Rand::Percent(float percent) { if (percent <= 0.0f) return false; if (percent >= 100.0f) return true; return Range(0.0f, 100.0f) <= percent; }
    bool StExt_Rand::Permille(int permille) { if (permille <= 0) return false; if (permille >= 1000) return true; return Range(1, 1000) <= permille; }

    zSTRING StExt_Rand::RandomString(int length)
    {
        if (length <= 0) return zSTRING();
        if (length > RandomStringMaxLength) length = RandomStringMaxLength;

        char buff[RandomStringMaxLength + 1];
        int charsetSize = sizeof(RandomCharset) - 1;
        for (int i = 0; i < length; ++i) buff[i] = RandomCharset[Index(charsetSize)];
        buff[length] = '\0';
        return zSTRING(buff);
    }

    void StExt_Rand::ShuffleArray(Array<int>& arr) { ShuffleArrayImpl(arr); } 
    void StExt_Rand::ShuffleArray(Array<unsigned int>& arr) { ShuffleArrayImpl(arr); } 
    void StExt_Rand::ShuffleArray(Array<long>& arr) { ShuffleArrayImpl(arr); } 
    void StExt_Rand::ShuffleArray(Array<unsigned long>& arr) { ShuffleArrayImpl(arr); } 
    void StExt_Rand::ShuffleArray(Array<float>& arr) { ShuffleArrayImpl(arr); } 
    void StExt_Rand::ShuffleArray(Array<byte>& arr) { ShuffleArrayImpl(arr); } 
    void StExt_Rand::ShuffleArray(Array<zSTRING>& arr) { ShuffleArrayImpl(arr); }

    void StExt_Rand::RandomSequence(Array<int>& arr, int min, int max) { RandomSequenceImpl(arr, min, max); }
    void StExt_Rand::RandomSequence(Array<unsigned int>& arr, unsigned int min, unsigned int max) { RandomSequenceImpl(arr, min, max); }
    void StExt_Rand::RandomSequence(Array<long>& arr, long min, long max) { RandomSequenceImpl(arr, min, max); }
    void StExt_Rand::RandomSequence(Array<unsigned long>& arr, unsigned long min, unsigned long max) { RandomSequenceImpl(arr, min, max); }
    void StExt_Rand::RandomSequence(Array<float>& arr, float min, float max) { RandomSequenceImpl(arr, min, max); }
    void StExt_Rand::RandomSequence(Array<byte>& arr, byte min, byte max) { RandomSequenceImpl(arr, min, max); }
}
