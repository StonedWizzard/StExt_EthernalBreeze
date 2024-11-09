#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    C_EnchantmentData::C_EnchantmentData()
    {
        Preffix = zSTRING();
        Affix = zSTRING();
        Suffix = zSTRING();
        VisualEffect = zSTRING();

        for (int i = 0; i < EnchantStatsMax; i++)
            StatId[i] = Invalid;

        for (int i = 0; i < EnchantAbilityMax; i++)
            AbilityId[i] = Invalid;
    }

    C_ItemData::C_ItemData()
    {
        InstanceName = zSTRING();
        BaseInstanceName = zSTRING();
    }

    void C_EnchantmentData::Archive(zCArchiver& arc)
    {
        arc.WriteInt("UId", UId);

        arc.WriteString("Preffix", Preffix);
        arc.WriteString("Affix", Affix);
        arc.WriteString("Suffix", Suffix);

        arc.WriteInt("Type", Type);
        arc.WriteInt("Flags", Flags);
        arc.WriteInt("Level", Level);
        arc.WriteInt("Rank", Rank);
        arc.WriteInt("SocketsUsed", SocketsUsed);
        arc.WriteInt("SocketsMax", SocketsMax);
        arc.WriteInt("Cost", Cost);

        arc.WriteRaw("AbilityId", AbilityId, sizeof(int) * EnchantAbilityMax);
        arc.WriteRaw("AbilityChance", AbilityChance, sizeof(int) * EnchantAbilityMax);
        arc.WriteRaw("AbilityValue", AbilityValue, sizeof(int) * EnchantAbilityMax);
        arc.WriteRaw("AbilityDuration", AbilityDuration, sizeof(int) * EnchantAbilityMax);

        arc.WriteRaw("StatId", StatId, sizeof(int) * EnchantStatsMax);
        arc.WriteRaw("StatValue", StatValue, sizeof(int) * EnchantStatsMax);
        arc.WriteRaw("StatDuration", StatDuration, sizeof(int) * EnchantStatsMax);

        arc.WriteInt("ExtraFlags", ExtraFlags);
        arc.WriteInt("AdditionalFlags", AdditionalFlags);        
        arc.WriteInt("ExtraMainflag", ExtraMainflag);

        arc.WriteRaw("CondAtr", CondAtr, sizeof(int) * EnchantConditionsMax);
        arc.WriteRaw("CondValue", CondValue, sizeof(int) * EnchantConditionsMax);

        arc.WriteInt("DamageTypes", DamageTypes);
        arc.WriteInt("DamageTotal", DamageTotal);
        arc.WriteRaw("Damage", Damage, sizeof(int) * oEDamageIndex_MAX);
        arc.WriteRaw("Protection", Protection, sizeof(int) * oEDamageIndex_MAX);
        arc.WriteInt("Range", Range);

        arc.WriteString("VisualEffect", VisualEffect);
    }

    void C_EnchantmentData::Unarchive(zCArchiver& arc)
    {
        arc.ReadInt("UId", UId);

        arc.ReadString("Preffix", Preffix);
        arc.ReadString("Affix", Affix);
        arc.ReadString("Suffix", Suffix);

        arc.ReadInt("Type", Type);
        arc.ReadInt("Flags", Flags);
        arc.ReadInt("Level", Level);
        arc.ReadInt("Rank", Rank);
        arc.ReadInt("SocketsUsed", SocketsUsed);
        arc.ReadInt("SocketsMax", SocketsMax);
        arc.ReadInt("Cost", Cost);

        arc.ReadRaw("AbilityId", &AbilityId, sizeof(int) * EnchantAbilityMax);
        arc.ReadRaw("AbilityChance", &AbilityChance, sizeof(int) * EnchantAbilityMax);
        arc.ReadRaw("AbilityValue", &AbilityValue, sizeof(int) * EnchantAbilityMax);
        arc.ReadRaw("AbilityDuration", &AbilityDuration, sizeof(int) * EnchantAbilityMax);

        arc.ReadRaw("StatId", &StatId, sizeof(int) * EnchantStatsMax);
        arc.ReadRaw("StatValue", &StatValue, sizeof(int) * EnchantStatsMax);
        arc.ReadRaw("StatDuration", &StatDuration, sizeof(int) * EnchantStatsMax);

        arc.ReadInt("ExtraFlags", ExtraFlags);
        arc.ReadInt("AdditionalFlags", AdditionalFlags);        
        arc.ReadInt("ExtraMainflag", ExtraMainflag);

        arc.ReadRaw("CondAtr", &CondAtr, sizeof(int) * EnchantConditionsMax);
        arc.ReadRaw("CondValue", &CondValue, sizeof(int) * EnchantConditionsMax);

        arc.ReadInt("DamageTypes", DamageTypes);
        arc.ReadInt("DamageTotal", DamageTotal);
        arc.ReadRaw("Damage", &Damage, sizeof(int) * oEDamageIndex_MAX);
        arc.ReadRaw("Protection", &Protection, sizeof(int) * oEDamageIndex_MAX);
        arc.ReadInt("Range", Range);

        arc.ReadString("VisualEffect", VisualEffect);
    }

    void C_ItemData::Archive(zCArchiver& arc)
    {
        arc.WriteString("InstanceName", InstanceName);
        arc.WriteString("BaseInstanceName", BaseInstanceName);

        arc.WriteInt("Type", Type);
        arc.WriteInt("Flags", Flags);
        arc.WriteInt("EnchantmentId", EnchantmentId);
        arc.WriteInt("Power", Power);        
    }

    void C_ItemData::Unarchive(zCArchiver& arc)
    {
        arc.ReadString("InstanceName", InstanceName);
        arc.ReadString("BaseInstanceName", BaseInstanceName);

        arc.ReadInt("Type", Type);
        arc.ReadInt("Flags", Flags);
        arc.ReadInt("EnchantmentId", EnchantmentId);
        arc.ReadInt("Power", Power);        
    }
}