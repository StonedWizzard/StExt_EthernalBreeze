#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    struct C_TimedEffect
    {
        zSTRING FunctionName;
        int Function;

        oCNpc* Self;
        oCNpc* Other;

        struct 
        {
            int Delay;
            int Enabled;
            int SelfNpcUid;
            int OtherNpcUid;
            int AIVariables[20];
            int Flags;
            int EffectId;
        }
        Parser;

        C_TimedEffect();
        bool CallTrigger();
        void SetAsInstance(zSTRING instName);
        void Archive(zCArchiver& arc);
        void Unarchive(zCArchiver& arc);
        ~C_TimedEffect();

        static C_TimedEffect* CreateTimedEffect(zSTRING funcName, int delay);
        static void DoTimedEffectsLoop();
        static void LoadTimedEffects(zCArchiver& arc);
        static void SaveTimedEffects(zCArchiver& arc);
        static void ClearTimedEffects();
        static void RegisterClassOffset();
        //static void SetAfterLoadEffect(int delay);

        static Array<C_TimedEffect*> TimedEffectScripts;
    };

    Array<C_TimedEffect*> C_TimedEffect::TimedEffectScripts;
}