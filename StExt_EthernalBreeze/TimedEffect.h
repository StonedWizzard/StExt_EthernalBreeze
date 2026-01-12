#include <UnionAfx.h>

namespace Gothic_II_Addon
{
    class TimedEffect
    {
    public:
        oCNpc* Self;
        oCNpc* Other;
        uint ElapsedTime;
        zSTRING FunctionName;
        int Function;

        struct 
        {
            int Delay;
            int Enabled;
            int SelfUId;
            int OtherUId;
            int AIVariables[32];
            int Flags;
            int EffectId;
        }ScriptInstance;

        TimedEffect();
        void Archive(zCArchiver& arc);
        void UnArchive(zCArchiver& arc);
        ~TimedEffect();
    };

    extern Array<TimedEffect*> TimedEffects;

    void TimedEffectsLoop();
    void FinalizeTimedEffects();
    void ClearTimedEffects();
    void SaveTimedEffects();
    void LoadTimedEffects();

    // Externals
    int CreateTimedEffect();
    int GetTimedEffectsCount();
    int GetTimedEffectByIndex();
    int GetTimedEffectByNpc();
}