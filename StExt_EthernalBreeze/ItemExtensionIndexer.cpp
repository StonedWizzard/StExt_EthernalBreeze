#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    ItemExtensionIndexer::ItemExtensionIndexer(const ItemExtension* extension)
        : Data(extension),
        Type(extension ? extension->Type : (byte)ItemType::Unknown),
        Class(extension ? extension->Class : (byte)ItemClass::Unknown),
        SubClass(extension ? extension->SubClass : (byte)ItemSubClass::Unknown),
        Flags(extension ? extension->Flags : 0UL),
        Level(extension ? extension->Level : Invalid),
        Rank(extension ? extension->Rank : Invalid),
        Power(extension ? extension->Properties[(int)ItemProperty::InitialPower]: Invalid),
        LevelMax(Invalid),
        PowerMax(Invalid)
    {}

    ItemExtensionIndexer::ItemExtensionIndexer(byte type, byte cls, byte subClass, uflag64 flags, int level, int rank, int power, int levelDelta, int powerDelta)
        : Data(nullptr), Type(type), Class(cls), SubClass(subClass),
        Flags(flags), Level(level), Rank(rank), Power(power),
        LevelMax(Level == Invalid ? Invalid : Level + levelDelta),
        PowerMax(Power == Invalid ? Invalid : Power + powerDelta)
    {}

    bool ItemExtensionIndexer::operator==(const ItemExtensionIndexer& other) const noexcept
    {
        // Strict comparison for two real indexers (e.g., during sorting or validation)
        if (Data && other.Data) 
        {
            return Type == other.Type && Class == other.Class && SubClass == other.SubClass &&
                Flags == other.Flags && Level == other.Level && Rank == other.Rank && Power == other.Power;
        }
        // Relaxed comparison for search
        else
        {
            // Determine which is the search key (nullptr) and which is the item
            const ItemExtensionIndexer* search = !Data ? this : &other;
            const ItemExtensionIndexer* item = !Data ? &other : this;

            if (search->Data == nullptr && item->Data == nullptr)
            {
                return Type == other.Type && Class == other.Class && SubClass == other.SubClass &&
                    Flags == other.Flags && Level == other.Level && Rank == other.Rank && Power == other.Power;
            }

            if (search->Type != (byte)ItemType::Unknown && search->Type != item->Type) return false;
            if (search->Class != (byte)ItemClass::Unknown && search->Class != item->Class) return false;
            if (search->SubClass != (byte)ItemSubClass::Unknown && search->SubClass != item->SubClass) return false;
            if (search->Rank != Invalid && search->Rank != item->Rank) return false;

            if (search->Level != Invalid)
            {
                if (search->LevelMax != Invalid && search->LevelMax >= search->Level) {
                    if (item->Level < search->Level || item->Level > search->LevelMax) return false;
                }
                else { if (item->Level != search->Level) return false; }
            }

            if (search->Power != Invalid)
            {
                if (search->PowerMax != Invalid && search->PowerMax >= search->Power) {
                    if (item->Power < search->Power || item->Power > search->PowerMax) return false;
                }
                else { if (item->Power != search->Power) return false; }
            }

            if (search->Flags != 0UL && !HasFlag(search->Flags, item->Flags)) return false;
            return true;
        }
        return false;
    }
    bool ItemExtensionIndexer::operator!=(const ItemExtensionIndexer& other) const noexcept { return !(*this == other); }

    bool ItemExtensionIndexer::operator<(const ItemExtensionIndexer& other) const noexcept
    {
        if (Type != other.Type) return Type < other.Type;
        if (Class != other.Class) return Class < other.Class;
        if (SubClass != other.SubClass) return SubClass < other.SubClass;
        if (Level != other.Level) return Level < other.Level;
        if (Rank != other.Rank) return Rank < other.Rank;
        if (Power != other.Power) return Power < other.Power;
        if (Flags != other.Flags) return Flags < other.Flags;
        if (LevelMax != other.LevelMax) return LevelMax < other.LevelMax;
        if (PowerMax != other.PowerMax) return PowerMax < other.PowerMax;
        return false;
    }

    bool ItemExtensionIndexer::operator>(const ItemExtensionIndexer& other) const noexcept { return other < *this; }
    bool ItemExtensionIndexer::operator<=(const ItemExtensionIndexer& other) const noexcept { return !(other < *this); }
    bool ItemExtensionIndexer::operator>=(const ItemExtensionIndexer& other) const noexcept { return !(*this < other); }
}